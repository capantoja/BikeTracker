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

/*
* PWM driver configuration structure
*/
static PWMConfig pwmcfg = {
  10000,                                    /* 10kHz PWM clock frequency.   */
  10,                                       /* Initial PWM period           */
  NULL,
  {
   {PWM_COMPLEMENTARY_OUTPUT_ACTIVE_HIGH | PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_COMPLEMENTARY_OUTPUT_ACTIVE_HIGH | PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_COMPLEMENTARY_OUTPUT_ACTIVE_HIGH | PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_COMPLEMENTARY_OUTPUT_ACTIVE_HIGH | PWM_OUTPUT_ACTIVE_HIGH, NULL}
  },
  0,
  0
};

int main(void) {

int Vel = 1;
int aux = 5000;
int t = 0;
// coeficientes del generador de trayectoria
int Tf = 100;
int Velocidad1 = 0;
int PosFinal = 1;
int PosInicial = 0; 
int a1 = 0;
int a2 = (3 / (Tf * Tf)) * (PosFinal - PosInicial);
int a3 = - (2 / (Tf * Tf * Tf)) * (PosFinal - PosInicial);  
halInit();
chSysInit();

chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
// Inicializa el driver PWM
  pwmStart(&PWMD1, &pwmcfg);
  palSetPadMode(GPIOE, 9, PAL_MODE_ALTERNATE(1));  // PWM1 (TIM1_CH1)


  
  while (TRUE) {

if (Vel == 1){

    for(t = 0; t <= Tf; t++){ 
      
      if(t <= Tf/2){Velocidad1 =  (a1 + (2 * a2 * t) + (3 * a3 * t * t)); 
                    pwmEnableChannel(&PWMD1, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, Velocidad1));
                    chThdSleepMilliseconds(5000);
                     }

      if(t > Tf/2) {/*pwmEnableChannel(&PWMD1, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 1000));*/}

      if(t == Tf)  {chThdSleepMilliseconds(5000);
                    Vel = 0;}
                             }
           }
       
if (Vel == 0){
    for(t = 0; t <= Tf/2; t++){Velocidad1 =  a1 + (2 * a2 * aux) + (3 * a3 * aux * aux); 
                    pwmEnableChannel(&PWMD1, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, Velocidad1));
                    aux++;
                    Vel = 1;
                    chThdSleepMilliseconds(1000);
                  }
    
     
}       

 }
}
