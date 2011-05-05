#!/bin/bash
../../../Extern/bin/protoc --java_out=../../../RobotControl/RobotControlGUI/src/ --cpp_out=../Source/Messages/ *.proto
