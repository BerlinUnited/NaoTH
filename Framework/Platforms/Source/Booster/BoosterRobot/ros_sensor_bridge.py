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


'''
    ---
    header:
      stamp:
        sec: 1772837540
        nanosec: 84387503
      frame_id: ''
    name:
    - AAHead_yaw
    - Head_pitch
    - Left_Shoulder_Pitch
    - Left_Shoulder_Roll
    - Left_Elbow_Pitch
    - Left_Elbow_Yaw
    - Right_Shoulder_Pitch
    - Right_Shoulder_Roll
    - Right_Elbow_Pitch
    - Right_Elbow_Yaw
    - Left_Hip_Pitch
    - Left_Hip_Roll
    - Left_Hip_Yaw
    - Left_Knee_Pitch
    - Left_Ankle_Pitch
    - Left_Ankle_Roll
    - Right_Hip_Pitch
    - Right_Hip_Roll
    - Right_Hip_Yaw
    - Right_Knee_Pitch
    - Right_Ankle_Pitch
    - Right_Ankle_Roll
    position:
    - -0.00062831852119416
    - 0.00062831852119416
    - 0.2005741149187088
    - -1.4903078079223633
    - 0.018791837617754936
    - -0.4943403899669647
    - 0.1783307045698166
    - 1.4385343790054321
    - -0.021859893575310707
    - 0.4905053377151489
    - -0.07000076025724411
    - -0.005531395319849253
    - 0.001716639962978661
    - 0.12722209095954895
    - -0.0883404133444756
    - 0.008991102336957369
    - -0.048256658017635345
    - 0.023079270496964455
    - 0.0036240178160369396
    - 0.12836651504039764
    - -0.10622044787208565
    - -0.022112139430561274
    velocity:
    - 0.00033918992266990244
    - 0.00017567025497555733
    - -0.020887300372123718
    - 0.014836282469332218
    - -0.018265802413225174
    - 0.007574477698653936
    - 0.0035524507984519005
    - -0.017125658690929413
    - -0.007214345969259739
    - 0.01358166802674532
    - -0.015777533873915672
    - -0.004827990662306547
    - 0.0006536308210343122
    - -0.0009340108372271061
    - 0.007974776785130543
    - 0.00831746228398721
    - 0.007084412965923548
    - -0.008996977470815182
    - -0.0019092722795903683
    - 0.004273134749382734
    - -0.000949768458660183
    - 0.012500550161382282
    effort:
    - -0.01565999910235405
    - 0.025280000641942024
    - 0.023499252274632454
    - 0.8929715752601624
    - -0.20123904943466187
    - -0.060670796781778336
    - 0.23157444596290588
    - -0.3789788484573364
    - 0.22901089489459991
    - 0.10553300380706787
    - 3.002124547958374
    - -0.560879111289978
    - -0.10351648926734924
    - 0.029084250330924988
    - 1.8633295410825017
    - -0.14153413969522682
    - 0.4887179434299469
    - -1.308717966079712
    - -0.6786080598831177
    - -0.2617582678794861
    - 1.8821528342970661
    - 0.05553931938094653
    ---
    '''




import rclpy
from rclpy.node import Node

from message_filters import Subscriber, TimeSynchronizer, ApproximateTimeSynchronizer
from rclpy.qos import QoSProfile, QoSReliabilityPolicy, QoSHistoryPolicy, QoSDurabilityPolicy


from sensor_msgs.msg import Image, JointState
from geometry_msgs.msg import PoseStamped

from booster_interface.msg import LowState, Odometer


# ros2 topic type /odometer_state
#from booster_interface.msg import Odometer


