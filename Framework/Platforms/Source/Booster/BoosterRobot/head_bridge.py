
import os
import time

import socket
import msgpack

from booster_robotics_sdk_python import (
    ChannelFactory,
    B1LocoClient,
    RobotMode
)


'''
info = client.GetRobotInfo()
                print(f"Name    : {info.name}")
                print(f"Nickname: {info.nickname}")
                print(f"Version : {info.version}")
                print(f"Model   : {info.model}")
                
client.GetUp()
client.ChangeMode(b1.RobotMode.kWalking)
client.ChangeMode(b1.RobotMode.kDamping)
client.GetUpWithMode(mode)
client.Move(x, y, yaw)

client.ResetOdometry()

BatteryState
BodyControl
B1OdometerStateSubscriber
B1BatteryStateSubscriber
Orientation
'''

class RosUnixCommandServer:
    def __init__(self, sock_path):
        self.sock_path = sock_path

        # Create listening socket
        self.srv = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)

        # Remove stale socket file
        try:
            os.unlink(self.sock_path)
        except FileNotFoundError:
            pass

        self.srv.bind(self.sock_path)
        self.srv.listen(1)

        print(f"Unix socket server listening at {self.sock_path}")
        print("Waiting for C++ client to connect...")

        self.conn, _ = self.srv.accept()
        print("Client connected.")

        # msgpack streaming unpacker
        self.unpacker = msgpack.Unpacker(raw=False)
        
        
        #ChannelFactory.Instance().Init(0)
        print("Init ChannelFactory ...")
        ChannelFactory.Instance().Init(0, "127.0.0.1")
        time.sleep(2)
        
        print("Init B1LocoClient ...")
        self.body_client = B1LocoClient()
        self.body_client.Init()
        time.sleep(3)
        

    def poll(self):
        """
        Receive data and handle commands if available.
        Call this periodically.
        """
        data = self.conn.recv(4096)
        if not data:
            print("Client disconnected.")
            return False

        self.unpacker.feed(data)

        for msg in self.unpacker:
            self.handle_message(msg)

        return True

    def handle_message(self, msg):
        """
        Handle incoming msgpack map.
        Example:
        {"cmd":"setHead","yaw":0.1,"pitch":-0.2}
        """
        cmd = msg.get("cmd")

        if cmd == "setHead":
            yaw = msg.get("yaw", 0.0)
            pitch = msg.get("pitch", 0.0)
            self.handle_set_head(yaw, pitch)

        else:
            print("Unknown command:", msg)

    def handle_set_head(self, yaw, pitch):
        print(f"[CMD] setHead yaw={yaw:.3f} pitch={pitch:.3f}")
        # put robot control code here
        self.body_client.RotateHead(pitch, yaw)


    def close(self):
        try:
            self.conn.close()
        except Exception:
            pass

        self.srv.close()

        try:
            os.unlink(self.sock_path)
        except FileNotFoundError:
            pass

def main():
    server = RosUnixCommandServer("/tmp/naoth_body")

    try:
        while True:
            if not server.poll():
                break
    finally:
        server.close()
    


if __name__ == "__main__":
    main()