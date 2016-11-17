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

int contador1 = 0;
int contador2 = 0;
int contador3 = 0;
int contador4 = 0;

int angulo1 = 0;
int angulo2 = 0;
int angulo3 = 0;
int angulo4 = 0;    

static WORKING_AREA(waThread1, 128);
static WORKING_AREA(waThread2, 1024);

// Led blink
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
/* Encoder pinout:
CW1  = B13;
CCW1 = B14;
CW2  = E6;
CCW2 = E5;
CW3  = D11;
CCW3 = B1;
CW4  = E12;
CCW4 = E7;
*/
/* Triggered when any encoder */

static msg_t Thread2(void *arg) 
{
  (void)arg;
  int up1 = 0;
  int dw1 = 0;
  int up2 = 0;
  int dw2 = 0;
  int up3 = 0;
  int dw3 = 0;
  int up4 = 0;
  int dw4 = 0;

  int last_estate_up1 = 0;
  int last_estate_dw1 = 0; 
  int last_estate_up2 = 0;
  int last_estate_dw2 = 0;
  int last_estate_up3 = 0;
  int last_estate_dw3 = 0;
  int last_estate_up4 = 0;
  int last_estate_dw4 = 0;

while(TRUE){

   up1 = palReadPad(GPIOB, 13);
   dw1 = palReadPad(GPIOB, 14);
   up2 = palReadPad(GPIOE, 6);
   dw2 = palReadPad(GPIOE, 5);
   up3 = palReadPad(GPIOD, 11);
   dw3 = palReadPad(GPIOB, 1);
   up4 = palReadPad(GPIOE, 12);
   dw4 = palReadPad(GPIOE, 7);

if ((up1 == 1) & (last_estate_up1 == 0)) { contador1++; }
if ((dw1 == 1) & (last_estate_dw1 == 0)) { contador1--; }

if ((up2 == 1) & (last_estate_up2 == 0)) { contador2++; }
if ((dw2 == 1) & (last_estate_dw2 == 0)) { contador2--; }

if ((up3 == 1) & (last_estate_up3 == 0)) { contador3++; }
if ((dw3 == 1) & (last_estate_dw3 == 0)) { contador3--; }

if ((up4 == 1) & (last_estate_up4 == 0)) { contador4++; }
if ((dw4 == 1) & (last_estate_dw4 == 0)) { contador4--; }

 angulo1 = contador1 * 3; 
 angulo2 = contador2 * 3; // 12000 pulsos equivalen a 360 grados de la articulacion
 angulo3 = contador3 * 3;
 angulo4 = contador4 * 4;

 last_estate_up1 = up1;
 last_estate_dw1 = dw1;

 last_estate_up2 = up2;
 last_estate_dw2 = dw2;

 last_estate_up3 = up3;
 last_estate_dw3 = dw3;

 last_estate_up4 = up4;
 last_estate_dw4 = dw4;

 chprintf((BaseSequentialStream*)&SD6,"%c %d %d %d %d %c \n\r", "S", angulo1, angulo2, angulo3, angulo4, "E");
 
}
return 0;
}
  


int main(void) {

  
  halInit();
  chSysInit();

sdStart(&SD6, NULL);                             // UART6, 9600, 8N1
palSetPadMode(GPIOC, 6, PAL_MODE_ALTERNATE(8));  // PC6 ----> TX
palSetPadMode(GPIOC, 7, PAL_MODE_ALTERNATE(8));  // PC7 <---- RX

palSetPadMode(GPIOB, 13, PAL_MODE_INPUT);        // CW1
palSetPadMode(GPIOB, 14, PAL_MODE_INPUT);        // CCW1
palSetPadMode(GPIOE, 6, PAL_MODE_INPUT);         // CW2
palSetPadMode(GPIOE, 5, PAL_MODE_INPUT);         // CCw2
palSetPadMode(GPIOD, 11, PAL_MODE_INPUT);        // CW3
palSetPadMode(GPIOB, 1, PAL_MODE_INPUT);         // CCW3
palSetPadMode(GPIOE, 12, PAL_MODE_INPUT);        // CW4
palSetPadMode(GPIOE, 7, PAL_MODE_INPUT);         // CCW4 

chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);  // Blink threat
chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO, Thread2, NULL);  // Encoder threat

chprintf((BaseSequentialStream*)&SD6, "Programa nicializado \n\r");

  while (TRUE) {
 }
}
