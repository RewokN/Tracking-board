#include "GPS.h"
#include "GSM.h"
#include <msp430g2553.h>
#include <timer.h>
#include "port1.h"
#include "uart.h"
#include "config.h"


uint8 message_buffer[100];          // Buffer for storing data

boolean oneSecondPassed = FALSE;    // Flag for timer
boolean gsmMessageSent = FALSE;     // Flag for checking if the message has been sent
boolean GPSExtractFlag = FALSE;     // Flag for successfully extracted data from GPS
uint8 gsmMessage[100];              // Buffer for storing gsmMessage Cred ca poate fi scos.

boolean isGSMinitialized = FALSE;   // Flag for the first initialization for GSM
boolean isGPSinitialized = FALSE;   // Flag for the first initialization for GPS
uint8 extractedData_main = 0;

extern GPS_DATA gps_data;
extern GSM_DATA gsm_data;
extern extractMessage extractState;

typedef enum
{
    INIT_GSM = 0, INIT_GPS, READ_FROM_GPS, READ_FROM_GSM, SEND_TO_GSM
} applicationStateType;

applicationStateType applicationState = INIT_GSM;
int main(void)
{
    /**GLOBAL SETTINGS**/
    WDTCTL = WDTPW | WDTHOLD;        // stop watchdog timer
    BCSCTL1 = CALBC1_1MHZ;           // Set DCO to 1Mhz
    DCOCTL = CALDCO_1MHZ;            // Set DCO to 1Mhz

    /************************************************/
    port_init();
    uart_init();
    timer_init();
    __enable_interrupt();
    uint8 *substring;

    while (1)
    {   //Initiate the GSM
        switch (applicationState)
        {
        case INIT_GSM:
            mux_switch(TO_GSM);                     // Switch the MUX TO_GSM
            if (FALSE == isGSMinitialized)
            {
                if (oneSecondPassed)
                { // Send every message after one second, the module will respond anyway.                {
                    oneSecondPassed = FALSE;       // Clear oneSecondPassed flag
                    isGSMinitialized = GSM_init();  // Check the
                    if (isGSMinitialized)
                    {
                        applicationState = INIT_GPS;
                    }
                }
            }
            break;
        case SEND_TO_GSM:
            mux_switch(TO_GSM);
            if ((gsmMessageSent == FALSE) && (isGSMinitialized != FALSE))
            {
                if (oneSecondPassed)
                {
                    oneSecondPassed = FALSE;
                    gsmMessageSent = GSM_sendMessage(gsmMessage, &gps_data,
                                                     GPSExtractFlag);
                    if (gsmMessageSent)
                        applicationState = READ_FROM_GSM;
                }
            }
            break;
        case READ_FROM_GSM:
            mux_switch(TO_GSM);
            if ((FALSE != readMessage(message_buffer))
                    && (isGSMinitialized != FALSE))
            {
                if (strcmp((sint8*) message_buffer, "RING") == 0)
                    sendMessage("ATH");
                substring = (uint8*) strtok((sint8*) message_buffer, ",: ");
                if (strcmp((sint8*) substring, "+CMT") == 0)
                {
                    while (substring)
                    {
                        extractedData_main = gsm_getMessageDetails(substring);
                        substring = (uint8*) strtok(NULL, ",: ");
                    }
                    applicationState = READ_FROM_GPS;
                    extractState = CMT;
                }
            }
            break;
        case INIT_GPS:
            mux_switch(TO_GPS);
            if (FALSE == isGPSinitialized)
            {
                if (oneSecondPassed)
                {
                    oneSecondPassed = FALSE;
                    isGPSinitialized = GPS_init();
                    if (isGPSinitialized)
                    {
                        applicationState = SEND_TO_GSM;
                        strcpy((sint8*) gsmMessage, "Initializare Completa.");
                        gsmMessageSent = FALSE;
                        GPSExtractFlag = FALSE;
                    }
                }
            }
            break;
        case READ_FROM_GPS:
            mux_switch(TO_GPS);
            if ((FALSE != readMessage(message_buffer))
                    && (isGPSinitialized != FALSE))
            {
                substring = (uint8*) strtok((sint8*) message_buffer, ", ");
                if (strcmp((sint8*) substring, "$GPGLL") == 0)
                {
                    while (substring)
                    {
                        if (GPS_extract_data(substring))
                        {
                            gsmMessageSent = FALSE;
                            GPSExtractFlag = TRUE;
                            applicationState = SEND_TO_GSM;
                            break;
                        }
                        substring = (uint8*) strtok(NULL, ", ");
                    }
                }
            }
            break;
        default:
            break;
        }
        memset(message_buffer, 0, 100);     // Clear the message_buffer
    }
}

