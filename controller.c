/*

   Programmer - David Whipple
   Platform        - Rasbperry PI/Plan 9 OS
   Date               - 10/4/2015

   Purpose         - This is a small bit of code to read a momentary switch connected via GPIO pin 27
                              and cycle through 1 of 3 states with an LED.  

                              State 1 - OFF
                              State 2 - ON
                              State 3 - Blinking

*/

#include <u.h>
#include <libc.h>
#include <stdio.h>

/*  Purpose - This function is a representation of a simple state machine that 
                         toggles between the 3 states.

      state 0 is an error mode
      state 1 is off - default on startup
      state 2 is on
      state 3 is blinking
*/

int change_state(int currentState, int file)
{
    switch (currentState) 
    {
          case 1: 
                 printf("Current mode is OFF, changing to ON.\n");
                 fprint(file, "set 22 1"); 
                 return 2;
          case 2: 
                 printf("Current mode is ON, changing to BLINK.\n");
                 fprint(file, "set 22 0"); 
                 return 3;
          case 3:
                 printf("Current mode is BLINK, changing to OFF.\n");
                 fprint(file, "set 22 0"); 
                 return 1;
    }
    return 0;   /* If we reached here, we are in invalid mode, return an error state */
}

void 
main() 
{
    int file;    /* File Descriptor */
    char buf[16];
    int numChars;
    int currentState=1;
    char newChar=0, lastChar=0;

    bind("#G", "/dev", MAFTER);

    file = open("/dev/gpio", ORDWR);

    fprint(file, "function 27 in");           /* Setup pin GPIO 27 to be an input, this is the switch */
    fprint(file, "function 22 out");        /* Setup pin GPIO 22 to be an output, this will be the LED */

    fprint(file, "set 22 0");                     /* Set the startup state to off */
    

    /*  This will be the primary loop for the entire system */
    printf("Entering Polling Loop to watch for switch, currentState is OFF.\n");

    while (1) {

        memset(buf, 0, 17);                       /* Empty my buffer */
        numChars=read(file, buf, 17);     /* Check the status of the port */
        newChar = buf[1];                         /* This grabs the character with GPIO 27 in it */

        if (currentState == 3)
        {
              fprint(file, "set 22 1");
              sleep(500);
              fprint(file, "set 22 0");
              sleep(500);
        }

        if ((newChar == '8') && (lastChar != '8'))  
        {
             currentState = change_state(currentState, file);
        }

        lastChar = newChar;                      /* This is to insure the switch is actually cycled */

}