class BoosterROS(Node):

    def __init__(self, server):
        super().__init__('naoth_sensor_bridge')

        qos_sensors = QoSProfile(
            history     = QoSHistoryPolicy.KEEP_LAST,
            depth       = 20,  # logger can lag briefly without losing frames (tune: ~0.3–1s worth)
            reliability = QoSReliabilityPolicy.RELIABLE,
            durability  = QoSDurabilityPolicy.VOLATILE
        )

        self.sub_head_pose      = Subscriber(self, PoseStamped  , '/head_pose_stamped', qos_profile = qos_sensors)
        #self.sub_joint_states   = Subscriber(self, JointState   , '/joint_states'     , qos_profile = qos_sensors)
        self.sub_low_state      = Subscriber(self, LowState     , '/low_state'        , qos_profile = qos_sensors)
        self.sub_odometry       = Subscriber(self, Odometer     , '/odometer_state'   , qos_profile = qos_sensors)

        #self.sub_head_pose = Subscriber(self, PoseStamped, '/robot_state', qos_profile = qos_sensors)
        #self.sub_head_pose = Subscriber(self, PoseStamped, '/robot_states', qos_profile = qos_sensors)
        #self.sub_head_pose = Subscriber(self, PoseStamped, '/battery_state', qos_profile = qos_sensors)

        self.sync = ApproximateTimeSynchronizer(
            [self.sub_head_pose, self.sub_low_state, self.sub_odometry],
            queue_size = 30,
            slop = 0.004,  # 10 ms
            allow_headerless=True,
        )

        self.sync.registerCallback(self.sensor_callback)

        # we wil be processing messages at a fixed frame rate
        self.timer = self.create_timer(0.01, self.process)  # 100 Hz
        self.latest_data = None

        self.server = server


    def process(self):
        if self.latest_data is None:
            print("[ROS_SENSOR_BRIDGE] no new data, skip.")
            return

        # unpack
        head_pose, odometer_state, low_state = self.latest_data

        # clear data
        self.latest_data = None

        sensor_data = {
            "head_pose"     : self.pose_stamped_to_dict(head_pose),
            #"joint_state"   : self.joints_to_dict(joint_state),
            "odometer_state": self.odometry_to_dict(odometer_state),
            "low_state"     : self.low_state_to_dict(low_state),
        }

        self.server.send_dict_as_msgpack(sensor_data)


    def sensor_callback(self, head_pose: PoseStamped, low_state: LowState, odometer_state: Odometer): #joint_state: JointState
        self.latest_data = (head_pose, odometer_state, low_state)


    # a generic conversion function
    def rosmsg_to_dict(self, msg):
        if hasattr(msg, "get_fields_and_field_types"):
            result = {}
            for field in msg.get_fields_and_field_types():
                value = getattr(msg, field)
                result[field] = self.rosmsg_to_dict(value)
            return result
        elif isinstance(msg, (list, tuple)):
            return [self.rosmsg_to_dict(v) for v in msg]
        else:
            return msg


    def pose_stamped_to_dict(self, msg: PoseStamped):
        ts_ns = (int(msg.header.stamp.sec) * 1_000_000_000) + int(msg.header.stamp.nanosec)

        data = {
            "timestamp": ts_ns,
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
        }

        return data

    NAOTH_JOINT_ORDER = [
        "AAHead_yaw",
        "Head_pitch",
        "Left_Shoulder_Pitch",
        "Left_Shoulder_Roll",
        "Left_Elbow_Pitch",
        "Left_Elbow_Yaw",
        "Right_Shoulder_Pitch",
        "Right_Shoulder_Roll",
        "Right_Elbow_Pitch",
        "Right_Elbow_Yaw",
        "Left_Hip_Pitch",
        "Left_Hip_Roll",
        "Left_Hip_Yaw",
        "Left_Knee_Pitch",
        "Left_Ankle_Pitch",
        "Left_Ankle_Roll",
        "Right_Hip_Pitch",
        "Right_Hip_Roll",
        "Right_Hip_Yaw",
        "Right_Knee_Pitch",
        "Right_Ankle_Pitch",
        "Right_Ankle_Roll"
    ]

    def joints_to_dict(self, msg):
        ts_ns = (int(msg.header.stamp.sec) * 1_000_000_000) + int(msg.header.stamp.nanosec)

        # transpose the separate lists into a map
        source_index = {name: i for i, name in enumerate(msg.name)}

        data = {
            "timestamp" : ts_ns,
            "name"      : BoosterROS.NAOTH_JOINT_ORDER,
            "position"  : [msg.position[source_index[name]] for name in BoosterROS.NAOTH_JOINT_ORDER],
            "velocity"  : [msg.velocity[source_index[name]] for name in BoosterROS.NAOTH_JOINT_ORDER],
            "effort"    : [msg.effort[source_index[name]]   for name in BoosterROS.NAOTH_JOINT_ORDER],
        }

        return data


    def odometry_to_dict(self, msg):
        #ts_ns = int(msg.header.stamp.sec) * 1_000_000_000 + int(msg.header.stamp.nanosec)

        data = {
            #"timestamp": ts_ns,

            "translation": {
                "x": float(msg.x),
                "y": float(msg.y),
            },

            "orientation": {
                "theta": float(msg.theta)
            }
        }

        return data

    def motor_state_to_dict(self, msg):
        return {
            "mode"          : int(msg.mode),
            "position"      : float(msg.q),
            "velocity"      : float(msg.dq),
            "acceleration"  : float(msg.ddq),
            "effort"        : float(msg.tau_est),
            "temperature"   : int(msg.temperature),
            "lost"          : int(msg.lost),
            "reserve"       : [int(x) for x in msg.reserve],
        }


    def low_state_to_dict(self, msg):
        data = {
            "imu_state": {
                "rpy"   : [float(x) for x in msg.imu_state.rpy ],
                "gyro"  : [float(x) for x in msg.imu_state.gyro],
                "acc"   : [float(x) for x in msg.imu_state.acc ],
            },
            "motor_state_parallel": [
                self.motor_state_to_dict(m) for m in msg.motor_state_parallel
            ],
            "motor_state_serial": [
                self.motor_state_to_dict(m) for m in msg.motor_state_serial
            ],
        }

        return data


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


    def send_dict_as_msgpack(self, data):
        # pack the sensor data
        msg = msgpack.packb(data, use_bin_type=True)

        # prefix with 4-byte length
        packet = struct.pack("!I", len(msg)) + msg
        self.send_data(packet)


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