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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static char IMEI[15]; 
static char Carga[15];
static char URL[] = "\"http://biketrackerserver-serversecurity.rhcloud.com/add.php\"\r"; //Biketracker server
static char APN[] = "\"web.colombiamovil.com.co\"\r"; // TIGO APN
static char Data[100];
static char buffer[200];
char RX_Data[20]; 
bool RX_Flag;

 /* Callback que se ejecuta cuando el buffer de recepcion se llena */
static void rxend(UARTDriver *uartp) 
{

  (void)uartp;
  RX_Flag = TRUE;

}

/* Configuracion del puerto Serial 9600,8,n,1 */
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

/* Funcion para enviar comando AT repetido hasta recibir respuesta */
uint8_t AT_Command2(char *ATcommand, char *Answer, uint8_t buffer_size)
{
    uint8_t answer = 0;
    bool Flag = TRUE;
    int contador = 0;
    int i;

      /* Detiene puertos seriales */
    uartStopReceive(&UARTD1); 
    chThdSleepMilliseconds(1);
    uartStopSend(&UARTD1);
    chThdSleepMilliseconds(1);

      /* Envia comando AT y espera respuesta */
    do
      {
          for(i = 0; i < 100; i++)
            {
              Data[i] = '0';
            }

          uartStartSend(&UARTD1, strlen(ATcommand), ATcommand);
          uartStartReceive(&UARTD1, buffer_size, buffer);
       
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

/* Funcion para enviar comando AT una sola vez y espera respuesta */
uint8_t AT_Command(char *ATcommand, char *Answer, uint8_t buffer_size)
  {

    uint8_t answer = 0;
    bool Flag = TRUE;
    int i;

      /* Detiene puertos seriales */
    uartStopReceive(&UARTD1); 
    chThdSleepMilliseconds(1);
    uartStopSend(&UARTD1);
    chThdSleepMilliseconds(1);
    uartStartSend(&UARTD1, strlen(ATcommand), ATcommand);
    uartStartReceive(&UARTD1, buffer_size, buffer);

      /* Espera respuesta */
    do
      {  
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
      } while(Flag);

    for(i = 0; i < 100; i++)
    {
      Data[i] = '0';
    }

    RX_Flag = FALSE; 
    
return answer;
  }

/* Obtiene el numero IMEI */
void IMEI_GET(void)
{     
  char IMEI_query[]="AT+GSN\r";
  int i;
  bool Flag = TRUE;

/* Detiene puertos seriales */
  uartStopReceive(&UARTD1); 
  chThdSleepMilliseconds(1);
  uartStopSend(&UARTD1);
  chThdSleepMilliseconds(1);

    while(Flag)
      {
          for(i = 0; i < 15; i++)
            {
              IMEI[i] = '0';
            }
          uartStartSend(&UARTD1, strlen(IMEI_query), IMEI_query);
          uartStartReceive(&UARTD1, 25, buffer);      
          if (RX_Flag)
            {
               for(i = 0; i < 17; i++)
               {
                  if(i > 1)
                  {
                    IMEI[i-2] = buffer[i];
                  } 
               }
                  Flag = FALSE;
            }
        chThdSleepMilliseconds(100);
      }
  RX_Flag = FALSE; 
  return;
}

/* Obtiene el estado de la carga de la Bateria */
void BATT_GET(void)
{     
  char BATT_query[]="AT+CBC\r";
  char Batt[15]; 
  int i;
  bool Flag = TRUE;


/* Detiene puertos seriales */
  uartStopReceive(&UARTD1); 
  chThdSleepMilliseconds(1);
  uartStopSend(&UARTD1);
  chThdSleepMilliseconds(1);
     /* Envia comando AT y espera respuesta */
    while(Flag)
      {
          for(i = 0; i < 15; i++)
            {
              Batt[i] = '0';
            }
          uartStartSend(&UARTD1, strlen(BATT_query), BATT_query);
          uartStartReceive(&UARTD1, 20, buffer);
          if (RX_Flag)
            {
               for(i = 0; i < 15; i++)
               {
                  Batt[i] = buffer[i];
               }
               for(i = 0; i < 15; i++)
               {
                  if(i > 9 && i < 13)
                    { 
                      Carga[i-10] = Batt[i];
                    }
               }
              Flag = FALSE;
            }
        chThdSleepMilliseconds(100);  
        
      }
  RX_Flag = FALSE;   
  return;
}  

/* Funcion que inicializa el modulo GSM */
uint8_t GSM_init(void)
{                        

  uint8_t GSM_OK = 0;
 /* Pulso de activacion del modulo GSM  */
  palSetPad(GPIOA, GPIOA_ON_GSM);
  chThdSleepSeconds(3);
  palClearPad(GPIOA, GPIOA_ON_GSM);
  chThdSleepSeconds(5);
  uartStopReceive(&UARTD1); 
  chThdSleepMilliseconds(1);
  uartStopSend(&UARTD1);
  chThdSleepMilliseconds(1);
  /* Echo OFF */
  uartStartSend(&UARTD1, 6, "ATE0\r");
  chThdSleepSeconds(5);
 
  if (AT_Command2("AT\r", "OK", 5))
   {
      IMEI_GET();
      BATT_GET();
      GSM_OK = 1;
   } 

  return GSM_OK;
}

/* Funcion para encender el modulo GSM */
void GSM_ON(void)
{                                           
 /* Pulso de activacion del modulo GSM  */
  palSetPad(GPIOA, GPIOA_ON_GSM);
  chThdSleepSeconds(3);
  palClearPad(GPIOA, GPIOA_ON_GSM);
  chThdSleepSeconds(5);
  return;
}

/* Funcion para apagar el modulo GSM */
void GSM_OFF(void)
{                                           
 /* Pulso de desactivacion del modulo GSM  */
  palSetPad(GPIOA, GPIOA_ON_GSM);
  chThdSleepSeconds(3);
  palClearPad(GPIOA, GPIOA_ON_GSM);
  chThdSleepSeconds(5);
  return;
}

/* funcion que hace post al servidor */
uint8_t HTTP_Post(void)
{

/* COMANDOS GSM PARA POST DE DATOS
AT+SAPBR=3,1,"Contype","GPRS"
AT+SAPBR=3,1,"APN","web.colombiamovil.com.co" 
AT+SAPBR=1,1 
AT+HTTPINIT
AT+HTTPPARA="CID",1
AT+HTTPPARA="URL","http://biketrackerserver-serversecurity.rhcloud.com/add.php"
AT+HTTPDATA=74,10000
{"IMEI": 862950025733240, "BATT": 3.7, "LAT": 4.636492, "LON": 74.083278} 
AT+HTTPACTION=1
AT+HTTPTERM
AT+SAPBR=0,1
*/

  uint8_t POST_OK = 0;
  char SetApn[50];
  char SetUrl[100];
  strcpy(SetApn, "AT+SAPBR=3,1,\"APN\",");
  strcat(SetApn, APN);
  strcpy(SetUrl, "AT+HTTPPARA=\"URL\",");
  strcat(SetUrl, URL);

  /////////////////// datos para prueba /////////////////////////////
   char lat[] = "4.636492";
   char lon[] = "74.083278";
   strcpy(Data, "{\"IMEI\": ");
   strcat(Data, IMEI);
   strcat(Data, ", \"BATT\": ");
   strcat(Data, Carga);
   strcat(Data, ", \"LAT\": ");
   strcat(Data, lat);
   strcat(Data, ", \"LON\": ");
   strcat(Data, lon);
   strcat(Data, "}\r");

//////////////////////////////////////////////////////////
uartStartSend(&UARTD1, strlen(SetApn), SetApn);
chThdSleepMilliseconds(100);
uartStartSend(&UARTD1, strlen(SetUrl), SetUrl);
chThdSleepMilliseconds(100);
uartStartSend(&UARTD1, strlen(Data), Data);
chThdSleepMilliseconds(100);

/*
 if(AT_Command2("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r", "OK", 5))
   {
    if(AT_Command2(SetApn, "OK", 5)) 
      {
        if(AT_Command2("AT+SAPBR=1,1\r", "OK", 5))
          {
            if(AT_Command2("AT+HTTPINIT\r", "OK", 5))
              {
                if(AT_Command2("AT+HTTPPARA = \"CID\",1\r", "OK", 5))
                  {
                    
                    if(AT_Command2(SetUrl, "OK", 5))
                      {
                        
                        if(AT_Command2("AT+HTTPDATA=74,10000\r", "DOWNLOAD",15))
                          {
                            if(AT_Command2(Data, "OK", 10))
                              {
                                if(AT_Command("AT+HTTPACTION=1\r", "OK", 10))
                                  {
                                    do{
                                        uartStartReceive(&UARTD1, 25, buffer);
                                        chThdSleepMilliseconds(1);
                                          if (RX_Flag)
                                            {
                                              if (strstr(buffer, "+HTTPACTION: 1,200,0") != NULL) 
                                                {
                                                  if(AT_Command2("AT+HTTPTERM\r", "OK", 10))
                                                    {
                                                      palClearPad(GPIOA, GPIOA_LED);
                                                      POST_OK = 1;
                                                    }
                                                }   
                                            }
                                      }while(POST_OK != 1);      
                                  }
                              }
                          }                    
                      } 
                  }
              }
          }
      }    
  }
    */
return POST_OK;
}




int main(void) 
{

  halInit();
  chSysInit(); 
    
  /* Inicia el driver serial */
  uartStart(&UARTD1, &uartCfg);                                                                                                                                                                                                                                                                                                        
  chThdSleepMilliseconds(1);
  
  
  /* Pin SAO = 0 */
  palClearPad(GPIOB, GPIOB_PIN_SAO);
   /* Activa GPS */
  palClearPad(GPIOA, GPIOA_ON_GPS);
  /* Activa serial para GSM */ 
  palClearPad(GPIOA, 3);
  palSetPad(GPIOF, 7);

  if(GSM_init())
    {   
      if (AT_Command2("AT+CREG?\r", "+CREG: 0,1", 15))
      {
        if (AT_Command2("AT+CGATT?\r", "+CGATT: 1", 15)){}
      }
    }
  GSM_OFF();


  while (TRUE) 
  {

  }
}
