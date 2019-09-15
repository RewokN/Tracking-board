#ifndef PORT1_H_
#define PORT1_H_

// includes
#include <string.h>
#include "config.h"
#include "uart.h"

// defines
typedef enum
{
    TO_GPS,   //0
    TO_GSM    //1
} stare_mux;

// functions

/*port_config
* Sets up the configuration for port1
* INPUT: None
* RETURN: None
*/
void port_init(void);

/*mux_switch
* Switches the state
* INPUT: enum stare_mux mux
* RETURN: None
*/
extern void mux_switch(stare_mux mux);
extern void Port1_GSM_apel(void);

#endif /* PORT1_H_ */
