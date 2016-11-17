
#include "ch.h"
#include "hal.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char RX_Data[20]; 
bool RX_Flag;
char message[] = "\n********** inicio **************\n";
char buffer[100];  
static char Data[100];

static WORKING_AREA(waThread1, 128);          // Declaracion del Thread para el led blink


/*
*  Thread para el led blink que indica que el sistema esta en modo RUN
*/ 

static msg_t Thread1(void *arg) 
{
  (void)arg;
  palSetPadMode(GPIOD, GPIOD_LED5, PAL_MODE_OUTPUT_PUSHPULL);
  
  while (TRUE) {
    palSetPad(GPIOD, GPIOD_LED5);       /* LED Red */
    chThdSleepMilliseconds(500);
    palClearPad(GPIOD, GPIOD_LED5);     
    chThdSleepMilliseconds(500);
  }
  return 0;
}


/*
 * This callback is invoked when a receive buffer has been completely written.
 * Callback para decodificar el dato recibido por el puerto serial
 */
static void rxend(UARTDriver *uartp) {
   
   (void)uartp;
   RX_Flag = TRUE;
   palClearPad(GPIOD, GPIOD_LED6);

return;
}


/*
 * UART 6 driver configuration structure.
 */
static UARTConfig uart_cfg_6 = {
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

/*
 * UART 2 driver configuration structure.
 */
static UARTConfig uart_cfg_2 = {
  NULL,
  NULL,
  rxend,
  NULL,
  NULL,
  115200,
  0,
  USART_CR2_LINEN,
  0
};

/* Funcion para enviar comando AT repetido hasta recibir respuesta */
uint8_t AT_Command2(char *ATcommand, char *Answer, uint8_t buffer_size)
{
    uint8_t answer = 0;
    bool Flag = TRUE;
    int contador = 0;
    int i;

      /* Detiene puertos seriales */
    uartStopReceive(&UARTD2); 
    chThdSleepMilliseconds(1);
    uartStopSend(&UARTD2);
    chThdSleepMilliseconds(1);

      /* Envia comando AT y espera respuesta */
    do
      {
          for(i = 0; i < 100; i++)
            {
              Data[i] = '0';
            }

          uartStartSend(&UARTD2, strlen(ATcommand), ATcommand);
          chThdSleepMilliseconds(1);
          uartStartReceive(&UARTD2, buffer_size, buffer);
         // chThdSleepMilliseconds(1);
         // uartStartSend(&UARTD6, strlen(ATcommand), ATcommand);
         // chThdSleepMilliseconds(1);
         // uartStartSend(&UARTD6, strlen(buffer), buffer);
       
          if (RX_Flag)
            {
              
               for(i = 0; i < buffer_size; i++)
               {
                  Data[i] = buffer[i];
               }
              if (strstr(Data, Answer) != NULL)    
                {
                  answer = 1;
                  Flag = FALSE;
                }

            }
          contador++;
          chThdSleepMilliseconds(100);      
      } while(Flag == TRUE && contador < 1000);
    RX_Flag = FALSE;   
return answer;
}


int main(void) {

  
  halInit();
  chSysInit();
  
  /* Inicializa el puerto serial UART6 C6(TX) y C7(RX) */
  uartStart(&UARTD6, &uart_cfg_6);
  palSetPadMode(GPIOC, 7, PAL_MODE_ALTERNATE(8));
  palSetPadMode(GPIOC, 6, PAL_MODE_ALTERNATE(8));

  /* Inicializa el puerto serial UART2 A2(TX) y A3(RX) */
  uartStart(&UARTD2, &uart_cfg_2);
  palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));

  /* Inicializa Thread para el blink led rojo */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);  // Blinker Thread

  palSetPadMode(GPIOD, GPIOD_LED6, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(GPIOA, 7, PAL_MODE_OUTPUT_PUSHPULL);
  chThdSleepSeconds(10);
  
  while (TRUE) 
  {
    palSetPad(GPIOA, 7);
  /* Detiene toda operacion de los puertos seriales */   
    uartStopReceive(&UARTD6); 
    uartStopSend(&UARTD6);
    uartStopReceive(&UARTD2); 
    uartStopSend(&UARTD2);

    uartStartSend(&UARTD6, strlen(message), message);
  //  uartStartSend(&UARTD2, 4, "AT\r\n");
    //chThdSleepMilliseconds(1);
   // uartStartReceive(&UARTD2, 5, buffer);

    //if (strstr("OK", buffer) != NULL)    
    // {
                 
    // }
     chThdSleepMilliseconds(500);
      if (AT_Command2("AT\r\n", "OK", 4))
       {
   //     palClearPad(GPIOD, GPIOD_LED6);
          uartStartSend(&UARTD6, strlen(Data), Data);
      
   //     chThdSleepMilliseconds(500);
   //     uartStartSend(&UARTD2, 1, "\r");
   //     palSetPad(GPIOD, GPIOD_LED6);
       }
  }
}
