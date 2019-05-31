import logging
import time
import os
import traceback

from AgentController import AgentController


def wait_for(condition:callable, sleeper:float, min_time:float=0.0, max_time:float=3600.0, *kwargs):
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

def wait_for_cmd(agent:AgentController, cmd_id:int, sleeper:float=0.1):
    """
    Waits for a command to be executed (and returned) by an agent instance. The command to wait for is identified by its
    id. Between two checks, if the command has returned, it is waiting :sleeper: seconds (or fractional). The agent
    instance must be the one, where the command was scheduled!

    :param agent:   the agent instance, where the command was scheduled and for which the result should be waiting for
    :param cmd_id:  the scheduled command id
    :param sleeper: how long should be waited between to checks, if the command returned
    :return:        the result of the command
    """
    data = agent.command_result(cmd_id)
    while data is None:
        time.sleep(sleeper)
        data = agent.command_result(cmd_id)
    return data

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
