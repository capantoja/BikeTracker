/**********************************************************
     ____  _ __      ______                __            
    / __ )(_) /_____/_  __/________ ______/ /_____  _____
   / __  / / //_/ _ \/ / / ___/ __ `/ ___/ //_/ _ \/ ___/
  / /_/ / / ,< /  __/ / / /  / /_/ / /__/ ,< /  __/ /    
 /_____/_/_/|_|\___/_/ /_/   \__,_/\___/_/|_|\___/_/  
 
                    Version 1.0


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
***********************************************************/ 

#include "ch.h"
#include "hal.h"
#include <string.h>

static char GRMC[80]; 
static char buffer[200];  
static char Validador;
static char latitud[10];
static char longitud[20];
static char NSIndicador;
static char WEIndicador;
static bool RX_Flag;


/* funcion que toma los datos de posicion del GPS */
void GPS(void)
{
  int i;
  int j = 0;
  char datos[80];
  int contador = 0;
  bool Flag = TRUE;
  uartStopReceive(&UARTD1); 
  chThdSleepMilliseconds(1);
  uartStopSend(&UARTD1);
  chThdSleepMilliseconds(1);
  
 // $GPRMC,001749.120,A,0328.1321,N,07629.3924,W,1.01,316.26,111116,,,A*7D

while(Flag)
{
  
  uartStartReceive(&UARTD1, 70, buffer);

if (RX_Flag)
{  
  
  // Guarda el mensaje RMC
  for (i = 0; i < 70; i++)
  {  
      if ((buffer[i] == '$') && (buffer[i+3] == 'R') && (i < 30)) // $GRMC message
      { 
        for(j=0; j<50; j++)
        {
          GRMC[j] = buffer[i+j]; 
        }
      }
  }

// Obtiene los datos de latitud y longitud
  for(j=0; j<50; j++)
        {
          if(datos[j] == ',')
          {
              contador++;
              switch(contador)
              {
                case 1: Validador = datos[j+1]; break;
                case 2: for (i = 0; i < 9; i++){ latitud[i] = datos[i+j+1];} break;
                case 3: NSIndicador = datos[j+1]; break;
                case 4: for (i = 0; i < 10; i++){ longitud[i] = datos[i+j+1];} break;
                case 5: WEIndicador = datos[j+1]; break;
                default: break;
              }
          }
        }     
  Flag = FALSE;
}
}

 RX_Flag = FALSE;       
 return;
}
  
/* Callback que se activa cuando el buffer de recepcion es llenado */  
static void rxend(UARTDriver *uartp) 
{

  (void)uartp;
  RX_Flag = TRUE;
  palClearPad(GPIOA, GPIOA_LED);
 
}

/* Configuracion del puerto serial 4800,8,n,1 */
static UARTConfig uartCfg = {
  NULL,
  NULL,
  rxend,
  NULL,
  NULL,
  4800,
  0,
  USART_CR2_LINEN,
  0
};



int main(void) {

  halInit();
  chSysInit(); 
    
  /* Inicializa UART */
  uartStart(&UARTD1, &uartCfg);                                                                                                                                                                                                                                                                                                        
  chThdSleepMilliseconds(1);
  
  /* Pin SAO = 0 */
  palClearPad(GPIOB, GPIOB_PIN_SAO);
   /* Activa GPS */
  palClearPad(GPIOA, GPIOA_ON_GPS);
  /* Activa serial para GPS */ 
  palSetPad(GPIOA, 3);
  palClearPad(GPIOF, 7);

  chThdSleepMilliseconds(100);
  //GPS();


  while (TRUE) {

     
     GPS();
    // chThdSleepMilliseconds(250);
   //  uartStopReceive(&UARTD1); 
   //  chThdSleepMilliseconds(1);
   //  uartStopSend(&UARTD1);
   //  chThdSleepMilliseconds(1);
    // uartStartSend(&UARTD1, strlen(latitud), latitud);
    // chThdSleepMilliseconds(1);
    // uartStartSend(&UARTD1, 5, "Hola\n");
     chThdSleepMilliseconds(250);
     palClearPad(GPIOA, GPIOA_LED);
     chThdSleepMilliseconds(250);
     palSetPad(GPIOA, GPIOA_LED);
     

  }
}
