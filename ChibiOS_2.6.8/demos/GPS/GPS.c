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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


static WORKING_AREA(waThread1, 128);
static char GRMC[80]; 
static char buffer[200];  
static char *latitud;
static char *longitud;
static char *NSIndicador;
static char *WEIndicador;
static char *Validador;


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

static void rxend(UARTDriver *uartp) {

  (void)uartp;
  int i;
  int j = 0;
  char *token;
  const char s[2] = ",";

  // Guarda el mensaje RMC
  for (i = 0; i < 80; i++)
  {  
      if ((buffer[i] == '$') && (buffer[i+3] == 'R') && (i<30)) // $GRMC message
      {
        for(j=0; j<50; j++)
        {
          GRMC[j] = buffer[i+j]; 
        }
      }
  }

// Obtiene los datos de latitud y longitud
  j = 0;
  token = strtok(GRMC, s);
    
      while( token != NULL ) 
      {     
       switch(j)
       {
         case 2: Validador = token; break;
         case 3: latitud = token; break;
         case 4: NSIndicador = token; break;
         case 5: longitud = token; break;
         case 6: WEIndicador = token; break;
         default: break;
        }
       token = strtok(NULL, s);
       j = j+1;
      }
  j = 0;
}


static UARTConfig uart_cfg = {
  NULL,
  NULL,
  rxend,
  NULL,
  NULL,
  9600,
  0,
  USART_CR2_LINEN,
  0
};

static UARTConfig uart_cfg_1 = {
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  9600,
  0,
  USART_CR2_LINEN,
  0
};


int main(void) {

 
  halInit();
  chSysInit();

  
  uartStart(&UARTD2, &uart_cfg);
  palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7)); // PA2 (TX)
  palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7)); // PA3 (RX)


  uartStart(&UARTD6, &uart_cfg_1);
  palSetPadMode(GPIOC, 7, PAL_MODE_ALTERNATE(8)); // PC7 (RX)
  palSetPadMode(GPIOC, 6, PAL_MODE_ALTERNATE(8)); // PC6 (TX)

  palClearPad(GPIOD, GPIOD_LED4);
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
  char *ret;
  while (TRUE) {
     uartStopReceive(&UARTD2); 
     chThdSleepMilliseconds(1);
     uartStopSend(&UARTD2);
     uartStopSend(&UARTD6);
     chThdSleepMilliseconds(1);
     uartStartReceive(&UARTD2, 80, buffer);
     chThdSleepMilliseconds(1);
     GRMC[49] = 0x0A;
     if(latitud != NULL)
     {
      uartStartSend(&UARTD6, strlen(latitud), latitud);
      chThdSleepMilliseconds(10);
      uartStartSend(&UARTD6, 2, "\n");
      chThdSleepMilliseconds(10);
      uartStartSend(&UARTD6, strlen(NSIndicador), NSIndicador);
      chThdSleepMilliseconds(10);
      uartStartSend(&UARTD6, 2, "\n");
      chThdSleepMilliseconds(10);
      uartStartSend(&UARTD6, strlen(longitud), longitud);
      chThdSleepMilliseconds(10);
      uartStartSend(&UARTD6, 2, "\n");
      chThdSleepMilliseconds(10);
      uartStartSend(&UARTD6, strlen(WEIndicador), WEIndicador);
      chThdSleepMilliseconds(10);
      uartStartSend(&UARTD6, 2, "\n");
      chThdSleepMilliseconds(10);
      
     }
     
     else
     uartStartSend(&UARTD6, 50, GRMC);

     //Verifica si el dato del GPS es valido
     ret = strchr(Validador, 'A'); 

     if(ret) palSetPad(GPIOD, GPIOD_LED4);


     chThdSleepMilliseconds(100);

 }
}
