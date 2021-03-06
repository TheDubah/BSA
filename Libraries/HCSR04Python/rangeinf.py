import RPi.GPIO as GPIO
import time

GPIO.cleanup()

GPIO.setmode(GPIO.BCM)

TRIG = 23
ECHO = 24

print "Distance Measurement In Progress"

GPIO.setup(TRIG,GPIO.OUT)
GPIO.setup(ECHO,GPIO.IN)

list = []

for i in range(0,40):

	GPIO.output(TRIG, False)
	time.sleep(0.1)

	GPIO.output(TRIG, True)
	time.sleep(0.00001)
	GPIO.output(TRIG, False)

	while GPIO.input(ECHO)==0:
	  pulse_start = time.time()

	while GPIO.input(ECHO)==1:
	  pulse_end = time.time()

	pulse_duration = pulse_end - pulse_start

	distance = pulse_duration * 17150

	distance = round(distance, 2)

	list.append(distance)
	
print list

GPIO.cleanup()
