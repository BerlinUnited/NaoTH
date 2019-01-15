import subprocess
import shutil
import re
import threading
import time

from utils import Logger, blackboard
from utils.Bluetooth import Bluetooth

# setup logger for network related logs
logger = Logger.getLogger("Network")


class Network(threading.Thread):
    """ Handles the connection to the GoPro wifi network."""

    def __init__(self, device:str, ssid:str, passwd:str, retries:int, mac:str = None):
        super().__init__()
        # creates a appropriate manager, based on the available network applications
        self.manager = NetworkManagerNmcli() if shutil.which('nmcli') is not None else NetworkManagerIw()

        self.device = self.manager.getWifiDevice(device)
        self.ssid = ssid
        self.passwd = passwd
        self.mac = mac
        self.retries = retries
        self.bt = None

        blackboard['network'] = 0

        self.__cancel = threading.Event()
        self.__timer = threading.Event()

    def setConfig(self, device:str, ssid:str, passwd:str, retries:int, mac:str = None):
        # set the new params
        self.device = self.manager.getWifiDevice(device)
        self.ssid = ssid
        self.passwd = passwd
        self.mac = mac
        self.retries = retries
        # NOTE: auto-reconnects on the next 'isConnected' check
        try:
            self.__timer.set()
        except:
            pass

    def connect(self):
        logger.info("Setting up network")
        if not self.isConnected():
            # check wifi device
            device = self.manager.getWifiDevice(self.device)
            logger.info("Using device %s", device)

            # wait for connection
            while not self.__cancel.is_set():
                # check if ssid is available
                if not self.manager.getSSIDExists(device, self.ssid):
                    logger.info("SSID not found: %s", self.ssid)
                    blackboard['network'] = 0 # NetworkNotAvailable
                    time.sleep(1)
                    # if canceled, skip everything
                    if self.__cancel.is_set():
                        continue
                    self.wakeGoProWLAN()
                else:
                    logger.info("Waiting for connection to %s", self.ssid)
                    blackboard['network'] = 2 # NetworkConnecting
                    network = self.manager.connectToSSID(device, self.ssid, self.passwd)

                    if network is None:
                        logger.error("Couldn't connect to network '%s' with device '%s'!", self.ssid, self.device)
                        blackboard['network'] = 1 # NetworkDisconnected
                        self.__timer.wait(10)
                    else:
                        logger.info("Connected to %s", network)
                        blackboard['network'] = 3 # NetworkConnected
                        break
        else:
            blackboard['network'] = 3 # NetworkConnected

    def disconnect(self):
        # TODO:
        blackboard['network'] = 1 # NetworkDisconnected

    def isConnected(self):
        return self.ssid == self.manager.getCurrentSSID(self.device, False)

    def wakeGoProWLAN(self):
        if self.mac is not None:
            logger.debug("Trying to activate network via bluetooth")
            self.bt = Bluetooth(self.mac)
            self.bt.setWifiOn().start()
            self.bt.join()
        else:
            logger.warning("Bluetooth MAC address not set!")

    def run(self):
        # connect and fire connected event
        self.connect()
        while not self.__cancel.is_set():
            if self.isConnected():
                self.__timer.wait(10)
            else:
                blackboard['network'] = 1 # NetworkDisconnected
                self.wakeGoProWLAN()
                self.connect()
            self.__timer.clear()
        logger.debug("Network thread finished.")

    def cancel(self):
        self.__cancel.set()
        self.manager.cancel()
        if self.bt is not None:
            self.bt.cancel()
        try:
            self.__timer.set()
        except Exception as e:
            # ignore un-acquired locks
            pass


class NetworkManager:
    def __init__(self):
        self._cancel = threading.Event()

    def getWifiDevices(self):
        ''' Retrieves all wifi devices.

        :return:        a list of available wifi devices
        '''

        return []

    def getWifiDevice(self, device:str=None):
        ''' Checks whether the given wifi device is available or returns the available wifi device if nothing given.
            There had to be only one wifi device if nothing was given. Otherwise an error is printed out.

        :param device:  the device, which should be checked
        :return:        a wifi device or None
        '''

        if device is None:
            devices = self.getWifiDevices()
            if len(devices) > 1:
                logger.error("Multiple devices available!")
            elif len(devices) == 0:
                logger.error("No device available!")
            else:
                return devices[0]
        elif device not in self.getWifiDevices():
            logger.error("Device '%s' is not available!", device)
        else:
            return device
        return None

    def getCurrentSSID(self, device: str, log=True):
        ''' Checks if the given device is currently connected and returns the SSID of the connected network otherwise returns None.

        :param device:  name of the device from which we want to get the current SSID
        :return:        the current SSID of the device
        '''

        return None

    def getSSIDExists(self, device:str, ssid:str):
        return False

    def connectToSSID(self, device:str, ssid:str, passwd:str=None):
        ''' Connects the given device to the given SSID with the provided password.

        :param device:  name of the device we're like to connect with
        :param ssid:    SSID of the network we're like to connect with
        :param passwd:  the password of the SSID we're like to connect with
        :return:        the SSID of the connected network or 'None' if an error occurred
        '''

        if self.getCurrentSSID(device) == ssid:
            return ssid

        return self._connect(device, ssid, passwd)

    def _connect(self, device: str, ssid: str, passwd: str = None):
        return None

    def getAPmac(self, device: str):
        return None

    def cancel(self):
        self._cancel.set()


