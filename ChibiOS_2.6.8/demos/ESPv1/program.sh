#!/bin/sh
cd build

sudo openocd -f /home/lithium/Documentos/ChibiOS_2.6.8/demos/ESPv1/stlink-v2.cfg -c "program ch.elf verify reset" -c shutdown
