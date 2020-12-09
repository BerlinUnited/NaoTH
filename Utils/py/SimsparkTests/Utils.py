import logging
import time
import os
import traceback

__all__ = ['wait_for', 'remove_simspark_logfile', 'TestRun']


def wait_for(condition: callable, sleeper: float, min_time: float = 0.0, max_time: float = 3600.0, *kwargs):
    """
    Waits as long as the condition doesn't return True. The condition is a callable, which returns True or False.
    Between two condition tests, it waits :sleeper: seconds. If the condition is True, it waits at least :min_time:
    seconds. Additional parameters for the condition callable are applied via :kwargs:.

    :param condition:   the callable, which should return True or False
    :param sleeper:     the time in seconds (or fractional seconds) between two condition checks
    :param min_time:    the min. time in seconds (or fractional seconds), which should be waited at least
    :param max_time:    the max. time in seconds (or fractional seconds), which should be waited at maximum; by default 3600sec = 1 hour
    :param kwargs:      additional arguments for the callable condition
    :return:            None
    """
    _start = time.monotonic()
    try:
        while (not condition(*kwargs) or (time.monotonic() - _start) < min_time) and (time.monotonic() - _start) < max_time:
            time.sleep(sleeper)
    except Exception as e:
        logging.error("%s\n%s", e, traceback.format_exc(limit=2))


def remove_simspark_logfile():
    """
    Removes the simspark log file 'sparkmonitor.log', if one is created by a test run.

    :return:    None
    """
    if os.path.isfile('sparkmonitor.log'):
        try:
            os.remove('sparkmonitor.log')
        except:
            logging.warning("Could not remove simspark log file!")


class TestRun:

    def __init__(self):
        self.logger = logging.getLogger(self.__class__.__name__)

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def run(self):
        tests = 0
        successful = 0

        for attr in dir(self):
            if not attr.startswith('test'):
                continue
            testFunc = getattr(self, attr)
            if not callable(testFunc):
                continue

            print(attr, '... ', end='', flush=True)
            tests += 1
            if testFunc():
                successful += 1
                print('ok', flush=True)
            else:
                print('FAIL', flush=True)

        return (tests, successful)