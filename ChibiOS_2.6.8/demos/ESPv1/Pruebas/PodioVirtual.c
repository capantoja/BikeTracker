/**************************************************************************************************
*                            TEATRO DE LA MEMORIA EN MUNDOS VIRTUALES
*                               UNIVERSIDAD NACIONAL DE COLOMBIA
*                                             2015
*     
*    Programa para el control de movimiento del podio virtual, implementado en el sistema  
*    STM32F4DISCOVERY.
*
*    Sistema Operativo RTOS: ChibiOs ver 2.6.8  Mayo 2015
*    Compilador gcc (Ubuntu/Linaro 4.8.1-10ubuntu9) 4.8.1
*
*    Encoder pinout:
*    CW1  = B13
*    CCW1 = B14
*    CW2  = E6
*    CCW2 = E5
*    CW3  = D11
*    CCW3 = B1
*    CW4  = E12
*    CCW4 = E7
*
*    PWM pinout:
*    PWM1 = E9
*    PWM2 = E11
*    PWM3 = E13
*    PWM4 = E14
*
*    Direccion de giro pinout:
*    dir1 = B7
*    dir2 = B8 
*    dir3 = B4
*    dir4 = B9
*
*    Uart pinout:
*    TX = C6
*    Rx = C7
*
*    Trama de transmision del puerto serial ["S", "s", angulo1, angulo2, angulo3, angulo4, "E", "s"]
*    Trama de recepcion del puerto serial ["S", eje, velocidad, direccion, "E"]
*
*
*
**************************************************************************************************/

#include "ch.h"
#include "hal.h"
#include <stdlib.h>
 
static WORKING_AREA(waThread1, 128);          // blink thread
static WORKING_AREA(waThread2, 1048);         // Declaracion del Thread de lectura del encoder 1
static WORKING_AREA(waThread3, 1048);         // Declaracion del Thread de lectura del encoder 2
static WORKING_AREA(waThread4, 1048);         // Declaracion del Thread de lectura del encoder 3
static WORKING_AREA(waThread5, 1048);         // Declaracion del Thread de lectura del encoder 4

static uint8_t message[8];                    // Buffer de transmision del puerto serial "Ss0000Es"
static uint8_t buffer[20];                    // Buffer de recepcion del puerto serial

/*
 * Mailboxes and buffers.
 */
static Mailbox mb1;
static msg_t b1[20];

static Mailbox mb2;
static msg_t b2[20];

static Mailbox mb3;
static msg_t b3[20];

static Mailbox mb4;
static msg_t b4[20];


/*
*  Thread para el led blink
*/ 

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
*  Thread de lectura del encoder 1
*/

static msg_t Thread2(void *arg) 
{
  (void)arg;
  msg_t Mensaje1;
  int up1 = 0;
  int dw1 = 0;
  int last_estate_up1 = 0;
  int last_estate_dw1 = 0; 
  long int contador1 = 0;
  int angulo1 = 0;
 

while(TRUE){

   up1 = palReadPad(GPIOB, 13);
   dw1 = palReadPad(GPIOB, 14);

if ((up1 == 1) & (last_estate_up1 == 0)) { contador1++; }
if ((dw1 == 1) & (last_estate_dw1 == 0)) { contador1--; }

 angulo1 = contador1 * 0.004;   

 last_estate_up1 = up1;
 last_estate_dw1 = dw1;

 /* Envia los angulos al Thread main a traves de MailBox */
 Mensaje1 = angulo1;
 chMBPostI(&mb1, Mensaje1);     
  
}
return 0;
}

/* 
*  Thread de lectura del encoder 2
*/

static msg_t Thread3(void *arg) 
{
  (void)arg;
  msg_t Mensaje2;  
  int up2 = 0;
  int dw2 = 0;
  int last_estate_up2 = 0;
  int last_estate_dw2 = 0;
  long int contador2 = 0;  
  int angulo2 = 0;

while(TRUE){

   up2 = palReadPad(GPIOE, 6);
   dw2 = palReadPad(GPIOE, 5);

if ((up2 == 1) & (last_estate_up2 == 0)) { contador2++; }
if ((dw2 == 1) & (last_estate_dw2 == 0)) { contador2--; }
    
 angulo2 = contador2 * 0.004;   

 last_estate_up2 = up2;
 last_estate_dw2 = dw2;
 

 /* Envia los angulos al Thread main a traves de MailBox */
 Mensaje2 = angulo2;
 chMBPostI(&mb2, Mensaje2);     
  
}
return 0;
}

