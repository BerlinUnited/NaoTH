#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
reformate the order of joint data
"""

import sys

target_names = "Time HeadPitch HeadYaw RShoulderPitch LShoulderPitch RShoulderRoll LShoulderRoll RElbowYaw LElbowYaw RElbowRoll LElbowRoll RHipYawPitch LHipYawPitch RHipPitch LHipPitch RHipRoll LHipRoll RKneePitch LKneePitch RAnklePitch LAnklePitch RAnkleRoll LAnkleRoll".split(' ')


filename = sys.argv[1]
f = open(filename, 'r')
lines = f.read().split('\n')

# read the joint name ( the first line )
names = lines[0].split(' ')

order = []

for n in target_names :
    for i in range(0, len(names)) :
        if n == names[i] :
            order.append(i)
            pass

# print order

for i in  range(1, len(lines)-1):
    data = lines[i].split(' ')
    newdata = []
    for j in order :
        newdata.append(data[j])
    print ' '.join(newdata)
