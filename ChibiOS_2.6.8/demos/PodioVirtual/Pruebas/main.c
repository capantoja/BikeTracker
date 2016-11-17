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
#include "test.h"

static WORKING_AREA(waThread1, 128);
static WORKING_AREA(waThread2, 128);
static WORKING_AREA(waThread3, 128);
static WORKING_AREA(waThread4, 128);


static msg_t Thread1(void *arg) 
{
  (void)arg;
  palSetPadMode(GPIOD, GPIOD_LED5, PAL_MODE_OUTPUT_PUSHPULL);
  
  while (TRUE) {
    palSetPad(GPIOD, GPIOD_LED5);       /* Red */
    chThdSleepMilliseconds(500);
    palClearPad(GPIOD, GPIOD_LED5);     
    chThdSleepMilliseconds(500);
  }
  return 0;
}

static msg_t Thread2(void *arg) 
{
  (void)arg;
  palSetPadMode(GPIOD, GPIOD_LED4, PAL_MODE_OUTPUT_PUSHPULL);
  
  while (TRUE) {
    palSetPad(GPIOD, GPIOD_LED4);       /* Green */
    chThdSleepMilliseconds(500);
    palClearPad(GPIOD, GPIOD_LED4);     
    chThdSleepMilliseconds(500);
  }
  return 0;
}

static msg_t Thread3(void *arg) 
{
  (void)arg;
  palSetPadMode(GPIOD, GPIOD_LED6, PAL_MODE_OUTPUT_PUSHPULL);
  
  while (TRUE) {
    palSetPad(GPIOD, GPIOD_LED6);       /* Blue */
    chThdSleepMilliseconds(500);
    palClearPad(GPIOD, GPIOD_LED6);     
    chThdSleepMilliseconds(500);
  }
  return 0;
}

static msg_t Thread4(void *arg) 
{
  (void)arg;
  palSetPadMode(GPIOD, GPIOD_LED3, PAL_MODE_OUTPUT_PUSHPULL);
  
  while (TRUE) {
    palSetPad(GPIOD, GPIOD_LED3);       /* Red */
    chThdSleepMilliseconds(500);
    palClearPad(GPIOD, GPIOD_LED3);     
    chThdSleepMilliseconds(500);
  }
  return 0;
}


int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO, Thread2, NULL);
chThdCreateStatic(waThread3, sizeof(waThread3), NORMALPRIO, Thread3, NULL);
chThdCreateStatic(waThread4, sizeof(waThread4), NORMALPRIO, Thread4, NULL);

  
  while (TRUE) {


 }
}
