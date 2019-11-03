# Blender Patch Generation

Robert Gützkow created a blender model of the soccer field and the Nao Robot and the black and white ball. This Blender Model
can be used to automatically generate synthetic images for a deep learning based detector. 

#### Download the blend file from  into this folder
run `python get_blender_file.py`  

The blender model was created to work in Blender 2.78

#### Generate Patches
run `generate_patches.sh`

This script assumes that the blender binary can be found on the PATH

Information on how to call blender python script with custom arguments:

https://blender.stackexchange.com/questions/6817/how-to-pass-command-line-arguments-to-a-blender-python-script
#### Note on Python Dependencies
the script needs the blender specific python packages (bpy and mathutils). It can be complicated to install those with pip.
But the blender binary already includes those. 

