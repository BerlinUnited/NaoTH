#!/usr/bin/env python3
import os
import socket
import time
import cv2
import numpy as np

# static images for testing

IMAGE_PATH = "./images_left_right_raw_02/raw_image_left_1771442554.png"
FPS = 10

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
    
    
def scale_to_nao_centered(img):
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
    
    


class RosUnixImageServer:
    def __init__(self, sock_path):
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

    def send_image(self, img):
        #data = scale_to_nao(img)
        data = scale_to_nao_centered(img)
        print(data.shape)
        self.conn.sendall(data.tobytes())
        
    def send_image_bytes(self, img_bytes):
        self.conn.sendall(img_bytes)

    def close(self):
        self.conn.close()
        self.srv.close()
        os.unlink(self.sock_path)


if __name__ == '__main__':
    # ---- send loop ----

    server = RosUnixImageServer("/tmp/naoth_image")

    try:
        number = 0
        while True:
            left_image, right_image = img_files[number]
            img = cv2.imread("images_left_right_raw_02/" + left_image, cv2.IMREAD_COLOR)
            server.send_image(img)
            
            number = (number + 1) % len(img_files)
            
            print(f"Sent frame: {number}")
            time.sleep(1.0 / FPS)

    except (BrokenPipeError, ConnectionResetError):
        print("Client disconnected")

    finally:
        server.close()