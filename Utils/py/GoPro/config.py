from goprocam.constants import *

# NaoCam / a1b0a1b0a1
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

# all available teams
teams = {
    0: "Invisibles",
    1: "UT Austin Villa",
    2: "Austrian Kangaroos",
    3: "Bembelbots",
    4: "Berlin United",
    5: "B-Human",
    6: "Cerberus",
    7: "DAInamite",
    8: "Dutch Nao Team",
    9: "Edinferno",
    10: "Kouretes",
    11: "MiPal",
    12: "Nao Devils Dortmund",
    13: "Nao-Team HTWK",
    14: "Northern Bites",
    15: "NTU RoboPAL",
    16: "RoboCanes",
    17: "RoboEireann",
    18: "UNSW Sydney",
    19: "SPQR Team",
    20: "TJArk",
    21: "UChile Robotics Team",
    22: "UPennalizers",
    23: "Crude Scientists",
    24: "HULKs",
    26: "MRL-SPL",
    27: "Philosopher",
    28: "Rimal Team",
    29: "SpelBots",
    30: "Team-NUST",
    31: "UnBeatables",
    32: "UTH-CAR",
    33: "NomadZ",
    34: "SPURT",
    35: "Blue Spider",
    36: "Camellia Dragons",
    37: "JoiTech-SPL",
    38: "Linköping Humanoids",
    39: "WrightOcean",
    40: "Mars",
    41: "Aztlan Team",
    42: "CMSingle",
    43: "TeamSP",
    44: "Luxembourg United",
    90: "DoBerMan",
    91: "B-HULKs",
    92: "Swift-Ark",
    93: "Team USA"
}