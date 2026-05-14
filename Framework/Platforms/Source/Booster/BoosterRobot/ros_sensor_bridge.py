
import os
import time

import socket
import msgpack
import struct

''' ros topics

/robot_description

/odometer_state
/motion_state

/low_state
/light_control
/light_status

/send_imu

/fall_down
/fall_down_recovery_state

/tf
/tf_static
/joint_states
/head_pose
/head_pose_stamped

/booster/ros2_k2_imu
/booster/ros2_k2_joint_cmd
/booster/ros2_k2_joint_states

'''



import rclpy
from rclpy.node import Node

from message_filters import Subscriber, TimeSynchronizer, ApproximateTimeSynchronizer
from rclpy.qos import QoSProfile, QoSReliabilityPolicy, QoSHistoryPolicy, QoSDurabilityPolicy

from sensor_msgs.msg import Image
from geometry_msgs.msg import PoseStamped


class BoosterROS(Node):
    
    def __init__(self, server):
        super().__init__('naoth_sensor_bridge')
        
        qos_sensors = QoSProfile(
            history     = QoSHistoryPolicy.KEEP_LAST,
            depth       = 20,  # logger can lag briefly without losing frames (tune: ~0.3–1s worth)
            reliability = QoSReliabilityPolicy.RELIABLE,
            durability  = QoSDurabilityPolicy.VOLATILE
        )
        
        self.sub_head_pose = Subscriber(self, PoseStamped, '/head_pose_stamped', qos_profile = qos_sensors)

        self.sync = ApproximateTimeSynchronizer(
            [self.sub_head_pose],
            queue_size = 30,
            slop = 0.01  # 10 ms
        )

        self.sync.registerCallback(self.sensor_callback)
        
        
        self.server = server
        
    def pose_stamped_to_msgpack(self, msg: PoseStamped):
        ts_ns = (int(msg.header.stamp.sec) * 1_000_000_000) + int(msg.header.stamp.nanosec)

        data = {
            "headPose": {
                "position": {
                    "x": float(msg.pose.position.x),
                    "y": float(msg.pose.position.y),
                    "z": float(msg.pose.position.z),
                },
                "orientation": {
                    "x": float(msg.pose.orientation.x),
                    "y": float(msg.pose.orientation.y),
                    "z": float(msg.pose.orientation.z),
                    "w": float(msg.pose.orientation.w),
                },
            },
            "timestamp": ts_ns,
        }

        return msgpack.packb(data, use_bin_type=True)
        
        
    def sensor_callback(self, head_pose: PoseStamped):
        #print("send head_pose")
        headPose_msgpack = self.pose_stamped_to_msgpack(head_pose)
    
        # prefix with 4-byte length
        packet = struct.pack("!I", len(headPose_msgpack)) + headPose_msgpack
        self.server.send_data(packet)
        
    

class UnixSocketConnector:
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
        
        
    def recv_exact(self, n):
        chunks = []
        total = 0
        while total < n:
            chunk = self.conn.recv(n - total)
            if not chunk:
                return None
            chunks.append(chunk)
            total += len(chunk)
        return b"".join(chunks)
        

    def receive_actuators(self):
        
        header = self.recv_exact(4)
        if header is None:
            print("Client disconnected.")
            return False
        
        # expected message length
        msg_len = struct.unpack("!I", header)[0]
        
        # receive the actual data
        data = self.recv_exact(msg_len)
        
        if data is None:
            return None
         
        return msgpack.unpackb(data, raw=False)

    def send_data(self, data):
        self.conn.sendall(data)

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


'''
actuator_data = {
    "headRotation": {
        "yaw": 0.3,
        "pitch": -0.1,
    },
    "walkVelocity": {
        "x": 1.0,
        "y": 0.0,
        "rotation": 0.2,
    },
}

sensor_data = {
    "headPose": {
        "position": {
            "x": 1.0,
            "y": 2.0,
            "z": 3.0,
        },
        "orientation": {
            "x": 0.0,
            "y": 0.0,
            "z": 0.0,
            "w": 1.0,
        },
    },
}

packed = msgpack.packb(actuator_data, use_bin_type=True)
unpacked = msgpack.unpackb(packed, raw=False)
'''


def main():
    
    server = UnixSocketConnector("/tmp/naoth_sensors")

    rclpy.init()

    sensors = BoosterROS(server)

    try:
        rclpy.spin(sensors)
    finally:
        rclpy.shutdown()
    


if __name__ == "__main__":
    main()