#include <timer.h>
#include "GSM.h"

//variables
GSM_DATA gsm_data;
extern GPS_DATA gps_data;
extractMessage extractState = CMT;

// State machine to initiate GSM
boolean GSM_init(void)
{
    static GSM_initType initStateMachine = SEND_AT; // Initiate with the first enumeration variable;
    static boolean isGSMinitialized = FALSE;        // Return variable: False
    switch (initStateMachine)
    {
    case SEND_AT:                     // Send test command
        sendMessage("AT");
        initStateMachine = SEND_AT_CMGF;
        isGSMinitialized = FALSE;
        break;
    case SEND_AT_CMGF:                // Select message mode
        sendMessage("AT+CMGF=1");
        initStateMachine = SEND_AT_CNMI;
        isGSMinitialized = FALSE;
        break;
    case SEND_AT_CNMI:                // Setting for handling the first messages
        sendMessage("AT+CNMI=1,2,0,0,0");
        initStateMachine = FINISHED;
        isGSMinitialized = FALSE;
        break;
    case FINISHED:                    // Setting initial variables
        memset(gsm_data.gsm_number, 0u, PHONE_NUMBER_DIMENSION);    // Clear the phone number
        gsm_data.flags.latitude = FALSE;
        gsm_data.flags.longitude = FALSE;
        gsm_data.flags.time = FALSE;
        strcpy((sint8*) gsm_data.gsm_number, "\"+40753333214\""); // Initialize the gsm_number for sending an initial message.
        isGSMinitialized = TRUE;                  // Set the flag is all is good
        break;
    }
    return isGSMinitialized;
}

boolean GSM_sendMessage(uint8 * message, GPS_DATA *gps_data, boolean GPS_FLAG)
{
    static sendMessageState messageState = IDLE_TXT;
    static boolean retVal = FALSE;
    static uint8 at_num[22];
    uint8 endMessge = 26;
    switch (messageState)
    {
    case IDLE_TXT:                                  // Wait to stabilize,
        messageState = SEND_AT_CMGS_1;
        retVal = FALSE;
        break;
    case SEND_AT_CMGS_1:                            // Select message mode
        sendMessage("AT+CMGF=1");
        messageState = SEND_AT_CMGS_NR;
        retVal = FALSE;
        break;
    case SEND_AT_CMGS_NR:                           // Initialize the message
        memset(at_num,0,22);
        strcpy((sint8*) at_num, "AT+CMGS=");
        strcat((sint8*) at_num, (sint8*) gsm_data.gsm_number);
        sendMessage(at_num);
        messageState = SEND_MESSAGE;
        retVal = FALSE;
        memset(at_num,0,22);
        break;
    case SEND_MESSAGE:                         // Verify flags and write message
        if (FALSE != GPS_FLAG)
        {
            if (gsm_data.flags.latitude != FALSE)   // Send latitude
            {
                sendMessage("Latitudine:");
                sendMessage(gps_data->latitude_value);
                sendMessage("Directie:");
                uart_putc(gps_data->latitude_direction);
                sendMessage("");
                gsm_data.flags.latitude = FALSE;
            }
            if (gsm_data.flags.longitude != FALSE)  // Send longitude
            {
                sendMessage("Longituduine:");
                sendMessage(gps_data->longitude_value);
                sendMessage("Directie:");
                uart_putc(gps_data->longitude_direction);
                sendMessage("");
                gsm_data.flags.longitude = FALSE;
            }

            if (gsm_data.flags.time != FALSE)       // Send time
            {
                sendMessage("Timpul curent:");
                sendMessage(gps_data->curr_time);
                gsm_data.flags.time = FALSE;
            }
        }
        else                                        // If its not any flag
        {
            sendMessage(message);                   // Send echo message

        }
        messageState = SEND_END_MESSAGE_26;
        retVal = FALSE;
        break;
    case SEND_END_MESSAGE_26:                       // End message
        sendMessage(&endMessge);
        messageState = FINISHED_TXT;
        retVal = FALSE;
        break;
    case FINISHED_TXT:                              // Finish State successful
        retVal = TRUE;
        messageState = IDLE_TXT;
        break;
    }
    return retVal;
}
// Extract message details
uint8 gsm_getMessageDetails(uint8* substring)
{
    static uint8 ret_value = FALSE;

    switch (extractState)
    {
    case CMT:
        extractState = NUMBER_PH;
        ret_value = NO_DATA;
        break;
    case NUMBER_PH:                                 // Extract number phone
        memset(gsm_data.gsm_number, 0, 15);
        if (substring[1] == '+' && substring[2] == '4') // Check if one token has on second position '+'
        {
            strcpy((sint8*) gsm_data.gsm_number, (sint8*) substring);
            gsm_data.gsm_number[14] = '\0'; //make sure that the string is ended
            ret_value |= NUMBER_PH;
        }
        extractState = CMD;
        break;
    case CMD:                                       // Check syntax
        if (strcmp((char*) substring, "lat") == 0) // Check if one token is "lat"
        {
            gsm_data.flags.latitude = TRUE;         // Rise a specific flag
            ret_value |= LATITUDE;
        }
        else if (strcmp((sint8*) substring, "long") == 0) // Check if one token is "long"
        {
            gsm_data.flags.longitude = TRUE;        // Rise a specific flag
            ret_value |= LONGITUDE;
        }
        else if (strcmp((sint8*) substring, "time") == 0) // Check if one token is "time"
        {
            gsm_data.flags.time = TRUE;             // Rise a specific flag
            ret_value |= TIME;
        }
        else
        {
            //ret_value = ONLY_PHONE_NUMBER;
        }
        break;
    }
    return ret_value;
}
