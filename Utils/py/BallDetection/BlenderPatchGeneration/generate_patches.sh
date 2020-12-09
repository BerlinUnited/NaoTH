#!/bin/bash


#blender RoboCup_v_1_2_1_nonInteractive-benji.blend --background --python generate_patches_from_scene.py -- -c

while true; do
	blender RoboCup_v_1_2_1_nonInteractive.blend --python generate_patches_from_scene.py -- -c
	sleep 5
done
