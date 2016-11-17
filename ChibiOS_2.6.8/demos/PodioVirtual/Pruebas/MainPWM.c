#include "ch.h"
#include "hal.h"
#include "test.h"
#include "chprintf.h"

/* PWM pinout:
PWM1 = E9;
PWM2 = E11;
PWM3 = E13;
PWM4 = E14;
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


  halInit();
  chSysInit();

 // sdStart(&SD6, NULL);                             // UART6, 9600, 8N1
//  palSetPadMode(GPIOC, 6, PAL_MODE_ALTERNATE(8));  // PC6 ----> TX
//  palSetPadMode(GPIOC, 7, PAL_MODE_ALTERNATE(8));  // PC7 <---- RX

  pwmStart(&PWMD1, &pwmcfg);
  palSetPadMode(GPIOE, 9, PAL_MODE_ALTERNATE(1));  // PWM1 (TIM1_CH1)
  palSetPadMode(GPIOE, 11, PAL_MODE_ALTERNATE(1)); // PWM2 (TIM1_CH2)
  palSetPadMode(GPIOE, 13, PAL_MODE_ALTERNATE(1)); // PWM3 (TIM1_CH3)
  palSetPadMode(GPIOE, 14, PAL_MODE_ALTERNATE(1)); // PWM4 (TIM1_CH4)
  chThdSleepMilliseconds(2000);
    
  pwmEnableChannel(&PWMD1, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 1000));
  pwmEnableChannel(&PWMD1, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 1000));
  pwmEnableChannel(&PWMD1, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 1000));
  pwmEnableChannel(&PWMD1, 3, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 1000));




  while (TRUE) {
   
 // chprintf((BaseSequentialStream*)&SD6, "hola mundo \n\r");
 // chThdSleep(100);  
  }
}