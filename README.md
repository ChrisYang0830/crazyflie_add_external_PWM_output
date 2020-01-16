# crazyflie_transform
Add a new PWM output using crazyflie. I use GPIO_IO1 to output a PWM signal to control a servomotor to achieve transformation.
The files are modified from the piezo.c

1. Put the deck file "transform_deck.c" to deck/drivers/src, add the files transform.c and transform.h to drivers/src 
and drivers/interface respectively.
2. Add to the Makefile
#Drivers
PROJ_OBJ_CF2 += transform.o
#DECK API
PROJ_OBJ_CF2 += transform_deck.o