/* 
*  Thread de lectura del encoder 3
*/

static msg_t Thread4(void *arg) 
{
  (void)arg;
  msg_t Mensaje3;
  int up3 = 0;
  int dw3 = 0;
  int last_estate_up3 = 0;
  int last_estate_dw3 = 0;
  long int contador3 = 0;
  int angulo3 = 0;
 

while(TRUE){

   up3 = palReadPad(GPIOD, 11);
   dw3 = palReadPad(GPIOB, 1);

if ((up3 == 1) & (last_estate_up3 == 0)) { contador3--; }
if ((dw3 == 1) & (last_estate_dw3 == 0)) { contador3++; }
   
 angulo3 = contador3 * 0.0128;   

 last_estate_up3 = up3;
 last_estate_dw3 = dw3;

 /* Envia los angulos al Thread main a traves de MailBox */
 Mensaje3 = angulo3;
 chMBPostI(&mb3, Mensaje3); 

   
  
}
return 0;
}

/* 
*  Thread de lectura del encoder 4
*/

static msg_t Thread5(void *arg) 
{
  (void)arg;
  msg_t Mensaje4;

  int up4 = 0;
  int dw4 = 0;
  int last_estate_up4 = 0;
  int last_estate_dw4 = 0;
  long int contador4 = 0;
  int angulo4 = 0;

while(TRUE){

   up4 = palReadPad(GPIOE, 12);
   dw4 = palReadPad(GPIOE, 7);

if ((up4 == 1) & (last_estate_up4 == 0)) { contador4++; }
if ((dw4 == 1) & (last_estate_dw4 == 0)) { contador4--; }

    
 angulo4 = contador4 * 0.114;   

 last_estate_up4 = up4;
 last_estate_dw4 = dw4;

 /* Envia los angulos al Thread main a traves de MailBox */
 Mensaje4 = angulo4;
 chMBPostI(&mb4, Mensaje4);     
  
}
return 0;
}

/*
*  Funcion de control de movimiento de los motores DC por PWM
*/ 
void ControlPWM(int Articulacion, int Velocidad, int Direccion) {
  

// Control Articulacion 1
  if((Articulacion == 49) & (Velocidad != 48)){  
      palSetPad(GPIOD, GPIOD_LED3);
      palClearPad(GPIOD, GPIOD_LED4);
      palClearPad(GPIOD, GPIOD_LED6);
      pwmEnableChannel(&PWMD1, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 4000));
      pwmEnableChannel(&PWMD1, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 0));     // Detiene el motor2
      pwmEnableChannel(&PWMD1, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 0));     // Detiene el motor3
      pwmEnableChannel(&PWMD1, 3, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 0));     // Detiene el motor4

      // Controla direccion de giro del motor1
      if (Direccion == 48){palSetPad(GPIOB, 7);}
      else {palClearPad(GPIOB, 7);}

       }

       
    
// Control Articulacion 2
  if((Articulacion == 50) & (Velocidad != 48)){ 
      
      palSetPad(GPIOD, GPIOD_LED4);
      palClearPad(GPIOD, GPIOD_LED3);
      palClearPad(GPIOD, GPIOD_LED6);
      pwmEnableChannel(&PWMD1, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 0));     // Detiene el motor1
      pwmEnableChannel(&PWMD1, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 2000));  // Detiene el motor2
      pwmEnableChannel(&PWMD1, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 0));     // Detiene el motor3
      pwmEnableChannel(&PWMD1, 3, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 0));     // Detiene el motor4
      
       // Controla direccion de giro del motor 2
      if (Direccion == 48){palSetPad(GPIOB, 8);}
      else {palClearPad(GPIOB, 8);}               
  }
          
