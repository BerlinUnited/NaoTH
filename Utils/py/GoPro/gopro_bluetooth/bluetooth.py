import subprocess
from time import sleep

def sendMessage(mac, msg):
  result = subprocess.run(['gatttool', '-i', 'hci0', '-t', 'random', '-b', mac, '--char-write-req', '--handle', '0x002f', '--value', msg], stdout=subprocess.PIPE).stdout.decode('utf-8')
  print (result)

def setWifiOn(mac):
  sendMessage(mac,'03170101')

def setWifiOff(mac):
  sendMessage(mac,'03170100')
  
def setBeepOn(mac):
  sendMessage(mac,'03160101')

def setBeepOff(mac):
  sendMessage(mac,'03160100')
  
if __name__ == '__main__':
  mac = 'F8:D2:E9:F0:AC:0B'
  mac = 'D6:B9:D4:D7:B7:40'
  #setWifiOn(mac)
  setBeepOn(mac)
  sleep(3)
  setBeepOff(mac)