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
#include "chprintf.h"


icucnt_t last_width, last_period;

static void icuwidthcb(ICUDriver *icup) {

  palSetPad(GPIOD, GPIOD_LED4);
  last_width = icuGetWidth(icup);
  chprintf((BaseSequentialStream*)&SD6, " last_width: %d \n\r", last_width);
}

static void icuperiodcb(ICUDriver *icup) {

  palClearPad(GPIOD, GPIOD_LED4);
  last_period = icuGetPeriod(icup);
  chprintf((BaseSequentialStream*)&SD6, "last_period: %d \n\r", last_period);
}

static ICUConfig icucfg = {
  ICU_INPUT_ACTIVE_HIGH,
  10000,                                    /* 10kHz ICU clock frequency.   */
  icuwidthcb,
  icuperiodcb,
  NULL,
  ICU_CHANNEL_2,
  0
};

/*
 * Application entry point.
 */
int main(void) {

 
  halInit();
  chSysInit();

  sdStart(&SD6, NULL);
  palSetPadMode(GPIOC, 6, PAL_MODE_ALTERNATE(8));
  palSetPadMode(GPIOC, 7, PAL_MODE_ALTERNATE(8));

  icuStart(&ICUD9, &icucfg);
  palSetPadMode(GPIOE, 6, PAL_MODE_ALTERNATE(3));
  icuEnable(&ICUD9);
  chThdSleepMilliseconds(2000);


  
  //icuDisable(&ICUD3);
  //icuStop(&ICUD3);
  palClearPad(GPIOD, GPIOD_LED4);
  palClearPad(GPIOD, GPIOD_LED5);

  /*
   * Normal main() thread activity, in this demo it does nothing.
   */
  while (TRUE) {
    chThdSleepMilliseconds(500);
  }
  return 0;
}