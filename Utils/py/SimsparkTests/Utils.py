import logging
import time
import os


def wait_for(condition, sleeper, min_time=0.0, *kwargs):
    _start = time.monotonic()
    try:
        while not condition(*kwargs) or (time.monotonic() - _start) < min_time:
            time.sleep(sleeper)
    except Exception as e:
        logging.error(e)

def wait_for_cmd(agent, cmd_id:int, sleeper=0.1):
    data = agent.command_result(cmd_id)
    while data is None:
        time.sleep(sleeper)
        data = agent.command_result(cmd_id)
    return data

def remove_simspark_logfile():
    if os.path.isfile('sparkmonitor.log'):
        try:
            os.remove('sparkmonitor.log')
        except:
            logging.warning("Could not remove simspark log file!")
