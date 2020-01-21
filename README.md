# crazyflie_transform
Add a new PWM output using crazyflie. I use GPIO_IO1 to output a PWM signal to control a servomotor to achieve transformation.
The files are modified from the piezo.c
In the ROS, I use button 0 to control transformation. 
In the file, crazyflie_ros/crazyflie_demo/scripts
line 48, change the ring/headlightEnable to transform_deck/tfmEnable

1. Put the deck file "transform_deck.c" to deck/drivers/src, add the files transform.c and transform.h to drivers/src 
and drivers/interface respectively.
2. Add to the Makefile
#Drivers
PROJ_OBJ += transform.o (the name of the file added to drivers/src)
#DECKs
PROJ_OBJ += transform_deck.o (the name of the file added to deck/drivers/src)
