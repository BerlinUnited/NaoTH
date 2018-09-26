import threading

from utils import GameController, blackboard


class CheckGameController:
    def __init__(self):
        gameController = GameController()
        gameController.start()

        packet = -1
        while True:
            try:
                gc = blackboard['gamecontroller']
                if gc is not None and packet != gc.packetNumber:
                    print(gc)
                    # check if one team is 'invisible'
                    if any([t.teamNumber == 0 for t in gc.team]):
                        print("-- INVISIBLES are playing! --\n")
                    print('Team left:', gc.team[0].teamNumber, 'Team right:', gc.team[1].teamNumber)
                    packet = gc.packetNumber
            except (KeyboardInterrupt, SystemExit):
                break

        gameController.cancel()
        gameController.join()