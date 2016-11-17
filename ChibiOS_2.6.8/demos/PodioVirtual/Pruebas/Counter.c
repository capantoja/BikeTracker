/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"
#include <stdlib.h>
#include "chprintf.h"

/*
 * Application entry point.
 */
int main(void) {

  halInit();
  chSysInit();


  sdStart(&SD6, NULL);
  palSetPadMode(GPIOC, 7, PAL_MODE_ALTERNATE(8)); 
  palSetPadMode(GPIOC, 6, PAL_MODE_ALTERNATE(8)); 

  palSetPadMode(GPIOE, 6, PAL_MODE_INPUT);
  palSetPadMode(GPIOE, 5, PAL_MODE_INPUT);

   int x = 0;
   
  while (TRUE) {
    if (palReadPad(GPIOE, 6)){
        x = x + 1;   
    }
   chprintf((BaseSequentialStream*)&SD6, "hola mundo \n\r");
   chThdSleepMilliseconds(100);
    
  }

  
}