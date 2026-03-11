
import os
import time

import socket
import msgpack
import struct

from booster_robotics_sdk_python import (
    ChannelFactory,
    B1LocoClient,
    RobotMode
)


'''
>>> print(dir(B1LocoClient))
['ActivateCustomTrainedTraj', 'ChangeMode', 'ControlDexterousHand', 'ControlGripper', 'Dance', 'GetFrameTransform', 'GetMode', 'GetRobotInfo', 'GetStatus', 'GetUp', 'GetUpWithMode', 'Handshake', 'Init', 'InitWithName', 'LieDown', 'LoadCustomTrainedTraj', 'Move', 'MoveHandEndEffector', 'MoveHandEndEffectorV2', 'MoveHandEndEffectorWithAux', 'PlaySound', 'RecordTrajectory', 'ReplayTrajectory', 'ResetOdometry', 'RotateHead', 'RotateHeadWithDirection', 'SendApiRequest', 'Shoot', 'StopHandEndEffector', 'StopSound', 'SwitchHandEndEffectorControlMode', 'UnloadCustomTrainedTraj', 'UpperBodyCustomControl', 'WaveHand', 'WholeBodyDance', 'ZeroTorqueDrag', '__class__', '__delattr__', '__dir__', '__doc__', '__eq__', '__format__', '__ge__', '__getattribute__', '__gt__', '__hash__', '__init__', '__init_subclass__', '__le__', '__lt__', '__module__', '__ne__', '__new__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__sizeof__', '__str__', '__subclasshook__']
'''

