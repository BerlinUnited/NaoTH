# demo.py
# to execute this demo, please run following
# source source /opt/ros/humble/setup.bash
# python demo.py

'''

ros2 topic echo -n 1 /booster_camera_bridge/image_left_raw/camera_info

ros2 topic echo /booster_camera_bridge/image_left_raw/camera_info

ros2 topic echo /booster_camera_bridge/image_right_raw

ros2 topic echo /booster_camera_bridge/StereoNetNode/rectified_image

'''

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
import cv2
import os
import numpy as np
import time

from rclpy.qos import QoSProfile, QoSReliabilityPolicy, QoSHistoryPolicy




class ImageSubscriber(Node):

  def __init__(self):
    super().__init__('image_subscriber')
    
    qos = QoSProfile(
        history=QoSHistoryPolicy.KEEP_LAST,
        depth=1,
        reliability=QoSReliabilityPolicy.BEST_EFFORT
    )
    
    '''
    self.depth_subscription = self.create_subscription(
      Image,
      '/booster_camera_bridge/StereoNetNode/stereonet_depth',
      self.depth_listener_callback,
      10)
      
    self.color_subscription = self.create_subscription(
      Image,
      '/booster_camera_bridge/StereoNetNode/rectified_image',
      self.color_listener_callback,
      10)
    '''
    
    self.left_subscription = self.create_subscription(
      Image,
      '/booster_camera_bridge/image_left_raw',
      self.raw_listener_callback_left,
      qos)
      
    self.right_subscription = self.create_subscription(
      Image,
      '/booster_camera_bridge/image_right_raw',
      self.raw_listener_callback_right,
      qos)
      
    self.bridge = CvBridge()
    
    self.t = 0

    # Create a directory named after the program creation time
    self.save_dir = os.path.join(os.getcwd(), f'images_{self.get_clock().now().to_msg().sec}')
    os.makedirs(self.save_dir, exist_ok=True)


  def depth_listener_callback(self, msg):
    self.get_logger().info('Receiving depth image')
    cv_image = self.bridge.imgmsg_to_cv2(msg, desired_encoding='passthrough')
    
    # Convert depth image from uint16 to meters
    depth_image_meters = cv_image * 0.001
    
    # Save the raw depth image with timestamp
    timestamp = self.get_clock().now().to_msg().sec
    raw_image_path = os.path.join(self.save_dir, f'depth_image_raw_{timestamp}.png')
    cv2.imwrite(raw_image_path, cv_image)
    
    
    # Normalize the depth image for display
    depth_image_normalized = cv2.normalize(depth_image_meters, None, 0, 255, cv2.NORM_MINMAX)
    depth_image_normalized = np.clip(depth_image_normalized, 0, 255).astype(np.uint8)
    
    # Apply color map to the normalized depth image
    depth_colormap = cv2.applyColorMap(depth_image_normalized, cv2.COLORMAP_JET)
    
    # Save the color rendered depth image
    color_image_path = os.path.join(self.save_dir, f'depth_image_color_{timestamp}.png')
    cv2.imwrite(color_image_path, depth_colormap)
    
    # Display the color rendered depth image
    #cv2.imshow('Depth Image', depth_colormap)
    #cv2.waitKey(1)


  def color_listener_callback(self, msg):
    self.get_logger().info('Receiving color image')

    yuv = np.frombuffer(msg.data, dtype=np.uint8).reshape((msg.height * 3 // 2, msg.width))
    bgr_image = cv2.cvtColor(yuv, cv2.COLOR_YUV2BGR_NV12)

    # Save the color image with timestamp
    timestamp = self.get_clock().now().to_msg().sec
    color_image_path = os.path.join(self.save_dir, f'color_image_{timestamp}.png')
    cv2.imwrite(color_image_path, bgr_image)

    # Display the color image
    #cv2.imshow('Color Image', bgr_image)
    #cv2.waitKey(1)


  def raw_listener_callback_left(self, msg):
    
    t = time.perf_counter()
    elapsed_ms = (t - self.t) * 1000
    print(f"{elapsed_ms:.3f} ms")
    self.t = t
      
    return
      
    yuv = np.frombuffer(msg.data, dtype=np.uint8).reshape((msg.height * 3 // 2, msg.width))
    bgr_image = cv2.cvtColor(yuv, cv2.COLOR_YUV2BGR_NV12)

    # Save the color image with timestamp
    timestamp = self.get_clock().now().to_msg().sec
    color_image_path = os.path.join(self.save_dir, f'raw_image_left_{timestamp}.png')
    cv2.imwrite(color_image_path, bgr_image)
  
  
  def raw_listener_callback_right(self, msg):
    return
    yuv = np.frombuffer(msg.data, dtype=np.uint8).reshape((msg.height * 3 // 2, msg.width))
    bgr_image = cv2.cvtColor(yuv, cv2.COLOR_YUV2BGR_NV12)

    # Save the color image with timestamp
    timestamp = self.get_clock().now().to_msg().sec
    color_image_path = os.path.join(self.save_dir, f'raw_image_right_{timestamp}.png')
    cv2.imwrite(color_image_path, bgr_image)


def main(args=None):
    rclpy.init(args=args)
    image_subscriber = ImageSubscriber()
    rclpy.spin(image_subscriber)
    image_subscriber.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()