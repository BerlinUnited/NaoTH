'''
Convert images to a naoth logfile.
'''


import os

from naoth.log import Reader as LogReader, Frame as LogFrame
from naoth.pb.Framework_Representations_pb2 import Image, FrameInfo, RobotInfo

import numpy as np
import cv2

# List of images pairs: (left.png, right.png)
img_files = [
("raw_image_left_1771442554.png", "raw_image_right_1771442554.png"),
("raw_image_left_1771442555.png", "raw_image_right_1771442555.png"),
("raw_image_left_1771442556.png", "raw_image_right_1771442556.png"),
("raw_image_left_1771442557.png", "raw_image_right_1771442557.png"),
("raw_image_left_1771442558.png", "raw_image_right_1771442558.png"),
("raw_image_left_1771442559.png", "raw_image_right_1771442559.png"),
("raw_image_left_1771442560.png", "raw_image_right_1771442560.png"),
("raw_image_left_1771442561.png", "raw_image_right_1771442561.png"),
("raw_image_left_1771442562.png", "raw_image_right_1771442562.png"),
("raw_image_left_1771442563.png", "raw_image_right_1771442563.png"),
("raw_image_left_1771442564.png", "raw_image_right_1771442564.png"),
("raw_image_left_1771442565.png", "raw_image_right_1771442565.png"),
("raw_image_left_1771442566.png", "raw_image_right_1771442566.png"),
("raw_image_left_1771442567.png", "raw_image_right_1771442567.png"),
("raw_image_left_1771442568.png", "raw_image_right_1771442568.png"),
("raw_image_left_1771442569.png", "raw_image_right_1771442569.png"),
("raw_image_left_1771442570.png", "raw_image_right_1771442570.png"),
("raw_image_left_1771442571.png", "raw_image_right_1771442571.png"),
("raw_image_left_1771442572.png", "raw_image_right_1771442572.png"),
("raw_image_left_1771442573.png", "raw_image_right_1771442573.png"),
("raw_image_left_1771442574.png", "raw_image_right_1771442574.png"),
("raw_image_left_1771442575.png", "raw_image_right_1771442575.png"),
("raw_image_left_1771442576.png", "raw_image_right_1771442576.png"),
("raw_image_left_1771442577.png", "raw_image_right_1771442577.png"),
("raw_image_left_1771442578.png", "raw_image_right_1771442578.png"),
("raw_image_left_1771442579.png", "raw_image_right_1771442579.png"),
("raw_image_left_1771442580.png", "raw_image_right_1771442580.png"),
("raw_image_left_1771442581.png", "raw_image_right_1771442581.png"),
("raw_image_left_1771442582.png", "raw_image_right_1771442582.png")]

# scale images to fit the default NAO image format 640x480
def scale_to_nao(img):
    h, w = img.shape[:2]
    
    # scale so width becomes 640 while keeping aspect ratio
    scale = 640.0 / w
    new_w = 640
    new_h = int(round(h * scale))

    resized = cv2.resize(img, (new_w, new_h), interpolation=cv2.INTER_LANCZOS4)#INTER_LINEAR

    # crop from the top so final height is 448
    if new_h < 480:
      raise RuntimeError("scaled image height is smaller than 480, cannot crop")

    cropped = resized[new_h-480:new_h, 0:640]

    h, w = cropped.shape[:2]
    yuv444 = cv2.cvtColor(cropped, cv2.COLOR_BGR2YCrCb)
    Y = yuv444[:, :, 0].astype(np.uint8)
    U = yuv444[:, :, 1].astype(np.uint8)
    V = yuv444[:, :, 2].astype(np.uint8)

    out = np.empty((h, w * 2), dtype=np.uint8)  # 2 bytes per pixel
    out[:, 0::4] = Y[:, 0::2] # Y0
    out[:, 1::4] = V[:, 0::2]
    out[:, 2::4] = Y[:, 1::2] # Y1
    out[:, 3::4] = U[:, 0::2]
    
    return out


def image_to_proto(img):
    pb = Image()

    # Dimensions
    pb.width  = 640
    pb.height = 480
    pb.data   = bytes(img.data)

    # Format mapping
    #enc = (msg.encoding or "").lower()
    pb.format = Image.YUV422

    # Timestamp in ms
    pb.timestamp = 0

    # maybe later?
    # pb.cameraInfo.... = ...

    return pb
    

with open("booster_image.log", 'wb') as output:

    frame_number = 0

    for imageLeft_path, imageRight_path in img_files:
        print(imageLeft_path, imageRight_path)
        imageLeft  = cv2.imread(imageLeft_path, cv2.IMREAD_COLOR)
        imageRight = cv2.imread(imageRight_path, cv2.IMREAD_COLOR)

        imageLeft  = scale_to_nao(imageLeft)
        imageRight = scale_to_nao(imageRight)
        
        # Create a new log frame that is not attached to a LogReader
        frame = LogFrame(frame_start = 0, frame_number = frame_number, scanner = None, parser = None)
        
        # generate image messages
        imageLeft  = image_to_proto(imageLeft)
        imageRight = image_to_proto(imageRight)
        frame.add_field("ImageTop", imageLeft)
        frame.add_field("Image"   , imageRight)
        
        # generate FrameInfo
        frameInfo = FrameInfo()
        frameInfo.frameNumber = frame_number
        frameInfo.time        = imageLeft.timestamp # use the left image as a baseline time
        frame.add_field("FrameInfo", frameInfo)
        
        # generate the RobotInfo
        if frame_number == 0:
            robotInfo = RobotInfo()
            robotInfo.platform = "Booster-K1";
            robotInfo.bodyNickName = "";
            robotInfo.headId = "";
            robotInfo.bodyId = "";
            robotInfo.basicTimeStep = 0;
            robotInfo.robotName = "41";
            frame.add_field("RobotInfo", robotInfo)
        
        
        # write log frame to file
        output.write(bytes(frame))
    
        frame_number += 1
    