'''
>>> import booster_robotics_sdk_python as b
>>> print(dir(b))
['AXIS', 'Action', 'AiApiId', 'AiClient', 'AiResponse', 'AiSubtitleSubscriber', 'ArmController', 'ArmJoint', 'AsrChunk', 'AsrConfig', 'B1BatteryStateSubscriber', 'B1LocoClient', 'B1LowCmdPublisher', 'B1LowHandDataSubscriber', 'B1LowHandTouchDataSubscriber', 'B1LowStateSubscriber', 'B1OdometerStateSubscriber', 'BUTTON_DOWN', 'BUTTON_UP', 'BatteryState', 'BodyControl', 'BoosterHandType', 'ChangeModeParameter', 'ChannelFactory', 'ControlDexterousHandParameter', 'ControlGripperParameter', 'CustomModel', 'CustomModelParams', 'CustomTrainedTraj', 'DanceId', 'DanceParameter', 'DetectResults', 'DexterousFingerParameter', 'Frame', 'GetDetectionObjectParameter', 'GetFrameTransformParameter', 'GetModeResponse', 'GetRobotInfoResponse', 'GetStatusResponse', 'GetUpWithModeParameter', 'GripperControlMode', 'GripperMotionParameter', 'HAT', 'HandAction', 'HandIndex', 'HandReplyData', 'HandReplyParam', 'HandTouchData', 'HandTouchParam', 'HandshakeParameter', 'ImuState', 'JointIndex', 'JointIndexWith7DofArm', 'JointOrder', 'LightApiId', 'LightControlClient', 'LlmConfig', 'LocoApiId', 'LowCmd', 'LowCmdType', 'LowState', 'LuiApiId', 'LuiAsrChunkSubscriber', 'LuiClient', 'LuiTtsConfig', 'LuiTtsParameter', 'MotorCmd', 'MotorState', 'MoveController', 'MoveHandEndEffectorParameter', 'MoveParameter', 'NONE', 'Odometer', 'Orientation', 'PARALLEL', 'PlaySoundParameter', 'Position', 'Posture', 'ProneBodyControlPosture', 'Quaternion', 'REMOVE', 'RecordTrajectoryParameter', 'RemoteControllerEvent', 'ReplayTrajectoryParameter', 'RobotMode', 'RotateHeadParameter', 'RotateHeadWithDirectionParameter', 'SERIAL', 'SetLEDLightColorParameter', 'SpeakParameter', 'StartAiChatParameter', 'StartVisionServiceParameter', 'Subtitle', 'SwitchHandEndEffectorControlModeParameter', 'Transform', 'TtsConfig', 'UpperBodyCustomControlParameter', 'VisionApiId', 'VisionClient', 'WaveHandParameter', 'WholeBodyDanceId', 'WholeBodyDanceParameter', 'ZeroTorqueDragParameter', '__builtins__', '__cached__', '__doc__', '__file__', '__loader__', '__name__', '__package__', '__path__', '__spec__', '__version__', '_core', 'arm_controller', 'kActivateCustomTrainedTraj', 'kArbicDance', 'kBody', 'kBoosterRobotUserId', 'kBoxingStyleKick', 'kChangeMode', 'kCheeringGesture', 'kControlDexterousHand', 'kControlGripper', 'kCrankDownLeft', 'kCrankDownRight', 'kCrankUpLeft', 'kCrankUpRight', 'kCustom', 'kDabbingGesture', 'kDamping', 'kDance', 'kDanceNewYear', 'kDanceNezha', 'kDanceTowardsFuture', 'kForce', 'kGestureBoxing', 'kGestureCheer', 'kGestureDabbing', 'kGestureLuckyCat', 'kGestureRespect', 'kGestureUltraman', 'kGetDetectionObject', 'kGetFrameTransform', 'kGetMode', 'kGetRobotInfo', 'kGetStatus', 'kGetUp', 'kGetUpWithMode', 'kGoalie', 'kHandClose', 'kHandControl', 'kHandOpen', 'kHandShake', 'kHandWave', 'kHandshake', 'kHead', 'kHeadPitch', 'kHeadYaw', 'kHumanlikeGait', 'kInactive', 'kInsideFoot', 'kInspireHand', 'kInspireTouchHand', 'kIsaacLab', 'kJointCnt', 'kJointCnt7DofArm', 'kLeftElbowPitch', 'kLeftElbowYaw', 'kLeftFoot', 'kLeftHand', 'kLeftHandRoll', 'kLeftHipPitch', 'kLeftHipRoll', 'kLeftHipYaw', 'kLeftKneePitch', 'kLeftShoulderPitch', 'kLeftShoulderRoll', 'kLeftWristPitch', 'kLeftWristYaw', 'kLieDown', 'kLoadCustomTrainedTraj', 'kLuckyCatGesture', 'kMichaelDance1', 'kMichaelDance2', 'kMichaelDance3', 'kMoonWalk', 'kMove', 'kMoveHandEndEffector', 'kMuJoCo', 'kNewYear', 'kNezha', 'kPlaySound', 'kPosition', 'kPrepare', 'kProneBody', 'kPushUp', 'kRecordTraj', 'kRecordTrajectory', 'kReplayTrajectory', 'kResetOdometry', 'kRespectGesture', 'kRevoHand', 'kRightElbowPitch', 'kRightElbowYaw', 'kRightFoot', 'kRightHand', 'kRightHandRoll', 'kRightHipPitch', 'kRightHipRoll', 'kRightHipYaw', 'kRightKneePitch', 'kRightShoulderPitch', 'kRightShoulderRoll', 'kRightWristPitch', 'kRightWristYaw', 'kRotateHead', 'kRotateHeadWithDirection', 'kRoundhouseKick', 'kRunRecordedTraj', 'kSendTtsText', 'kSetLEDLightColor', 'kShoot', 'kSoccer', 'kSoccerGait', 'kSpeak', 'kStartAiChat', 'kStartAsr', 'kStartFaceTracking', 'kStartTts', 'kStartVisionService', 'kStop', 'kStopAiChat', 'kStopAsr', 'kStopFaceTracking', 'kStopHandEndEffector', 'kStopLEDLightControl', 'kStopSound', 'kStopTts', 'kStopVisionService', 'kSwitchHandEndEffectorControlMode', 'kTopicAiSubtitle', 'kTopicBoosterHandData', 'kTopicFallDown', 'kTopicHandTouchData', 'kTopicJointCtrl', 'kTopicLowState', 'kTopicOdometerState', 'kTopicProneBodyControlStatus', 'kTopicRobotReplayTrajID', 'kTopicRobotStates', 'kTopicTF', 'kTowardsFuture', 'kUltramanGesture', 'kUnknown', 'kUnloadCustomTrainedTraj', 'kUpperBodyCustomControl', 'kWaist', 'kWalking', 'kWaveHand', 'kWholeBodyDance', 'kZeroTorqueDrag', 'move_controller']
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
header:
  stamp:
    sec: 1772837540
    nanosec: 88464145
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
- -1.4899243116378784
- 0.018791837617754936
- -0.4943403899669647
- 0.1783307045698166
- 1.4385343790054321
- -0.022243401035666466
- 0.4905053377151489
- -0.07000076025724411
- -0.005531395319849253
- 0.001716639962978661
- 0.12722209095954895
- -0.08834041334447584
- 0.008991102336957796
- -0.048256658017635345
- 0.023079270496964455
- 0.0036240178160369396
- 0.12836651504039764
- -0.10622044787208584
- -0.022112139430561565
velocity:
- 0.00033535296097397804
- 0.0005197153659537435
- -0.0307103730738163
- 0.0249040387570858
- -0.013410419225692749
- 0.01807217113673687
- 0.02008192054927349
- -0.013062581419944763
- -0.03073566034436226
- 0.00905674509704113
- -0.01336824893951416
- -0.001368747209198773
- 0.003603203222155571
- -0.008747158572077751
- 0.00830004740190355
- 0.007238856889448549
- 0.005498512182384729
- -0.006513504311442375
- -0.0030761153902858496
- 0.0040065753273665905
- -0.002637879852353868
- 0.005767930617681775
effort:
- -0.01565999910235405
- 0.025280000641942024
- 0.002136295661330223
- 0.9057893753051758
- -0.191412091255188
- -0.05896176025271416
- 0.21704764664173126
- -0.39948728680610657
- 0.22046571969985962
- 0.1106601133942604
- 1.466153860092163
- -1.0027838945388794
- 0.24153847992420197
- -0.31992673873901367
- 2.393650301626814
- -0.08437297561486645
- 0.3490842580795288
- -1.070769190788269
- 0.172527477145195
- -0.4362637400627136
- 1.3218565426232527
- 0.8064860889721751
---
'''




