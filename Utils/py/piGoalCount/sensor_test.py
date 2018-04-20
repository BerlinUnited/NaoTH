import RPi.GPIO as GPIO
import time
 
GPIO.setmode(GPIO.BCM)
 
#set GPIO Pins
GPIO_INPUT_GOAL1 = 23
GPIO_INPUT_GOAL2 = 24
 
#set GPIO direction (IN / OUT)
GPIO.setup(GPIO_INPUT_GOAL1, GPIO.IN)
GPIO.setup(GPIO_INPUT_GOAL2, GPIO.IN)
 
if __name__ == '__main__':
  try:
    while True:
      goal1 = GPIO.input(GPIO_INPUT_GOAL1)
      goal2 = GPIO.input(GPIO_INPUT_GOAL2)
      
      if goal1:
        print ("Activated: Goal 1")
      
      if goal2:
        print ("Activated: Goal 2")
      
      time.sleep(0.01)

    # Reset by pressing CTRL + C
  except KeyboardInterrupt:
    print("Measurement stopped by User")
    GPIO.cleanup()
    
  GPIO.cleanup()
