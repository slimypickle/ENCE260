#include "system.h"
#include "pio.h"

/* Include the pacer module from the previous lab.
   You must have completed this before starting this lab2-exercise.  */
#include "pacer.h"

int main (void)
{
    system_init ();
    
    /* TODO: Initialise the pins of the LED matrix.  */
    

    /* Set up pacer with a frequency of 100 Hz.  
        This gives 50Hz alternating between the 2 states.*/
    pacer_init (100);

    while (1)
    {
        /* Pace the loop.  */
        pacer_wait ();
        
        /* TODO: Drive the LED matrix using the pio functions,
           displaying only three corner LEDs.  */
    }
}
