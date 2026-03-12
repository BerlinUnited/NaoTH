
import os

from naoth.log import Reader as LogReader

if __name__ == "__main__":
    
    game_log = "tmp/game.log"
    
    with LogReader(game_log) as reader:
        for frame in reader.read():
            print(frame.number)
            print(frame.get_names())
            
            # only print the first frame
            break
            
                
                