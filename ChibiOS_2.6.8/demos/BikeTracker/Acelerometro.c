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


***********************************************************/ 

#include "ch.h"
#include "hal.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h> 


// Parametros del acelerometro MMA8451Q 
#define MMA8451_ADDRESS  0x1C  // MMA8451Q address: 0x1C: pin SAO = 0  0x1D: pin SAO = 1; 
#define WHO_AM_I         0x0D
#define CTRL_REG1        0x2A
#define CTRL_REG2        0x2B
#define MODS_MASK        0x03
#define MOTION_REG       0x15
#define MOTION_DET       0X16
#define MOTION_THRES     0X17        
#define MOTION_COUNTER   0X18 
#define MOTION_CFG       0x78
#define ACCEL_RX_DEPTH   6
#define ACCEL_TX_DEPTH   2   

msg_t status = RDY_OK;     
static uint8_t rxbuf[ACCEL_RX_DEPTH];
static uint8_t txbuf[ACCEL_TX_DEPTH];
static uint8_t data_accel = 0;
     


/* I2C timming configuration Standard mode 100kHz, for clock = 8MHz */
static const I2CConfig i2cconfig = {
  STM32_TIMINGR_PRESC(0x01) |
  STM32_TIMINGR_SCLDEL(0x04) | STM32_TIMINGR_SDADEL(0x02) |
  STM32_TIMINGR_SCLH(0x0F)  | STM32_TIMINGR_SCLL(0x013),
  0,
  0
};


void MMA8451Standby(void){

  txbuf[0] = CTRL_REG1; /* register address */
  txbuf[1] = 0x0;
  status = i2cMasterTransmitTimeout(&I2CD1, MMA8451_ADDRESS, txbuf, sizeof(txbuf), rxbuf, 0, TIME_INFINITE);
  if (status != RDY_OK){} 
}

void MMA8451Active(void){ 
 // Activate MMA8451Q,  ODR: 6,25Hz 160mS
  chThdSleepMilliseconds(10);
  txbuf[0] = CTRL_REG1; /* register address */
  txbuf[1] = 0x31;
  status = i2cMasterTransmitTimeout(&I2CD1, MMA8451_ADDRESS, txbuf, sizeof(txbuf), rxbuf, 0, TIME_INFINITE);
  if (status != RDY_OK){}

}

void init_MMA8451Q(void){

  MMA8451Standby();

  chThdSleepMilliseconds(10);
  txbuf[0] = CTRL_REG2; /* register address */
  txbuf[1] = MODS_MASK; /* Low power mode */
  status = i2cMasterTransmitTimeout(&I2CD1, MMA8451_ADDRESS, txbuf, sizeof(txbuf), rxbuf, 0, TIME_INFINITE);
  if (status != RDY_OK){}

  chThdSleepMilliseconds(10);
  txbuf[0] = MOTION_REG; /* register address */
  txbuf[1] = MOTION_CFG; /* Motion detection mode */
  status = i2cMasterTransmitTimeout(&I2CD1, MMA8451_ADDRESS, txbuf, sizeof(txbuf), rxbuf, 0, TIME_INFINITE);
  if (status != RDY_OK){}

  chThdSleepMilliseconds(10);
  txbuf[0] = MOTION_THRES; /* register address */
  txbuf[1] = 0x8F;         /* DBCNT = 1; THRESHOLD = 16 */
  status = i2cMasterTransmitTimeout(&I2CD1, MMA8451_ADDRESS, txbuf, sizeof(txbuf), rxbuf, 0, TIME_INFINITE);
  if (status != RDY_OK){}
  
  chThdSleepMilliseconds(10);
  txbuf[0] = MOTION_COUNTER; /* register address */
  txbuf[1] = 0x0F;           /* Debounce counter = 16 */
  status = i2cMasterTransmitTimeout(&I2CD1, MMA8451_ADDRESS, txbuf, sizeof(txbuf), rxbuf, 0, TIME_INFINITE);
  if (status != RDY_OK){}
  
  chThdSleepMilliseconds(10);
  MMA8451Active();
  
}


int main(void) {

  halInit();
  chSysInit();  
                                                                                                                                                                                                                                                                                                       
  /* Starts I2C */
  i2cStart(&I2CD1, &i2cconfig);
  /* Desactiva GSM */
  palClearPad(GPIOA, GPIOA_ON_GSM);              
  /* Desactiva GPS */
  palClearPad(GPIOA, GPIOA_ON_GPS);
  /* Pin SAO = 0 */
  palClearPad(GPIOB, GPIOB_PIN_SAO);
  /* Configura el acelerometro MMA8451Q */
  init_MMA8451Q();
  



  while (TRUE) 
  {

    //txbuf[0] = MOTION_DET;  /* register address */  
    txbuf[0] = WHO_AM_I;
    status = i2cMasterTransmitTimeout(&I2CD1, MMA8451_ADDRESS, txbuf, sizeof(txbuf), rxbuf, 1, TIME_INFINITE);
    if (status != RDY_OK){}


    data_accel = rxbuf[0];

    if(data_accel == 0x1A){
    //palSetPad(GPIOA, GPIOA_LED);
    // chThdSleepMilliseconds(250);
    palClearPad(GPIOA, GPIOA_LED);
   //  chThdSleepMilliseconds(250);
   }

    /* Detecta movimiento */
    if ((data_accel >> 7) & 1) 
    {
      
    }
    else 
    {
     

    }
  }
}
