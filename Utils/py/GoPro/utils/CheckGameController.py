import threading

from utils import Event, GameController


class CheckGameController:
    def __init__(self, loopControl:threading.Event):
        Event.registerListener(self)

        gameController = GameController()
        gameController.start()

        try:
            loopControl.wait()
        except (KeyboardInterrupt, SystemExit):
            loopControl.set()

        gameController.cancel()
        gameController.join()


    def receivedGC(self, evt:Event.GameControllerMessage):
        """ Is called, when a new GameController message was received. """
        gc_data = evt.message
        print(gc_data)
        # check if one team is 'invisible'
        if any([t.teamNumber == 0 for t in gc_data.team]):
            print("-- INVISIBLES are playing! --\n")
        print('Team left:', gc_data.team[0].teamNumber, 'Team right:', gc_data.team[1].teamNumber)
