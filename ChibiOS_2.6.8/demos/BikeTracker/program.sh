#!/bin/sh
cd build
sudo openocd -f /home/lithium/Documentos/ChibiOS_2.6.8/demos/BikeTracker/Config.cfg -c "program BikeTracker.hex verify reset" -c shutdown
	

