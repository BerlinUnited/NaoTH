import time

from utils.Bluetooth import Bluetooth

class CheckBluetooth:
    def __init__(self, mac):

        print('Set beep ON for', mac)

        try:
            bt = Bluetooth(mac, True)
            bt.setBeepOn().start()
            bt.join()

            print('Beep for 3 seconds ...')
            time.sleep(3)

            print('Set beep OFF for', mac)

            bt = Bluetooth(mac)
            bt.setBeepOff().start()
            bt.join()
        except KeyboardInterrupt:
            # skip error message for keyboard interrupts
            pass

        print('Done')