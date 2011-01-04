#!/bin/bash
protoc --java_out=../../RobotControl/RobotControlGUI/src/ --cpp_out=../Source/Messages/ *.proto