class NetworkManagerNmcli(NetworkManager):

    def getWifiDevices(self):
        '''
            getting available devices:  nmcli -t device
        '''
        wifi_devices = []

        logger.debug("Get all available wifi devices: 'nmcli -t device'")
        devices = subprocess.run(['nmcli', '-t', 'device'], stdout=subprocess.PIPE).stdout.decode('utf-8')
        for dev in devices.split('\n'):
            if dev:
                dev_state = dev.split(':')
                if dev_state[1] == 'wifi':
                    wifi_devices.append(dev_state[0])
        return wifi_devices

    def getCurrentSSID(self, device:str, log=True):
        '''
            checking connection:        nmcli -g GENERAL.STATE device show <device>
            determine network:          nmcli -g GENERAL.CONNECTION device show <device>
        '''
        if log:
            logger.debug("Check device state: 'nmcli -g GENERAL.STATE device show %s'", device)
        state = subprocess.run(['nmcli', '-g', 'GENERAL.STATE', 'device', 'show', device], stdout=subprocess.PIPE).stdout.decode('utf-8').strip()
        # do we have an established connection
        if state.split()[0] == '100':
            if log:
                logger.debug("Get SSID of current connection: 'nmcli -g GENERAL.CONNECTION device show %s'", device)
            # return SSID of current connection
            return subprocess.run(['nmcli', '-g', 'GENERAL.CONNECTION', 'device', 'show', device], stdout=subprocess.PIPE).stdout.decode('utf-8').strip()
        return None

    def getSSIDExists(self, device: str, ssid: str):
        logger.debug("Scan for the SSID: 'nmcli device wifi rescan ifname %s'", device)
        subprocess.run(['nmcli', 'device', 'wifi', 'rescan', 'ifname', device], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL).stdout.decode('utf-8').strip()
        logger.debug("Check SSID: 'nmcli device wifi list ifname %s'", device)
        result = subprocess.run(['nmcli', 'device', 'wifi', 'list', 'ifname', device], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL).stdout.decode('utf-8').strip()
        match = re.search(r'.*\s+' + ssid + '\s+.*', result)
        return (match is not None)

    def getAPmac(self, device: str):
        # TODO: !
        logger.debug("Scan for the MAC: 'iwconfig %s'", device)
        result = subprocess.run(['iwconfig', device], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL).stdout.decode(
            'utf-8').strip()
        # Access Point: F6:DD:9E:87:A1:63
        match = re.search(r'.*Access Point: (..:..:..:..:..:..).*', result)
        if match is None:
            return None
        else:
            return match.group(1).replace(':', '')

    def _connect(self, device:str, ssid:str, passwd:str=None):
        '''
            get existing networks:      nmcli connection show <ssid>
            connect to network:         nmcli connection up <ssid> ifname <device>
            create/connect network:     nmcli device wifi connect <ssid> password <passwd> ifname <device>
        '''
        logger.debug("Check if wifi network profile already exists: 'nmcli connection show %s'", ssid)
        # exists a saved wifi network with the given SSID?
        if subprocess.run(['nmcli', 'connection', 'show', ssid], stdout=subprocess.DEVNULL,
                          stderr=subprocess.DEVNULL).returncode == 0:
            logger.debug("Connect to existing wifi network profile: 'nmcli connection up %s ifname %s'", ssid, device)
            # try to activate the saved wifi network with the given SSID
            r = subprocess.run(['nmcli', 'connection', 'up', ssid, 'ifname', device], stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE)
            if r.returncode != 0:
                logger.error("Couldn't connect to network '%s' with device '%s'!\n\twhat: %s", ssid, device,
                             str(r.stderr, 'utf-8').strip())
            else:
                return ssid
        # ... else create new connection - if password given
        elif passwd is not None:
            logger.debug(
                "Create a new wifi network profile and connect: 'nmcli device wifi connect %s password %s ifname %s'",
                ssid, passwd, device)
            r = subprocess.run(['nmcli', 'device', 'wifi', 'connect', ssid, 'password', passwd, 'ifname', device])
            if r.returncode != 0:
                logger.error("Couldn't connect to network '%s' with device '%s'; wrong credentials?\n\twhat: %s", ssid,
                             device, str(r.stderr, 'utf-8').strip())
            else:
                return ssid
        else:
            logger.error("Could not connect to %s, password required!", ssid)
        return None


