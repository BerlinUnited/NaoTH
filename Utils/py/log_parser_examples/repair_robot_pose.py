
import os, math

from naoth.log import Reader as LogReader
from naoth.log import BehaviorParser

from naoth.pb.RobotPose_pb2 import RobotPose


if __name__ == "__main__":
    
    # input log
    game_log = "tmp/game.log"
    
    # generate the name for the new log file
    game_log_out = f"{game_log}.robot_pose.log"

    behavior_parser = None

    with LogReader(game_log) as reader, open(game_log_out, 'wb') as output:
        for frame in reader.read():
            #print(frame.get_names())
            
            # quit if the robot pose already exists
            if "RobotPose" in frame:
                print("RobotPose already exists. Exit.")
                break
            
            # empty msg
            msg = RobotPose()
            
            # initialize behavior parser
            if "BehaviorStateComplete" in frame:               
                print(frame.number)
                behavior_parser = BehaviorParser(frame["BehaviorStateComplete"])
                
                msg.isValid = False
                msg.pose.translation.x = 0
                msg.pose.translation.y = 0
                msg.pose.rotation = 0

            # update behavior parser and export robot pose from behavior symbols
            elif behavior_parser is not None and "BehaviorStateSparse" in frame:
            
                behavior_frame = behavior_parser.parse(frame["BehaviorStateSparse"])
                inputs = behavior_frame.input_symbols
                
                # RobotPose (from the input symbols)
                msg.isValid = inputs["robot_pose.is_valid"]
                msg.pose.translation.x = inputs["robot_pose.x"]
                msg.pose.translation.y = inputs["robot_pose.y"]
                msg.pose.rotation = inputs["robot_pose.rotation"] / 180.0 * math.pi
                
            else:
                print("error")
                quit()
                
            # add the RobotPose to the frame and write it to the new file
            frame.add_field("RobotPose", msg)
            output.write(bytes(frame))
            
            
                
                