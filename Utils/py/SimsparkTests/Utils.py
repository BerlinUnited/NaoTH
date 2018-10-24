import time


def wait_for(condition, sleeper, min_time=0.0):
    _start = time.monotonic()
    while not condition() or (time.monotonic() - _start) < min_time:
        time.sleep(sleeper)

def wait_for_cmd(agent, cmd_id:int, sleeper=0.1):
    data = agent.command_result(cmd_id)
    while data is None:
        time.sleep(sleeper)
        data = agent.command_result(cmd_id)
    return data
