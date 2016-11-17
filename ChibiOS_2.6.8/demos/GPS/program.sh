#!/bin/sh
cd build
sudo openocd -f /home/lithium/Documentos/ChibiOS_2.6.8/demos/GPS/Config.cfg -c "program ch.hex verify reset" -c shutdown
	

