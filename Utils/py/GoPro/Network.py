import subprocess
import logging

logger = logging.getLogger("Network")

def getWifiDevices():
    ''' Retrieves all wifi devices.
        used command: nmcli -t device

    :return:        a list of available wifi devices
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

def getWifiDevice(device:str=None):
    ''' Checks whether the given wifi device is available or returns the available wifi device if nothing given.
        There had to be only one wifi device if nothing was given. Otherwise an error is printed out.

    :param device:  the device, which should be checked
    :return:        a wifi device or None
    '''
    if device is None:
        devices = getWifiDevices()
        if len(devices) > 1:
            logger.error("Multiple devices available!")
        elif len(devices) == 0:
            logger.error("No device available!")
        else:
            return devices[0]
    elif device not in getWifiDevices():
        logger.error("Device '%s' is not available!", device)
    else:
        return device
    return None

def getCurrentSSID(device:str):
    ''' Checks if the given device is currently connected and returns the SSID of the connected network otherwise returns None.
        command for
            > checking connection: nmcli -g GENERAL.STATE device show <device>
            > determine network: nmcli -g GENERAL.CONNECTION device show <device>

    :param device:  name of the device from which we want to get the current SSID
    :return:        the current SSID of the device
    '''

    logger.debug("Check device state: 'nmcli -g GENERAL.STATE device show %s'", device)
    state = subprocess.run(['nmcli', '-g', 'GENERAL.STATE', 'device', 'show', device], stdout=subprocess.PIPE).stdout.decode('utf-8').strip()
    # do we have an established connection
    if state.split()[0] == '100':
        logger.debug("Get SSID of current connection: 'nmcli -g GENERAL.CONNECTION device show %s'", device)
        # return SSID of current connection
        return subprocess.run(['nmcli', '-g', 'GENERAL.CONNECTION', 'device', 'show', device], stdout=subprocess.PIPE).stdout.decode('utf-8').strip()
    return None

def connectToSSID(device:str, ssid:str, passwd:str=None):
    ''' Connects the given device to the given SSID with the provided password.
        used commands:
            > nmcli connection show <ssid>
            > nmcli connection up <ssid> ifname <device>
            > nmcli device wifi connect <ssid> password <passwd> ifname <device>

    :param device:  name of the device we're like to connect with
    :param ssid:    SSID of the network we're like to connect with
    :param passwd:  the password of the SSID we're like to connect with
    :return:        the SSID of the connected network or 'None' if an error occurred
    '''

    if getCurrentSSID(device) != ssid:
        logger.debug("Check if wifi network profile already exists: 'nmcli connection show %s'", ssid)
        # exists a saved wifi network with the given SSID?
        if subprocess.run(['nmcli', 'connection', 'show', ssid], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL).returncode == 0:
            logger.debug("Connect to existing wifi network profile: 'nmcli connection up %s ifname %s'", ssid, device)
            # try to activate the saved wifi network with the given SSID
            r = subprocess.run(['nmcli', 'connection', 'up', ssid, 'ifname', device], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            if r.returncode != 0:
                logger.error("Couldn't connect to network '%s' with device '%s'!\n\twhat: %s", ssid, device, str(r.stderr,'utf-8').strip())
            else:
                return ssid
        # ... else create new connection - if password given
        elif passwd is not None:
            logger.debug("Create a new wifi network profile and connect: 'nmcli device wifi connect %s password %s ifname %s'", ssid, passwd, device)
            r = subprocess.run(['nmcli', 'device', 'wifi', 'connect', ssid, 'password', passwd, 'ifname', device])
            if r.returncode != 0:
                logger.error("Couldn't connect to network '%s' with device '%s'; wrong credentials?\n\twhat: %s", ssid, device, str(r.stderr,'utf-8').strip())
            else:
                return ssid
        else:
            logger.error("Could not connect to %s, password required!", ssid)
    else:
        return ssid

    return None
