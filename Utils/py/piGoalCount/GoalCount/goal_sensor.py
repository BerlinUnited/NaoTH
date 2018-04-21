import RPi.GPIO as GPIO
import time
import threading

class GoalSensor:
  def __init__(self):
    
    GPIO.setmode(GPIO.BCM)
    
    #set GPIO Pins
    self.GPIO_INPUT_GOAL1 = 23
    self.GPIO_INPUT_GOAL2 = 24
    
    #set GPIO direction (IN / OUT)
    GPIO.setup([self.GPIO_INPUT_GOAL1, self.GPIO_INPUT_GOAL2], GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

    # goal counts
    self.goal1 = 0
    self.time1 = 0
    
    self.goal2 = 0
    self.time2 = 0
    
    self.time_delay = 5.0
    
    GPIO.add_event_detect(self.GPIO_INPUT_GOAL1, GPIO.RISING, self.updateGoal1)
    GPIO.add_event_detect(self.GPIO_INPUT_GOAL2, GPIO.RISING, self.updateGoal2)
    
    self.update = threading.Event()

  def updateGoal1(self, pin):
    print("updateGoal2")
    if self.time1 == 0 or self.time1 + self.time_delay < time.time():
      self.goal1 += 1
      self.time1 = time.time()
      update.set()
    
  def updateGoal2(self, pin):
    print("updateGoal2")
    if self.time2 == 0 or self.time2 + self.time_delay < time.time():
      self.goal2 += 1
      self.time2 = time.time()
      update.set()
      
  def reset(self):
    self.goal1 = 0
    self.time1 = 0
    self.goal2 = 0
    self.time2 = 0
      
  def close(self):
    GPIO.cleanup()


if __name__ == '__main__':

  print("GoalSensor active")
  goalSensor = GoalSensor()
  
  # dummy waiting, this will be done better
  try:
    while True:
      time.sleep(0.01)

  # Reset by pressing CTRL + C
  except KeyboardInterrupt:
    print("Measurement stopped by User")
    goalSensor.close()
    
  goalSensor.close()