'''
info = client.GetRobotInfo()
print(f"Name    : {info.name}")
print(f"Nickname: {info.nickname}")
print(f"Version : {info.version}")
print(f"Model   : {info.model}")
                

client.ChangeMode(b1.RobotMode.kWalking)
client.ChangeMode(b1.RobotMode.kDamping)

client.GetUp()
client.GetUpWithMode(mode)
client.Move(x, y, yaw)

client.ResetOdometry()

BatteryState
BodyControl
B1OdometerStateSubscriber
B1BatteryStateSubscriber
Orientation

RotateHeadWithDirectionParameter
'''

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


class Booster:
    def __init__(self):
        #ChannelFactory.Instance().Init(0)
        print("Init ChannelFactory ...")
        ChannelFactory.Instance().Init(0, "127.0.0.1")
        time.sleep(2)
        
        print("Init B1LocoClient ...")
        self.body_client = B1LocoClient()
        self.body_client.Init()
        time.sleep(3)
        
        info = self.body_client.GetRobotInfo()
        print(f"Name    : {info.name}")
        print(f"Nickname: {info.nickname}")
        print(f"Version : {info.version}")
        print(f"Model   : {info.model}")
        
    
    '''
    expect:
    
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
    '''
    def send_actuators(self, actuators):
        
        #print(actuators)
        #print(dir(actuators))
        
        headRotation = actuators["headRotation"]
        self.body_client.RotateHead(headRotation["pitch"], headRotation["yaw"])
        
        return
        
        if actuators["id"] == "walk":
            walkVelocity = actuators["walkVelocity"]
            self.body_client.ChangeMode(RobotMode.kWalking)
            self.body_client.Move(walkVelocity["x"], walkVelocity["y"], walkVelocity["rotation"])
        elif actuators["id"] == "stand":
            self.body_client.ChangeMode(RobotMode.kPrepare)
            #self.body_client.Move(0, 0, 0)
        else:
            self.body_client.ChangeMode(RobotMode.kDamping)
            
        #self.body_client.GetUp()




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
    
    booster = Booster()
    
    server = UnixSocketConnector("/tmp/naoth_body")

    try:
        while True:
            # read commands
            actuators = server.receive_actuators()
            
            if actuators is None:
                print("Client disconnected.")
                break
            
            booster.send_actuators(actuators)
            
            # send sensors
            #server.send_data()
            
    finally:
        server.close()
    


if __name__ == "__main__":
    main()