#ifndef GSM_H_
#define GSM_H_
#include <string.h>
#include "config.h"
#include "uart.h"
#include "GPS.h"

#define PHONE_NUMBER_DIMENSION 15u

typedef struct
{
    uint8 gsm_number[PHONE_NUMBER_DIMENSION];
    union
    {
        struct
        {
            uint8 latitude:1;
            uint8 longitude:1;
            uint8 time:1;
            uint8 not_used:5;
        };
    } flags;
} GSM_DATA;

typedef enum
{
    IDLE = 0,
    SEND_AT,
    SEND_AT_CMGF,
    SEND_AT_CNMI,
    FINISHED
} GSM_initType;

typedef enum
{
    IDLE_TXT,
    SEND_AT_CMGS_1,
    SEND_AT_CMGS_NR,
    SEND_MESSAGE,
    SEND_END_MESSAGE_26,
    FINISHED_TXT
} sendMessageState;

typedef enum
{
    CMT = 0,
    NUMBER_PH,
    CMD
} extractMessage;

typedef enum{
    NO_DATA           = 0 ,
    PHONE_NUMBER      = 1 << 0,
    LATITUDE          = 1 << 1,
    LONGITUDE         = 1 << 2,
    TIME              = 1 << 3,
    ONLY_PHONE_NUMBER = 1 << 4
}dataExtractedType;

//functions
boolean GSM_init(void);
boolean GSM_sendMessage(uint8 * message, GPS_DATA * gps_message,
                        boolean GPS_FLAG);
uint8 gsm_getMessageDetails(uint8* substring);

#endif /* GSM_H_ */