// Control Articulacion 3 
  if((Articulacion == 51) & (Velocidad != 48)){ 
      
      palSetPad(GPIOD, GPIOD_LED6);
      palClearPad(GPIOD, GPIOD_LED3);
      palClearPad(GPIOD, GPIOD_LED4);
      pwmEnableChannel(&PWMD1, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 0));     // Detiene el motor1
      pwmEnableChannel(&PWMD1, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 0));     // Detiene el motor2
      pwmEnableChannel(&PWMD1, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 1000));      
      pwmEnableChannel(&PWMD1, 3, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 0));     // Detiene el motor4  

         // Controla direccion de giro del motor3 
      if (Direccion == 48){palSetPad(GPIOB, 4);}
      else {palClearPad(GPIOB, 4);} 
  }

// Control Articulacion 4
  if((Articulacion == 52) & (Velocidad != 48)){
      
      pwmEnableChannel(&PWMD1, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 0));     // Detiene el motor1
      pwmEnableChannel(&PWMD1, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 0));     // Detiene el motor2
      pwmEnableChannel(&PWMD1, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 0));     // Detiene el motor3
      pwmEnableChannel(&PWMD1, 3, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 1000));   

       // Controla direccion de giro del motor4 
      if (Direccion == 48){palSetPad(GPIOB, 9);}
      else {palClearPad(GPIOB, 9);} 

    
  }

  
if(Velocidad == 48){
   pwmEnableChannel(&PWMD1, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 0));     // Detiene el motor1
   pwmEnableChannel(&PWMD1, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 0));     // Detiene el motor2
   pwmEnableChannel(&PWMD1, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 0));     // Detiene el motor3
   pwmEnableChannel(&PWMD1, 3, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 0));     // Detiene el motor4
   palClearPad(GPIOD, GPIOD_LED4); 
   palClearPad(GPIOD, GPIOD_LED3);
   palClearPad(GPIOD, GPIOD_LED6);  
 }
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
  
}

/*
 * This callback is invoked when a receive buffer has been completely written.
 * Callback para decodificar el dato recibido por el puerto serial
 */
