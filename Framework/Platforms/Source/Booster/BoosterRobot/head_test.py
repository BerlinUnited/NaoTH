
import time

from booster_robotics_sdk_python import (
    ChannelFactory,
    B1LocoClient,
    RobotMode
)

def main():
    #ChannelFactory.Instance().Init(0)
    print("Init ChannelFactory ...")
    ChannelFactory.Instance().Init(0, "127.0.0.1")
    time.sleep(2)
    
    print("Init B1LocoClient ...")
    client = B1LocoClient()
    client.Init()
    time.sleep(3)
    
    
    while True:
        head_yaw, head_pitch = -0.785, 0.0
        client.RotateHead(head_pitch, head_yaw)
        time.sleep(3)
        head_yaw, head_pitch = -0.785, 0.0
        client.RotateHead(head_pitch, head_yaw)
        time.sleep(3)
    
    


if __name__ == "__main__":
    main()