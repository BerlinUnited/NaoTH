#!/usr/bin/env python3
# -*- coding: utf8 -*-

import RPi.GPIO as GPIO
import socket
import time
import json
import traceback

import tempfile
from daemonize import Daemonize
import os

class StatusMonitor():
  def __init__(self):
    self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    
  def sendMessage(self, msg):
    self.sock.sendto(msg.encode(), ("localhost", 8000))
    
  def setNoGoProNetwork(self, delay = 1):
    self.sendMessage('{{"blue":"blink", "delay":"0.1", "time":"{}"}}'.format(delay))
    
  def setConnectingToGoPro(self, delay = 1):
    self.sendMessage('{{"blue":"blink", "time":"{}"}}'.format(delay))
    
  def setConnectedToGoPro(self, delay = 1):
    self.sendMessage('{{"blue":"on", "time":"{}"}}'.format(delay))
    
  def setReceivedMessageFromGCWithInvisibleTeam(self, delay = 1):
    self.sendMessage('{{"green":"blink", "time":"{}"}}'.format(delay))
    
  def setReceivedMessageFromGC(self, delay = 1):
    self.sendMessage('{{"green":"on", "time":"{}"}}'.format(delay))
    
  def setDidntReceivedMessageFromGC(self, delay = 1):
    self.sendMessage('{{"green":"off", "time":"{}"}}'.format(delay))
    
  def setReceivedMessageFromGC(self, delay = 1):
    self.sendMessage('{{"green":"on", "time":"{}"}}'.format(delay))
  
  def setRecordingOn(self, delay = 1):
    self.sendMessage('{{"red":"blink", "time":"{}"}}'.format(delay))
    
  def setRecordingOff(self, delay = 1):
    self.sendMessage('{{"red":"off", "time":"{}"}}'.format(delay))
  
    
class LED():
  def __init__(self, PIN):
    self.PIN = PIN

    #GPIO.setwarnings(False)
    GPIO.setup(self.PIN, GPIO.OUT)
    
    self._set(False)
    self.blink_delay = 0
    self.message_validity_time = 0
    
  def _set(self, state):
    self.state = state
    self.time = time.time()
    
    if self.state:
      GPIO.output(self.PIN,GPIO.HIGH)
    else:
      GPIO.output(self.PIN,GPIO.LOW)
      
  def on(self):
    self.blink_delay = 0
    self._set(True)
    
  def off(self):
    self.blink_delay = 0
    self._set(False)
      
  def blink(self, delay = 1.0):
    self.blink_delay = delay
    if self.blink_delay <= 0:
      self.blink_delay = 0
    else:
      self.update()

  def update(self):
    #print ('update: ', self.time, self.blink_delay)
    
    #hack: off phase is allways 1s
    if self.state:
      delay = self.blink_delay
    else:
      delay = 1.0
      
    if self.blink_delay > 0 and time.time() > self.time + delay:
      self._set(not self.state)
    
    if self.message_validity_time > 0 and time.time() > self.message_validity_time:
      self._set(False)
      
    
class LEDServer():
  def __init__(self):
  
    GPIO.setmode(GPIO.BCM)
    
    self.leds = {
      "red":    LED(22),
      "blue":   LED(17),
      "green":  LED(27)
    }
    
    self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    self.sock.bind(("localhost", 8000))
    self.sock.settimeout(0.1)

  def reset(self):
    for name in self.leds:
      self.leds[name].off()
    
  def update(self):
    for name in self.leds:
      self.leds[name].update()
    
  def start_animation(self):
    
    for i in range(0,3):
      for name in self.leds:
        self.leds[name].on()
      self.update()
      
      time.sleep(0.2)
      
      for name in self.leds:
        self.leds[name].off()
        
      self.update()
      
      time.sleep(0.2)
      
    self.update()
    
      
  def run(self):
  
    self.start_animation()
    
    while True:
      #print ('\nwaiting to receive message')
      try:
        data, address = self.sock.recvfrom(4096)
        
        #print ('received %s bytes from %s' % (len(data), address))
        #print (data.decode('utf-8'))
        
        # parse the message
        msg = json.loads(data.decode('utf-8'))
        
        for name in msg:
          if name in self.leds:
            led = self.leds[name]
            if msg[name] == 'on':
              led.on()
            elif msg[name] == 'off':
              led.off()
            elif msg[name] == 'blink':
              if "delay" in msg:
                led.blink(float(msg['delay']))
              else:
                led.blink()
            
            if 'time' in msg and float(msg['time']) > 0:
              led.message_validity_time = time.time() + float(msg['time'])
          
      except socket.timeout:
        pass
      except (KeyboardInterrupt, SystemExit):
        logger.debug("Interrupted or Exit")
        break
      except:
        traceback.print_exc()
        
      self.update()

def main():
  server = LEDServer()
  server.run()
      
statusMonitor = StatusMonitor()

if __name__ == "__main__":
  
  # define vars
  tempdir = tempfile.gettempdir()
  name = 'led'  # os.path.basename(sys.argv[0])
  # check for existing lock file and running process
  lock_file = os.path.join(tempdir, name + '.lock')
  
  daemon = Daemonize(app=name, pid=lock_file, action=main, foreground=True)
  daemon.start()
  