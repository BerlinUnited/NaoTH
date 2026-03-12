
from naoth.log import Reader as LogReader
from naoth.log import BehaviorParser

if __name__ == "__main__":
    
    # adjust the path to your logfile
    game_log = "./game.log"

    # specalized behavior parser
    parser = None
    
    # representation containinf information about the behavior
    # such as input_symbols, output_symbols etc.
    behavior_frame = None
    
    with LogReader(game_log) as reader:
    
        for frame in reader.read():

            # 1. initialize the behavior parser with the complete behavior information
            # this should happen only once in the first frame
            if "BehaviorStateComplete" in frame:
                print(frame.number)
                parser = BehaviorParser(frame["BehaviorStateComplete"])
            
            # 2. parse the behavior frame
            if parser is not None and "BehaviorStateSparse" in frame:
                behavior_frame = parser.parse(frame["BehaviorStateSparse"])
                
            
            # 3. use the behavior_frame to read the input symbols
            if behavior_frame is not None:
                
                inputs = behavior_frame.input_symbols
                
                bm_x = inputs["ball.x"]
                bm_y = inputs["ball.y"]
                
                print((bm_x, bm_y))