class NetworkManagerIw(NetworkManager):
    def getWifiDevices(self):
        '''
            getting available devices:  iw dev
        '''

        wifi_devices = []

        logger.debug("Get all available wifi devices: 'iw dev'")
        devices = subprocess.run(['iw', 'dev'], stdout=subprocess.PIPE).stdout.decode('utf-8')
        for dev in devices.split('\n'):
            if dev:
                match = re.search('Interface\s+(\w+)', dev.strip())
                if match:
                    wifi_devices.append(match.group(1))
        return wifi_devices

    def getCurrentSSID(self, device:str, log=True):
        '''
            determine current network:  iwgetid <device> -r
        '''
        # old: iwgetid %s -r / wpa_cli -i %s status

        logger.debug("Get current wifi SSID: 'iwconfig %s'", device)
        result = subprocess.run(['iwconfig', device], stdout=subprocess.PIPE)
        search = re.search(r'ESSID:"(\S+)"', result.stdout.decode('utf-8').strip(), re.M)
        # do we have an established connection
        if search:
            # return SSID of current connection
            return search.group(1).strip()

        return None

    def getSSIDExists(self, device:str, ssid:str):
      # make sure the device is up
      self.__checkDeviceState(device)
      logger.debug("Scan for the SSID: 'iwlist %s scanning essid %s'", device, ssid)
      result = subprocess.run(['iwlist', device, 'scanning', 'essid', ssid], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL).stdout.decode('utf-8').strip()
      match = re.search(r'.*('+ssid+').*', result)
      return (match is not None)
      
    def getAPmac(self, device:str):
      logger.debug("Scan for the MAC: 'iwconfig %s'", device)
      result = subprocess.run(['iwconfig', device], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL).stdout.decode('utf-8').strip()
      #Access Point: F6:DD:9E:87:A1:63
      match = re.search(r'.*Access Point: (..:..:..:..:..:..).*', result)
      if match is None:
        return None
      else:
        return match.group(1).replace(':', '')
      
    def _connect(self, device:str, ssid:str, passwd:str=None):
        '''
            activate device:            ifconfig <device> up
            check existing networks:    wpa_cli -i <device> list_networks
            connect to network:         wpa_cli -i <device> select_network <network_id>
            configure new network:      wpa_cli -i <device> reconfigure
        '''
        # make sure the device is up
        self.__checkDeviceState(device)

        # stop running wpa_supplicant instance
        subprocess.run(['wpa_cli', '-i', device, 'terminate'], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

        wpa_file = "/tmp/wpa_gopro.conf"
        logger.debug("Create a new wifi network profile in '%s'", wpa_file)
        try:
            f = open(wpa_file, "w")
            f.writelines([
                'ctrl_interface=/run/wpa_supplicant\n'
                'update_config=1\n\n'
                'network={\n',
                '\tssid="' + str(ssid) + '"\n',
                '\tpsk="' + str(passwd) + '"\n',
                '}\n'
            ])
            #f.writelines(['network={\n', 'ssid="' + ssid + '"\n', 'psk="' + passwd + '"\n', '}\n'])
            f.close()

            # (re-)start the wpa_supplicant deamon
            logger.debug("Start wpa_supplicant with config: '%s'", wpa_file)
            subprocess.run(['wpa_supplicant', '-B', '-i', device, '-c', wpa_file], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

            # wait max. 10s or until connected
            it = 0
            while (self.getCurrentSSID(device) is None or self.getCurrentSSID(device) != ssid) and it < 10 and not self._cancel.is_set():
                time.sleep(1)
                it += 1
            # check if connection was successful and return
            if self.getCurrentSSID(device) == ssid:
                return ssid
        except:
            logger.critical("Error creating wifi network config file ({})!".format(wpa_file))

        return None

    def __checkDeviceState(self, device):
        logger.debug("Bringing device up: 'ifconfig %s up'", device)
        subprocess.run(['ifconfig', device, 'up'], stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
