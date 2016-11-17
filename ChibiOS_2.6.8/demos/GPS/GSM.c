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
static char buffer[200];  
static char IMEI;     
const char URL[] = "http://biketrackerserver-serversecurity.rhcloud.com/add.php";
const char APN[] = "web.vmc.net.co";

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

static void rxend(UARTDriver *uartp) 
{

  (void)uartp;
  char *ret;

  ret = strstr(buffer, "OK");

  if(ret) palSetPad(GPIOD, GPIOD_LED4);
  else palClearPad(GPIOD, GPIOD_LED4);

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
  char message[]="862950025733240,4.636492,N,-74.083278,W,3.7";
  char AT[]="AT\r\n";
  while (TRUE) 
  {
     uartStopReceive(&UARTD2); 
     chThdSleepMilliseconds(1);
     uartStopSend(&UARTD2);
     uartStopSend(&UARTD6);
     chThdSleepMilliseconds(1);
     //uartStartReceive(&UARTD2, 80, buffer);
     chThdSleepMilliseconds(100);
     uartStartSend(&UARTD6, strlen(message), message);
     chThdSleepMilliseconds(100);
     

     uartStartSend(&UARTD2, strlen(AT), AT);
     chThdSleepMilliseconds(1);
     uartStartReceive(&UARTD2, 80, buffer);
     chThdSleepMilliseconds(100);


 }
}
