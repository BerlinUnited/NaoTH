#!/bin/bash
# Information on how to call blender python script with custom arguments: https://blender.stackexchange.com/questions/6817/how-to-pass-command-line-arguments-to-a-blender-python-script 

#blender RoboCup_v_1_2_1_nonInteractive-benji.blend --background --python generate_patches_from_scene.py -- -c

while true; do
	blender RoboCup_v_1_2_1_nonInteractive-benji.blend --python generate_patches_from_scene.py -- -c
	sleep 5
done

#for i in `seq 1 10`;
#do
#	blender RoboCup_v_1_2_1_nonInteractive-benji.blend --python generate_patches_from_scene.py -- -c
#	sleep 5
#done    