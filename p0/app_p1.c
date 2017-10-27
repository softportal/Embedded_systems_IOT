/**
 * This example shows how to use the Motion Click wrapper of the LetMeCreate
 * library.
 *
 * Whenever the motion click detects an event, it flashes all LED's ten times.
 * The user must press Ctrl+C to terminate the program.
 *
 * The Motion Click must be inserted in Mikrobus 1 before running this program.
 */

#include <stdio.h>
#include <letmecreate/letmecreate.h>
#include <unistd.h>
//#include "examples/common.h"
int s = 2;
void leave(void)
{
	exit(0);
}

void long_t(void)
{
   s = 3;
}

void short_t(void)
{
   s = 1;
}

void flash_leds(uint8_t arg)
{
    /* Only flash LED's when motion starts getting detected. */
    if (arg != 1)
        return;

    int x = 0;
    for(; x < 10; ++x) {
        led_switch_on(ALL_LEDS);
        sleep(s);
        led_switch_off(ALL_LEDS);
        sleep(s);
    }
}

int main(void)
{
    //initialize hardware
    switch_init();
    led_init();

    motion_click_enable(MIKROBUS_1);
    motion_click_attach_callback(MIKROBUS_1, flash_leds);

    switch_add_callback(SWITCH_2_PRESSED, leave);
    switch_add_callback(SWITCH_1_PRESSED, short_t);
    switch_add_callback(SWITCH_1_RELEASED, long_t);

    printf("LED's will flash when Motion Click detects a movement.\n");
    printf("Press Ctrl+C to quit.\n");

    while (1)
        ;

    return 0;
}
