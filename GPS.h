#ifndef GPS_H_
#define GPS_H_

// includes
#include "uart.h"
#include "config.h"

typedef enum
{
    GPS_INIT = 0,
    SET_AQ_TIME,
    SET_ONLY_GPGLL,
    FINISHED_GPS_INIT
} GPS_initType;

typedef enum
{
    GPGLL = 0,
    LAT_VAL,
    LAT_DIR,
    LONG_VAL,
    LONG_DIR,
    TIME_VAL,
    FINISHED_EXTRACT
} gps_extract_data;

typedef struct
{
    uint8 latitude_value[12];
    uint8 longitude_direction;
    uint8 longitude_value[12];
    uint8 latitude_direction;
    uint8 curr_time[8];
} GPS_DATA;

//functions

/*GPS_init();
* Machine state for initializing the GPS
* Sets acquisition rate at 2 seconds
* Sets NMEA sentence output only to GPGLL
* INPUT: None
* RETURN: boolean isGPSinitialized
*/
boolean GPS_init(void);

/*GPS_extract_data();
* Machine state for extracting data from GPS
* INPUT: uint8 *substring
* RETURN: boolean retVal
*/
boolean GPS_extract_data(uint8 *substring);

/*gps_cpyrmz_mp(uint8 *message, uint8 *substring);
* Copy data to variables, remove excess zeros and move point in sentence
* INPUT: uint8 *message, uint8 *substring
* RETURN: uint8 *message
*/
uint8 *gps_cpyrmz_mp(uint8 *message, uint8 *substring);

#endif /* GPS_H_ */
