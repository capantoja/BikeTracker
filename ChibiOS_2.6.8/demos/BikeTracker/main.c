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
#include "gps.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


static char GRMC[80]; 
static char buffer[200];  
static char Validador;
static char latitud[10];
static char longitud[20];
static char NSIndicador;
static char WEIndicador;
static char IMEI[15]; 
static char Carga[15];
static char URL[] = "\"http://biketrackerserver-serversecurity.rhcloud.com/add.php\"\r"; //Biketracker server
static char APN[] = "\"web.colombiamovil.com.co\"\r"; // TIGO APN
static char Data[100];
static char buffer[200];
char RX_Data[20]; 
bool RX_Flag;



//Declaracion de threads
static WORKING_AREA(blinker, 64);


/*  Thread para el led blink que indica que el sistema esta en modo RUN */

static msg_t Blink_LED(void *arg) 
{
  (void)arg;
  
  while (TRUE) {
    palSetPad(GPIOA, GPIOA_LED);     
    chThdSleepMilliseconds(500);
    palClearPad(GPIOA, GPIOA_LED);     
    chThdSleepMilliseconds(500);
  }
  return 0;
}
  
/* Callback que se activa cuando el buffer de recepcion es llenado */  
static void rxend(UARTDriver *uartp) 
{

  (void)uartp;
  RX_Flag = TRUE;
 
}

/* Configuracion del puerto serial 9600,8,n,1 */
static UARTConfig uartCfg = {
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
  /* Elimina Echo en la respuesta del modulo */
  uartStartSend(&UARTD1, 6, "ATE0\r");
  chThdSleepSeconds(5);
 
  if (AT_Command2("AT\r", "OK", 5)) GSM_OK = 1;

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

 
   strcpy(Data, "{\"IMEI\": ");
   strcat(Data, IMEI);
   strcat(Data, ", \"BATT\": ");
   strcat(Data, Carga);
   strcat(Data, ", \"LAT\": ");
   strcat(Data, latitud);
   strcat(Data, ", \"LON\": ");
   strcat(Data, longitud);
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

void InicializaGSM(void)
{
  /* Activa serial para GSM */ 
  palClearPad(GPIOA, 3);
  palSetPad(GPIOF, 7);

  /* Inicializa modulo GSM */
  if(GSM_init())
    {   
      if (AT_Command2("AT+CREG?\r", "+CREG: 0,1", 15))
      {
        if (AT_Command2("AT+CGATT?\r", "+CGATT: 1", 15)){}
      }
    }
  GSM_OFF();
  return;
}

void GPS_on(void)
{
  /* Activa GPS */
  palClearPad(GPIOA, GPIOA_ON_GPS);
  return;
}

void GPS_off(void)
{
  /* Activa GPS */
  palSetPad(GPIOA, GPIOA_ON_GPS);
  return;
}

int main(void) {

  halInit();
  chSysInit(); 

  /* Inicializa UART */
  uartStart(&UARTD1, &uartCfg);                                                                                                                                                                                                                                                                                                        
  
  /* Inicializa Thread para el blink led */
  chThdCreateStatic(blinker, sizeof(blinker), NORMALPRIO, Blink_LED, NULL);  // Blinker Thread
  
  /* Pin SAO = 0 */
  palClearPad(GPIOB, GPIOB_PIN_SAO);
   
  GPS_on();
  InicializaGSM();
 

  
  

  while (TRUE) 
  {






  }
}
