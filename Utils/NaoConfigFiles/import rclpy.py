import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image, CameraInfo
import cv2
from cv_bridge import CvBridge
#from ultralytics import YOLO
import numpy as np
from geometry_msgs.msg import PoseStamped
import os
import socket
from rclpy.qos import QoSProfile, QoSReliabilityPolicy, QoSHistoryPolicy, QoSDurabilityPolicy
from message_filters import Subscriber, TimeSynchronizer, ApproximateTimeSynchronizer



class RosUnixImageServer(Node):
    def __init__(self, sock_path):
        super().__init__('image_subscriber')
        # Create listening socket
        self.srv = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.sock_path = sock_path

        # Remove stale socket file
        try:
            os.unlink(self.sock_path)
        except FileNotFoundError:
            pass

        self.srv.bind(self.sock_path)     # <-- creates the socket file
        self.srv.listen(1)

        print(f"Unix socket server listening at {self.sock_path}")
        print("Waiting for C++ client to connect...")

        self.conn, _ = self.srv.accept()  # blocking
        print("Client connected.")

        qos_sensors = QoSProfile(
            history     = QoSHistoryPolicy.KEEP_LAST,
            depth       = 20,  # logger can lag briefly without losing frames (tune: ~0.3–1s worth)
            reliability = QoSReliabilityPolicy.RELIABLE,
            durability  = QoSDurabilityPolicy.VOLATILE
        )

        # Subscribe to the booster left camera image
        self.left_camera_sub = Subscriber(self,
            Image,
            '/boostercamera/head/raw/rgb',
            qos_profile = qos_sensors
        )
        self.sync = ApproximateTimeSynchronizer(
            [self.left_camera_sub],
            queue_size = 30,
            slop = 0.033  # 30 ms
        )
        self.bridge = CvBridge()
        self.sync.registerCallback(self.combined_callback)

    def combined_callback(self, camera_msg):
        #data = scale_to_nao(img)
        yuv = np.frombuffer(camera_msg.data, dtype=np.uint8).reshape((camera_msg.height * 3 // 2, camera_msg.width))
        img = cv2.cvtColor(yuv, cv2.COLOR_YUV2BGR_NV12)
        data = self.scale_to_nao_centered(img)
        print(data.shape)
        self.conn.sendall(data.tobytes())

    def scale_to_nao_centered(self, img):
        h, w = img.shape[:2]

        # scale so width becomes 640 while keeping aspect ratio
        scale = 640.0 / w
        new_w = 640
        new_h = int(round(h * scale))

        resized = cv2.resize(img, (new_w, new_h), interpolation=cv2.INTER_LANCZOS4)

        # crop centered vertically so final height is 480
        if new_h < 480:
            raise RuntimeError("scaled image height is smaller than 480, cannot crop")

        top = (new_h - 480) // 2
        bottom = top + 480
        cropped = resized[top:bottom, 0:640]

        h, w = cropped.shape[:2]
        yuv444 = cv2.cvtColor(cropped, cv2.COLOR_BGR2YCrCb)

        Y = yuv444[:, :, 0].astype(np.uint8)
        U = yuv444[:, :, 1].astype(np.uint8)
        V = yuv444[:, :, 2].astype(np.uint8)

        out = np.empty((h, w * 2), dtype=np.uint8)  # 2 bytes per pixel
        out[:, 0::4] = Y[:, 0::2]  # Y0
        out[:, 1::4] = V[:, 0::2]
        out[:, 2::4] = Y[:, 1::2]  # Y1
        out[:, 3::4] = U[:, 0::2]

        return out
    def send_image_bytes(self, img_bytes):
        self.conn.sendall(img_bytes)

    def close(self):
        self.conn.close()
        self.srv.close()
        os.unlink(self.sock_path)

def main(args=None):
    rclpy.init(args=args)
#    node = YoloImageSubscriber()
    node = RosUnixImageServer("/tmp/naoth_image")
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()