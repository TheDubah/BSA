INSTRUCTIONS:

TO COMPILE AND CLEAN:
	-'make' command in same directory as .c and Makefile files compiles with gcc flags
	-'make clean' command removes current compiled binaries. (ALWAYS DO THIS BEFORE RECOMPILING!!)
	
TO HOOKUP ARM TO CONTROLLER:
	-Don't forget to use 9V power supply for controller.
	-Connect each servo (BLACK=GROUND) to 3-pin channel headers on controller.
	-Connect mini-USB cable to controller and first USB port on Raspi.
	
TO RUN PROGRAM:
	-Add your own code to main() to test out the 'servo_command()' functions.
	-The order of parameters is (channel,pulse,time) for servo_command1. For multiple servos it's (channel1,pulse1,channel2,pulse2,...time).
	-'./ssc32u' command will run the compiled binary