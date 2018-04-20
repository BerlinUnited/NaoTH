import RPi.GPIO as GPIO
import time
 
GPIO.setmode(GPIO.BCM)
 
#set GPIO Pins
GPIO_INPUT_GOAL1 = 23
GPIO_INPUT_GOAL2 = 24
 
#set GPIO direction (IN / OUT)
GPIO.setup([GPIO_INPUT_GOAL1, GPIO_INPUT_GOAL2], GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

def handle_goal(pin):
  if pin == GPIO_INPUT_GOAL1:
    print ("Activated: Goal 1")
  
  if pin == GPIO_INPUT_GOAL2:
    print ("Activated: Goal 2")
  

if __name__ == '__main__':

  #GPIO.RISING
  #GPIO.BOTH
  GPIO.add_event_detect(BTN_G, GPIO.RISING, handle)

  # dummy waiting, this will be done better
  try:
    while True:
      time.sleep(0.01)

  # Reset by pressing CTRL + C
  except KeyboardInterrupt:
    print("Measurement stopped by User")
    GPIO.cleanup()
    
  GPIO.cleanup()
