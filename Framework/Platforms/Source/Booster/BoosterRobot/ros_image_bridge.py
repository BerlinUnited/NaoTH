# demo.py
# to execute this demo, please run following
# source source /opt/ros/humble/setup.bash
# python demo.py

'''

# Booster K1

## Camera Info
- 20fps
- opening angle (w x h): 105° x 94°
- resolution (w x h): 544 x 488

## relevantros messages:


/booster_camera_bridge/image_right_raw  # Rectified left-eye
/booster_camera_bridge/image_right_raw  # Rectified right-eye

/image_left_raw/camera_info             # Rectified left-eye  camera information (raw)
/image_right_raw/camera_info            # Rectified right-eye camera information (raw)


/StereoNetNode/rectified_image          # Rectified left-eye  image
/StereoNetNode/rectified_right_image    # Rectified right-eye image

/StereoNetNode/stereonet_depth          # Rectified left-eye aligned depth image
/StereoNetNode/stereonet_visual         # Rectified left-eye image with color-rendered depth map


NOTE: 

## Listen to topics in command line
```
ros2 topic echo /booster_camera_bridge/image_left_raw/camera_info
ros2 topic echo /booster_camera_bridge/image_right_raw

ros2 topic echo /booster_camera_bridge/StereoNetNode/rectified_image


ros2 topic type /head_pose_stamped
ros2 topic info /head_pose_stamped
```

'''

import os
import time
import numpy as np
import cv2

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from geometry_msgs.msg import PoseStamped

from message_filters import Subscriber, TimeSynchronizer, ApproximateTimeSynchronizer
from rclpy.qos import QoSProfile, QoSReliabilityPolicy, QoSHistoryPolicy, QoSDurabilityPolicy

import msgpack
import struct

# naoth
#from naoth.log import Reader as LogReader, Frame as LogFrame
#import naoth.pb.Framework_Representations_pb2 as framework_pb2

from image_bridge import RosUnixImageServer


