import subprocess
import logging
import shutil
import re
import time

class NetworkManager:
    def __init__(self):
        pass

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

    def getCurrentSSID(self, device: str):
        ''' Checks if the given device is currently connected and returns the SSID of the connected network otherwise returns None.

        :param device:  name of the device from which we want to get the current SSID
        :return:        the current SSID of the device
        '''

        return None

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

    def getCurrentSSID(self, device:str):
        '''
            checking connection:        nmcli -g GENERAL.STATE device show <device>
            determine network:          nmcli -g GENERAL.CONNECTION device show <device>
        '''
        logger.debug("Check device state: 'nmcli -g GENERAL.STATE device show %s'", device)
        state = subprocess.run(['nmcli', '-g', 'GENERAL.STATE', 'device', 'show', device], stdout=subprocess.PIPE).stdout.decode('utf-8').strip()
        # do we have an established connection
        if state.split()[0] == '100':
            logger.debug("Get SSID of current connection: 'nmcli -g GENERAL.CONNECTION device show %s'", device)
            # return SSID of current connection
            return subprocess.run(['nmcli', '-g', 'GENERAL.CONNECTION', 'device', 'show', device], stdout=subprocess.PIPE).stdout.decode('utf-8').strip()
        return None

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

    def getCurrentSSID(self, device:str):
        '''
            determine current network:  iwgetid <device> -r
        '''

        logger.debug("Get current wifi SSID: 'iwgetid %s -r'", device)
        result = subprocess.run(['iwgetid', device, '-r'], stdout=subprocess.PIPE)
        # do we have an established connection
        if result.returncode == 0:
            # return SSID of current connection
            return result.stdout.decode('utf-8').strip()

        return None

    def getSSIDExists(self, device:str, ssid:str):
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
        logger.debug("Bringing device up: 'ifconfig %s up'", device)
        subprocess.run(['ifconfig', device, 'up'], stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)

        # get all configured wifi networks
        network_id = -1
        logger.debug("Check if wifi network profile already exists: 'wpa_cli -i %s list_networks'", device)
        result = subprocess.run(['wpa_cli', '-i', device, 'list_networks'], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL).stdout.decode('utf-8').strip()
        # ... and find network ID of configured wpa network, which is the given one
        for networks in result.split('\n'):
            match = re.search('(\d+)\s+(\w+)\s+(\w+)', networks.strip())
            if match and match.group(2) == ssid:
                network_id = int(match.group(1))

        # exists a saved wifi network with the given SSID?
        if network_id >= 0:
            logger.debug("Connect to existing wifi network profile: 'wpa_cli -i %s select_network %d'", device, network_id)
            # try to activate the saved wifi network with the given SSID
            subprocess.run(['wpa_cli', '-i', device, 'select_network', str(network_id)], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            # wait max. 10s or until connected
            it = 0
            while (self.getCurrentSSID(device) is None or self.getCurrentSSID(device) != ssid) and it < 10:
                time.sleep(1)
                it += 1
            # check if connecting was successful
            if ssid == self.getCurrentSSID(device):
                return ssid
            logger.error("Couldn't connect to network '%s' with device '%s'!", ssid, device)
        # ... else create new connection - if password given
        elif passwd is not None:
            logger.debug("Create a new wifi network profile in '/etc/wpa_supplicant/wpa_supplicant.conf'")
            try:
                f = open("/etc/wpa_supplicant/wpa_supplicant.conf", "a")
                f.writelines(['network={\n', 'ssid="'+ssid+'"\n', 'psk="'+passwd+'"\n','}\n'])
                f.close()
                logger.debug("Update network config: 'wpa_cli -i %s reconfigure'", device)
                subprocess.run(['wpa_cli', '-i', device, 'reconfigure'], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
                # don't provide passwd, otherwise we could write 'new' config multiple time!
                return self._connect(device,ssid)
            except:
                logger.critical("Root permission needed to add wifi network!")
        else:
            logger.error("Could not connect to %s, password required!", ssid)
        return None


logger = logging.getLogger("Network")
manager = NetworkManagerNmcli() if shutil.which('nmcli') is not None else NetworkManagerIw()

# make object methods 'public'
getWifiDevices = manager.getWifiDevices
getWifiDevice  = manager.getWifiDevice
getCurrentSSID = manager.getCurrentSSID
connectToSSID  = manager.connectToSSID
getSSIDExists  = manager.getSSIDExists
getAPmac  = manager.getAPmac
