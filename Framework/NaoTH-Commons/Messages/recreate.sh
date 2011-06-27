#!/bin/bash
../../../Extern/bin/protoc --java_out=../../../RobotControl/RobotConnector/src/ --cpp_out=../Source/Messages/ *.proto