class ImageServer(Node):

  def __init__(self, output_path = None):
    super().__init__('image_logger')
    
    qos_logger = QoSProfile(
        history     = QoSHistoryPolicy.KEEP_LAST,
        depth       = 20,  # logger can lag briefly without losing frames (tune: ~0.3–1s worth)
        reliability = QoSReliabilityPolicy.RELIABLE,
        durability  = QoSDurabilityPolicy.VOLATILE
    )
    
    self.left_sub  = Subscriber(self, Image, '/booster_camera_bridge/image_left_raw' , qos_profile = qos_logger)
    self.right_sub = Subscriber(self, Image, '/booster_camera_bridge/image_right_raw', qos_profile = qos_logger)
    
    self.head_pose = Subscriber(self, PoseStamped, '/head_pose_stamped', qos_profile = qos_logger)

    self.sync = ApproximateTimeSynchronizer(
        [self.left_sub, self.right_sub],
        queue_size=30,
        slop=0.002  # 2 ms
    )
    
    '''
    self.sync = TimeSynchronizer(
        [self.left_sub, self.right_sub],
        queue_size=10
    )
    '''

    self.sync.registerCallback(self.stereo_callback)

    self.ts = TimeSynchronizer([self.head_pose], 10)
    self.ts.registerCallback(self.head_pose_callback)
    
    self.current_pose = None
    self.socket = None

  def __enter__(self):
    self.socket = RosUnixImageServer("/tmp/naoth_image")
    return self

  def __exit__(self, exc_type, exc, tb):
    if self.socket is not None:
        self.socket.close()
    return False


  def image_to_proto(self, msg):
  
    # msg is sensor_msgs.msg.Image
    pb = framework_pb2.Image()

    # Dimensions
    pb.width  = msg.width
    pb.height = msg.height

    pb.data   = bytes(msg.data)

    # Format mapping
    #enc = (msg.encoding or "").lower()
    pb.format = framework_pb2.Image.YUV420_NV12

    # Timestamp in ms
    t_ms = msg.header.stamp.sec * 1000 + (msg.header.stamp.nanosec // 1_000_000)
    # enforce 32 bit
    pb.timestamp = int(t_ms & 0xFFFFFFFF) 

    # maybe later?
    # pb.cameraInfo.... = ...

    return pb
    
  
  def image_to_proto_nao(self, msg):
    # msg is sensor_msgs.msg.Image
    pb = framework_pb2.Image()

    # Dimensions
    pb.width  = 640
    pb.height = 480

    pb.data   = self.nv12_544x448_to_yuyv_640x480_centered(msg.data)

    # Format mapping
    #enc = (msg.encoding or "").lower()
    pb.format = framework_pb2.Image.YUV422

    # Timestamp in ms
    t_ms = msg.header.stamp.sec * 1000 + (msg.header.stamp.nanosec // 1_000_000)
    # enforce 32 bit
    pb.timestamp = int(t_ms & 0xFFFFFFFF) 

    # maybe later?
    # pb.cameraInfo.... = ...

    return pb
    
  
  # convert to NAO images
  def nv12_544x448_to_yuyv_640x480_centered(self, nv12_bytes):
    in_w  = 544
    in_h  = 448
    
    out_w = 640
    out_h = 480
  
    nv12 = np.frombuffer(nv12_bytes, dtype=np.uint8)
    y_size = in_w * in_h
    uv_size = in_w * (in_h // 2)
    if nv12.size < y_size + uv_size:
      raise ValueError("nv12 buffer too small, expected at least {} bytes, got {}".format(y_size + uv_size, nv12.size))
  
    y_in = nv12[:y_size].reshape((in_h, in_w))
    uv_in = nv12[y_size:y_size + uv_size].reshape((in_h // 2, in_w))
  
    pad_x = (out_w - in_w) // 2
    pad_y = (out_h - in_h) // 2
    if pad_x < 0 or pad_y < 0 or (out_w - in_w) % 2 != 0 or (out_h - in_h) % 2 != 0:
      raise ValueError("output size must be >= input size and centered padding must be integer on both sides")
  
    y_out = np.zeros((out_h, out_w), dtype=np.uint8)
    y_out[pad_y:pad_y + in_h, pad_x:pad_x + in_w] = y_in
  
    uv_pairs = uv_in.reshape((in_h // 2, in_w // 2, 2))  # last dim: U then V for each 2x2 block
    uv_full = np.repeat(uv_pairs, 2, axis=0)  # upsample vertically to 4:2:2 (nearest), shape (in_h, in_w//2, 2)
  
    out_w_pairs = out_w // 2
    pad_x_pairs = pad_x // 2
    u422_out = np.zeros((out_h, out_w_pairs), dtype=np.uint8)
    v422_out = np.zeros((out_h, out_w_pairs), dtype=np.uint8)
    u422_out[pad_y:pad_y + in_h, pad_x_pairs:pad_x_pairs + (in_w // 2)] = uv_full[:, :, 0]
    v422_out[pad_y:pad_y + in_h, pad_x_pairs:pad_x_pairs + (in_w // 2)] = uv_full[:, :, 1]
  
    yuyv = np.zeros((out_h, out_w * 2), dtype=np.uint8)
    yuyv[:, 0::2] = y_out
    yuyv[:, 1::2][:, 0::2] = u422_out
    yuyv[:, 1::2][:, 1::2] = v422_out
  
    return yuyv.tobytes()
    
    
  def msg2img(self, msg):
    yuv = np.frombuffer(msg.data, dtype=np.uint8).reshape((msg.height * 3 // 2, msg.width))
    bgr_image = cv2.cvtColor(yuv, cv2.COLOR_YUV2BGR_NV12)
    
    # Timestamp in ms
    t_ms = msg.header.stamp.sec * 1000 + (msg.header.stamp.nanosec // 1_000_000)
    # enforce 32 bit
    timestamp = int(t_ms & 0xFFFFFFFF) 
    
    return bgr_image
  
  
  
  def stereo_callback(self, left_msg, right_msg):
    # send scaled
    img_left = self.msg2img(left_msg)
    self.socket.send_image(img_left)
    
    # send centered
    #nao_img_bytes = self.nv12_544x448_to_yuyv_640x480_centered(left_msg.data)
    #self.socket.send_image_bytes(nao_img_bytes)
    
    
    headPose_msgpack = pose_stamped_to_msgpack(self.current_pose)
    
    # prefix with 4-byte length
    packet = struct.pack("!I", len(headPose_msgpack)) + headPose_msgpack
    self.socket.send_image_bytes(packet)
    
    
    '''
    print(f"Frame {self.frame_number}")
    
    # Create a new log frame that is not attached to a LogReader
    frame = LogFrame(frame_start = 0, frame_number = self.frame_number, scanner = None, parser = None)
    
    # generate image messages
    imageLeft  = self.image_to_proto(left_msg)
    imageRight = self.image_to_proto(right_msg)
    frame.add_field("ImageLeft", imageLeft)
    frame.add_field("ImageRight", imageRight)
    
    # HACK: NAO image format
    imageTop    = self.image_to_proto_nao(left_msg)
    imageBottom = self.image_to_proto_nao(right_msg)
    frame.add_field("ImageTop", imageTop)
    frame.add_field("Image"   , imageBottom)
    
    # generate FrameInfo
    frameInfo = framework_pb2.FrameInfo()
    frameInfo.frameNumber = self.frame_number
    frameInfo.time        = imageLeft.timestamp # use the left image as a baseline time
    frame.add_field("FrameInfo", frameInfo)
    
    # generate the RobotInfo
    if self.frame_number == 0:
        robotInfo = framework_pb2.RobotInfo()
        robotInfo.platform = "Booster-K1";
        robotInfo.bodyNickName = "";
        robotInfo.headId = "";
        robotInfo.bodyId = "";
        robotInfo.basicTimeStep = 0;
        robotInfo.robotName = "41";
        frame.add_field("RobotInfo", robotInfo)
    
    if self.current_pose is not None:
        frame.add_field("HeadPose", self.current_pose)
    
    # write log frame to file
    self.output.write(bytes(frame))
    
    # increase for the next frame
    self.frame_number += 1
    
    # DEBUG: check the time offset
    left_ns  = left_msg.header.stamp.sec  * 1000000000 + left_msg.header.stamp.nanosec
    right_ns = right_msg.header.stamp.sec * 1000000000 + right_msg.header.stamp.nanosec
    diff_ns  = left_ns - right_ns
    if diff_ns > 0:
      self.get_logger().info("Stereo sync diff (ns) = {}".format(diff_ns))
  '''
  
  '''
  def pose_stamped_to_headpose_pb(self, msg: PoseStamped) -> framework_pb2.HeadPose:
    pb = framework_pb2.HeadPose()

    # translation
    pb.pose.translation.x = float(msg.pose.position.x)
    pb.pose.translation.y = float(msg.pose.position.y)
    pb.pose.translation.z = float(msg.pose.position.z)

    # rotation quaternion (optional in your schema, but we’ll fill it)
    pb.pose.rotation_quaternion.x = float(msg.pose.orientation.x)
    pb.pose.rotation_quaternion.y = float(msg.pose.orientation.y)
    pb.pose.rotation_quaternion.z = float(msg.pose.orientation.z)
    pb.pose.rotation_quaternion.w = float(msg.pose.orientation.w)

    # timestamp (optional uint64): ROS2 builtin_interfaces/Time -> uint64 nanoseconds
    # msg.header.stamp has sec + nanosec
    ts_ns = (int(msg.header.stamp.sec) * 1_000_000_000) + int(msg.header.stamp.nanosec)
    pb.timestamp = ts_ns

    return pb
  
  def pose_stamped_to_protobuf(self, msg: PoseStamped):
    pb = framework_pb2.HeadPose()

    # translation
    pb.pose.translation.x = float(msg.pose.position.x)
    pb.pose.translation.y = float(msg.pose.position.y)
    pb.pose.translation.z = float(msg.pose.position.z)

    # rotation quaternion (optional in your schema, but we’ll fill it)
    pb.pose.rotation_quaternion.x = float(msg.pose.orientation.x)
    pb.pose.rotation_quaternion.y = float(msg.pose.orientation.y)
    pb.pose.rotation_quaternion.z = float(msg.pose.orientation.z)
    pb.pose.rotation_quaternion.w = float(msg.pose.orientation.w)

    # timestamp (optional uint64): ROS2 builtin_interfaces/Time -> uint64 nanoseconds
    # msg.header.stamp has sec + nanosec
    ts_ns = (int(msg.header.stamp.sec) * 1_000_000_000) + int(msg.header.stamp.nanosec)
    pb.timestamp = ts_ns

    return pb
  '''
  
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
    
  
  def head_pose_callback(self, head_pose_msg):
    self.current_pose = head_pose_msg
    
    #self.current_pose = self.pose_stamped_to_headpose_pb(head_pose_msg)
    #print(self.current_pose.timestamp)
    
    
    
    
def main(args=None):
    
  rclpy.init(args=args)
    
  with ImageServer() as image_server:
    try:
      rclpy.spin(image_server)
    finally:
      image_server.destroy_node()
        
  rclpy.shutdown()


if __name__ == '__main__':
  main()