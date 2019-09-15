#include "GPS.h"
#include "GSM.h"

// variables
GPS_DATA gps_data;
extern GSM_DATA gsm_data;
GPS_initType gps_Init;

boolean GPS_init(void)
{
    boolean isGPSinitialized = FALSE;

    switch (gps_Init)
    {
    case GPS_INIT:
        isGPSinitialized = FALSE;                   // Not completed yet
        gps_Init = SET_AQ_TIME;                     // Next state: SET_AQ_TIME
        break;
    case SET_AQ_TIME:
        //Packet Type: 220 PMTK_SET_POS_FIX This message is used to set position fix interval.
        sendMessage("$PMTK220,2000*1C");
        isGPSinitialized = FALSE;                  // Not completed yet
        gps_Init = SET_ONLY_GPGLL;                 // Next state: SET_ONLY_GPGLL
        break;
    case SET_ONLY_GPGLL:
        // Packet Type: 314 PMTK_API_SET_NMEA_OUTPUT, sets only GPGLL
        sendMessage("$PMTK314,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29");
        isGPSinitialized = FALSE;                  // Not completed yet
        gps_Init = FINISHED_GPS_INIT;              // Next state: FINISHED_GPS_INIT
        break;
    case FINISHED_GPS_INIT:
        isGPSinitialized = TRUE;                   // Completed
        break;
    }
    return isGPSinitialized;
}

boolean GPS_extract_data(uint8 *substring)
{
    static gps_extract_data extractState = GPGLL;
    boolean retVal = FALSE;
    switch (extractState)
    {
    case GPGLL:           // Extracted $GLGLL
        retVal = FALSE;          // Not completed yet
        extractState = LAT_VAL;  // Next state: LAT_VAL
        break;
    case LAT_VAL:        // Extracted Latitude value
        if (substring)
        {
            gps_cpyrmz_mp(gps_data.latitude_value, substring); // Process data: Copy, remove excess zeros, move point
        }
        retVal = FALSE;          // Not completed yet
        extractState = LAT_DIR;  // Next state: LAT_DIR
        break;
    case LAT_DIR:                // Extracted Latitude Meridian
        if (substring)
        {
            gps_data.latitude_direction = substring[0];       // Copy data
        }
        retVal = FALSE;          // Not completed yet
        extractState = LONG_VAL; // Next state: LONG_VAL
        break;
    case LONG_VAL:               // Extracted Longitude Value
        if (substring)
        {
            gps_cpyrmz_mp(gps_data.longitude_value, substring);
        }
        retVal = FALSE;          // Not completed yet
        extractState = LONG_DIR; // Next state: LONG_DIR
        break;
    case LONG_DIR:               // Extracted Longitude Value
        if (substring)
        {
            gps_data.longitude_direction = substring[0];      // Copy data
        }
        retVal = FALSE;           // Not completed yet
        extractState = TIME_VAL;  // Next state: TIME_VAL
        break;
    case TIME_VAL:

        if (substring)
        {
            // Insert ':' between hours, minutes, seconds and copy them
            uint8 k = 0, i = 0;
            while ((i < strlen((sint8*) substring)) && (substring[i] != '.'))
            {
                if ((i % 2 == 0) && i != 0)             // insert after 2 consecutive characters ':' otherwise just continue copying
                    gps_data.curr_time[k++] = ':';
                gps_data.curr_time[k++] = substring[i]; // copy character by character.
                i++;
            }
            gps_data.curr_time[k] = '\0';               // close the string.
        }
        retVal = FALSE;                  // Not completed yet
        extractState = FINISHED_EXTRACT; // Next state: TIME_VAL
        break;
    case FINISHED_EXTRACT:
        extractState = GPGLL;           // Reinitialize the state machine
        retVal = TRUE;                  // Completed
        break;
    }
    return retVal;
}

// GPS data processing: copy and remove 0 from the beginning and move point found in string
uint8 *gps_cpyrmz_mp(uint8 *message, uint8 *substring)
{
    uint8 i = 0, k = 0;
    while (substring[i] == '0')                 // Skip 0 from the beginning
        i++;
    while ((message[k] = substring[i]) != 0)    // Copy string
    {
        if (message[k] == '.')                  // Move point 2 positions back
        {
            message[k] = message[k - 1];
            message[k - 1] = message[k - 2];
            message[k - 2] = '.';
        }
        i++;
        k++;
    }
    return message;
}
