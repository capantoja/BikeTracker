#include "ch.h"
#include "hal.h"

static VirtualTimer vt3, vt4, vt5;
static const uint8_t message[] = "Ss1234Es";
static uint8_t buffer[20];

void ControlPWM(int Articulacion, int Velocidad, int Direccion) {

  if((Articulacion == 49) & (Velocidad != 48)){
      // Control Articulacion 1
      pwmEnableChannel(&PWMD1, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 1000));  

      // Configura direccion de giro 
            if (Direccion == 48){                                         
               palSetPad(GPIOB, 7);
               }
            else {
               palClearPad(GPIOB, 7);
                 } 
  }
  else {
      pwmEnableChannel(&PWMD1, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 0));     // Detiene el motor1
  }

  if((Articulacion == 50) & (Velocidad != 48)){
    // Control Articulacion 2
      pwmEnableChannel(&PWMD1, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 1000));  

       // Configura direccion de giro 
            if (Direccion == 48){
               palSetPad(GPIOB, 8);
               }
            else {
               palClearPad(GPIOB, 8);
               }  
              
  }
   else {
      pwmEnableChannel(&PWMD1, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 0));     // Detiene el motor2
  }

  if((Articulacion == 51) & (Velocidad != 48)){
    // Control Articulacion 3
      pwmEnableChannel(&PWMD1, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 1000));  

         // Configura direccion de giro 
            if (Direccion == 48){
               palSetPad(GPIOB, 4);
               }
            else {
               palClearPad(GPIOB, 4);
               } 
  }
   else {
      pwmEnableChannel(&PWMD1, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 0));     // Detiene el motor3
  }

  if((Articulacion == 52) & (Velocidad != 48)){
    // Control Articulacion 4
      pwmEnableChannel(&PWMD1, 3, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 1000));  

       // Configura direccion de giro 
            if (Direccion == 48){
               palSetPad(GPIOB, 9);
               }
            else {
               palClearPad(GPIOB, 9);
              } 
  }
   else {
      pwmEnableChannel(&PWMD1, 3, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 0));     // Detiene el motor4
  }
}

static void led3off(void *p) {

  (void)p;
  palClearPad(GPIOD, GPIOD_LED3);
  palClearPad(GPIOD, GPIOD_LED5);
}

static void led4off(void *p) {

  (void)p;
  palClearPad(GPIOD, GPIOD_LED4);
}

static void led5off(void *p) {

  (void)p;
  palClearPad(GPIOD, GPIOD_LED5);
}

/*
 * This callback is invoked when a transmission buffer has been completely
 * read by the driver.
 */
static void txend1(UARTDriver *uartp) {

  (void)uartp;
}

/*
 * This callback is invoked when a transmission has physically completed.
 */
static void txend2(UARTDriver *uartp) {

  (void)uartp;
  palSetPad(GPIOD, GPIOD_LED5);
  chSysLockFromIsr();
  if (chVTIsArmedI(&vt5))
    chVTResetI(&vt5);
  chVTSetI(&vt5, MS2ST(200), led5off, NULL);
  chSysUnlockFromIsr();
}

/*
 * This callback is invoked on a receive error, the errors mask is passed
 * as parameter.
 */
static void rxerr(UARTDriver *uartp, uartflags_t e) {

  (void)uartp;
  (void)e;
}

/*
 * This callback is invoked when a character is received but the application
 * was not ready to receive it, the character is passed as parameter.
 */
static void rxchar(UARTDriver *uartp, uint16_t c) {

  (void)uartp;
  (void)c;
  /* Flashing the LED each time a character is received.*/
  palSetPad(GPIOD, GPIOD_LED4);
  chSysLockFromIsr();
  if (chVTIsArmedI(&vt4))
    chVTResetI(&vt4);
  chVTSetI(&vt4, MS2ST(200), led4off, NULL);
  chSysUnlockFromIsr();
}

/*
 * This callback is invoked when a receive buffer has been completely written.
 */
static void rxend(UARTDriver *uartp) {

  (void)uartp;
  int i;
  int Articulacion = 0;
  int Velocidad    = 0;
  int Direccion    = 0;
  
  
  for (i = 0; i < 20; i++) {
    
     // Detecta el encabezado S y el finalizador E
    if ((buffer[i] == 83) & (buffer[i + 7] == 69)) {      
      Articulacion = (int)buffer[i + 1];
      Velocidad    = (int)buffer[i + 2];
      Direccion    = (int)buffer[i + 3];
    }      
  }

  ControlPWM(Articulacion, Velocidad, Direccion);

   /* Flashing the LED each time a character is received.*/  
  palSetPad(GPIOD, GPIOD_LED3);
  chSysLockFromIsr();
  if (chVTIsArmedI(&vt3))
    chVTResetI(&vt3);
  chVTSetI(&vt3, MS2ST(200), led3off, NULL);
  chSysUnlockFromIsr();
}

/*
 * UART driver configuration structure.
 */
static UARTConfig uart_cfg_1 = {
  txend1,
  txend2,
  rxend,
  rxchar,
  rxerr,
  9600,
  0,
  USART_CR2_LINEN,
  0
};

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

  
  halInit();
  chSysInit();

  // Inicializa el puerto serial UART6 PC6(TX) y PC7(RX)
  uartStart(&UARTD6, &uart_cfg_1);
  palSetPadMode(GPIOC, 7, PAL_MODE_ALTERNATE(8));
  palSetPadMode(GPIOC, 6, PAL_MODE_ALTERNATE(8));

  // Inicializa el driver PWM
  pwmStart(&PWMD1, &pwmcfg);
  palSetPadMode(GPIOE, 9, PAL_MODE_ALTERNATE(1));  // PWM1 (TIM1_CH1)
  palSetPadMode(GPIOE, 11, PAL_MODE_ALTERNATE(1)); // PWM2 (TIM1_CH2)
  palSetPadMode(GPIOE, 13, PAL_MODE_ALTERNATE(1)); // PWM3 (TIM1_CH3)
  palSetPadMode(GPIOE, 14, PAL_MODE_ALTERNATE(1)); // PWM4 (TIM1_CH4)
  chThdSleepMilliseconds(1000);
   
  // Inicializa pines de salida digitales 
  palSetPadMode(GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL); // direction motor 1
  palSetPadMode(GPIOB, 8, PAL_MODE_OUTPUT_PUSHPULL); // direction motor 2
  palSetPadMode(GPIOB, 4, PAL_MODE_OUTPUT_PUSHPULL); // direction motor 3
  palSetPadMode(GPIOB, 9, PAL_MODE_OUTPUT_PUSHPULL); // direction motor 4
 
  // Detiene los cuatro motores 
  pwmEnableChannel(&PWMD1, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 0));  // Stop motor 1
  pwmEnableChannel(&PWMD1, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 0));  // Stop motor 2
  pwmEnableChannel(&PWMD1, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 0));  // Stop motor 3
  pwmEnableChannel(&PWMD1, 3, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 0));  // Stop motor 4


  while (TRUE) {
      uartStopReceive(&UARTD6);
    //  uartStopSend(&UARTD6);
      uartStartReceive(&UARTD6, 20, buffer);
    
  }
}