static void rxend(UARTDriver *uartp) {

  (void)uartp;
  int i;
  int Articulacion = 0;
  int Velocidad    = 0;
  int Direccion    = 0;
  int dato         = 0;
  int cont         = 0;
  bool flag        = FALSE;
  int aux[5];
  

  // Recorre el buffer de recepcion
  for (i = 0; i < 8; i++){  
      dato = (int)buffer[i];
      
     // Detecta el encabezado S
    if ((dato == 83) && (flag == FALSE)) {  
       cont = 0;
       aux[0] = 83;
       cont++;
       flag = TRUE;
       } 
     else{ 
     // Separa los datos en las variables Articulacion, Velocidad, Direccion                 
         if(flag == TRUE){
          aux[cont] = dato;
          cont++;
                if(cont == 4){
                    if((aux[0] == 83) && (aux[4] == 69)){
                            
                        Articulacion = aux[1];
                        Velocidad    = aux[2];
                        Direccion    = aux[3];
                        ControlPWM(Articulacion, Velocidad, Direccion);                         
                        cont = 0;
                        flag = FALSE; 
                        }
                  }
             }  
         }
    }     
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
  100000,                                    /* 100kHz PWM clock frequency.   */
  10,                                        /* Initial PWM period  0,0001s   */
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
  msg_t Angulos1, Angulos2, Angulos3, Angulos4;


  /* Inicializa el puerto serial UART6 C6(TX) y C7(RX) */
  uartStart(&UARTD6, &uart_cfg_1);
  palSetPadMode(GPIOC, 7, PAL_MODE_ALTERNATE(8));
  palSetPadMode(GPIOC, 6, PAL_MODE_ALTERNATE(8));

  /* Inicializa el driver PWM */
  pwmStart(&PWMD1, &pwmcfg);
  palSetPadMode(GPIOE, 9, PAL_MODE_ALTERNATE(1));  // PWM1 (TIM1_CH1)
  palSetPadMode(GPIOE, 11, PAL_MODE_ALTERNATE(1)); // PWM2 (TIM1_CH2)
  palSetPadMode(GPIOE, 13, PAL_MODE_ALTERNATE(1)); // PWM3 (TIM1_CH3)
  palSetPadMode(GPIOE, 14, PAL_MODE_ALTERNATE(1)); // PWM4 (TIM1_CH4)
   
  /* Inicializa pines de salida digitales */ 
  palSetPadMode(GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL); // direction motor 1
  palSetPadMode(GPIOB, 8, PAL_MODE_OUTPUT_PUSHPULL); // direction motor 2
  palSetPadMode(GPIOB, 4, PAL_MODE_OUTPUT_PUSHPULL); // direction motor 3
  palSetPadMode(GPIOB, 9, PAL_MODE_OUTPUT_PUSHPULL); // direction motor 4
 
  /* Detiene los cuatro motores */
  pwmEnableChannel(&PWMD1, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 0));  // Stop motor 1
  pwmEnableChannel(&PWMD1, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 0));  // Stop motor 2
  pwmEnableChannel(&PWMD1, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 0));  // Stop motor 3
  pwmEnableChannel(&PWMD1, 3, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 0));  // Stop motor 4
 

  /* Inicializa los pines de lectura de los encoder */
  palSetPadMode(GPIOB, 13, PAL_MODE_INPUT);        // CW1
  palSetPadMode(GPIOB, 14, PAL_MODE_INPUT);        // CCW1
  palSetPadMode(GPIOE, 6,  PAL_MODE_INPUT);        // CW2
  palSetPadMode(GPIOE, 5,  PAL_MODE_INPUT);        // CCw2
  palSetPadMode(GPIOD, 11, PAL_MODE_INPUT);        // CW3
  palSetPadMode(GPIOB, 1,  PAL_MODE_INPUT);        // CCW3
  palSetPadMode(GPIOE, 12, PAL_MODE_INPUT);        // CW4
  palSetPadMode(GPIOE, 7,  PAL_MODE_INPUT);        // CCW4

  /* Inicializa MailBox */
  chMBInit(&mb1, b1, 8);
  chMBInit(&mb2, b2, 8);
  chMBInit(&mb3, b3, 8);
  chMBInit(&mb4, b4, 8);
 

  /* Inicializa Thread para el blink led rojo */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);  // Blinker Thread

  /* Inicializa Threads de lectura de la posicion de los ejes podio */
  chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO, Thread2, NULL);  // Encoder 1 thread 
  chThdCreateStatic(waThread3, sizeof(waThread3), NORMALPRIO, Thread3, NULL);  // Encoder 2 thread 
  chThdCreateStatic(waThread4, sizeof(waThread4), NORMALPRIO, Thread4, NULL);  // Encoder 3 thread 
  chThdCreateStatic(waThread5, sizeof(waThread5), NORMALPRIO, Thread5, NULL);  // Encoder 4 thread 

  while (TRUE) {

  /* Detiene toda operacion del puerto serial */   
  uartStopReceive(&UARTD6); 
  uartStopSend(&UARTD6);

  /* Lee el puerto serial para recibir comandos desde el servidor */ 
  uartStartReceive(&UARTD6, 5, buffer);

  /* Recibe los angulos via MailBox */
  chMBFetch(&mb1, &Angulos1, TIME_INFINITE);
  chMBFetch(&mb2, &Angulos2, TIME_INFINITE);
  chMBFetch(&mb3, &Angulos3, TIME_INFINITE);
  chMBFetch(&mb4, &Angulos4, TIME_INFINITE);

  /* Crea el vector de datos para ser enviado al servidor */
 message[0] = 83;                 // Encabezado "S"
 message[1] = 115;                // Segundo encabezado "s"
 message[2] = Angulos1;           // Angulo 1
 message[3] = Angulos2;           // Angulo 2
 message[4] = Angulos3;           // Angulo 3
 message[5] = Angulos4;           // Angulo 4
 message[6] = 69;                 // Finalizador "E"
 message[7] = 115;                // Segundo finalizador "s"

  /* Envia los angulos al servidor */
  uartStartSend(&UARTD6, 8, message);

  /* 100 Milisegundos de retardo */
  chThdSleepMilliseconds(100); 
  
  }
}
