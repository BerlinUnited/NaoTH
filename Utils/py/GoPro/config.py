from goprocam.constants import *

# NAOCAM / a1b0a1b0a1
# NAOCAM_2 / a1b0a1b0a1
# GP26329941 / cycle9210
# GP26297683 / epic0546

ssid = 'NAOCAM_2'
passwd = 'a1b0a1b0a1'
retries = -1

# if the following configs are set, the cam tries to set them before starting recording
# if you want to configure these setting manually, comment them out
fps = Video.FrameRate.FR30
fov = Video.Fov.Wide
resolution = Video.Resolution.R1080p

