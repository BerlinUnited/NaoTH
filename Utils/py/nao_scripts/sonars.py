import time
import signal
import threading
import argparse
import naoqi


def parseArguments():
	parser = argparse.ArgumentParser(description="Prints out all sonar sensors read from ALMemory.\n"
												 "For this to work, at least 'pythonbridge' and 'sensors' has to be active in the '/etc/naoqi/autoload.ini'!\n"
												 "(restart naoqi after changing the 'autoload.ini': nao restart)\n\n"
												 "See also:\n"
												 "http://doc.aldebaran.com/2-4/family/robots/sonar_robot.html\n"
												 "http://doc.aldebaran.com/2-4/family/nao_dcm/actuator_sensor_names.html",
									 formatter_class=argparse.RawDescriptionHelpFormatter)
	parser.add_argument('-i','--interval', type=float, default=1, action='store', help="The interval, the sensors should be read.")
	return parser.parse_args()

def cancel_signal(signum, frame):
	canceled.set()

if __name__ == '__main__':
	args = parseArguments()
	print 'Starting sonar test'

	canceled = threading.Event()
	signal.signal(signal.SIGINT, cancel_signal)

	m = naoqi.ALProxy("ALMemory","localhost", 9559)
	s = naoqi.ALProxy("ALSonar", "localhost", 9559)

	# TODO: setting the sonar mode!
	#Device/SubDeviceList/US/Actuator/Value
	print 'Sonar mode:',  m.getData("Device/SubDeviceList/US/Actuator/Value")

	# NOTE: isn't necessary, if we just read from memory
	#s.subscribe("SonsarTest")

	fields = ['T','L','R','L1','L2','L3','L4','L5','L6','L7','L8','L9','R1','R2','R3','R4','R5','R6','R7','R8','R9']
	format_str = ''
	for f in fields:
		format_str += ' {%s:>7.5f}'%(f)

	i = 0
	while not canceled.is_set():
		if i == 0:
			print ''.join([' {:7}' for _ in fields]).format(*fields)

		data = {
			'T': m.getData("Device/SubDeviceList/US/Sensor/Value"),
			'L': m.getData("Device/SubDeviceList/US/Left/Sensor/Value"),
			'R': m.getData("Device/SubDeviceList/US/Right/Sensor/Value"),
			'L1': m.getData("Device/SubDeviceList/US/Left/Sensor/Value1"),
			'L2': m.getData("Device/SubDeviceList/US/Left/Sensor/Value2"),
			'L3': m.getData("Device/SubDeviceList/US/Left/Sensor/Value3"),
			'L4': m.getData("Device/SubDeviceList/US/Left/Sensor/Value4"),
			'L5': m.getData("Device/SubDeviceList/US/Left/Sensor/Value5"),
			'L6': m.getData("Device/SubDeviceList/US/Left/Sensor/Value6"),
			'L7': m.getData("Device/SubDeviceList/US/Left/Sensor/Value7"),
			'L8': m.getData("Device/SubDeviceList/US/Left/Sensor/Value8"),
			'L9': m.getData("Device/SubDeviceList/US/Left/Sensor/Value9"),
			'R1': m.getData("Device/SubDeviceList/US/Right/Sensor/Value1"),
			'R2': m.getData("Device/SubDeviceList/US/Right/Sensor/Value2"),
			'R3': m.getData("Device/SubDeviceList/US/Right/Sensor/Value3"),
			'R4': m.getData("Device/SubDeviceList/US/Right/Sensor/Value4"),
			'R5': m.getData("Device/SubDeviceList/US/Right/Sensor/Value5"),
			'R6': m.getData("Device/SubDeviceList/US/Right/Sensor/Value6"),
			'R7': m.getData("Device/SubDeviceList/US/Right/Sensor/Value7"),
			'R8': m.getData("Device/SubDeviceList/US/Right/Sensor/Value8"),
			'R9': m.getData("Device/SubDeviceList/US/Right/Sensor/Value9"),
		}

		print format_str.format(**data)

		time.sleep(args.interval)

		i += 1
		if i >= 50:
			i = 0

	# TODO: ALExtractor::received unsubscribe from non-subscribed module???
	#s.unsubscribe("SonarTest")

	print 'Finish'
