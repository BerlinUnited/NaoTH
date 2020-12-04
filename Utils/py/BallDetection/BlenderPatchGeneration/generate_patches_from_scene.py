import bpy
import mathutils
import sys
import math
import random
import os
import datetime
import time
import argparse
from pathlib import Path

from abc import ABC, abstractmethod

from bpy.props import (StringProperty,
                       BoolProperty,
                       IntProperty,
                       FloatProperty,
                       EnumProperty,
                       PointerProperty,
                       )
from bpy.types import (Panel,
                       Operator,
                       PropertyGroup,
                       )

bl_info = {
    "name": "Automated Rendering for Deep Learning Data Generation",
    "description": "This script automates the data generation for the RoboCup ball detection.",
    "author": "Robert Guetzkow",
    "version": (1, 0, 2),
    "blender": (2, 78, 0),
    "location": "",
    "warning": "",
    "wiki_url": "",
    "tracker_url": "",
    "category": "Development"
}


# Test Arguments
class TestArguments:
    op_mode = "OP2"
    path = ""
    patch_size = 16
    num_images = 1
    generate_no_ball = True


# ------------------------------------------------------------------------
#    Store settings from UI
# ------------------------------------------------------------------------
class UISettings(PropertyGroup):
    op_mode = EnumProperty(name="Dropdown:",
                           description="Operation mode",
                           items=[('OP1', "Patch", ""),
                                  ('OP2', "Patch with mask", ""),
                                  ('OP3', "Image with mask", ""), ])

    patch_size = IntProperty(
        name="Patch size",
        description="An integer property",
        default=24,
        min=12,
        max=960)

    res_factor = FloatProperty(
        name="Resolution factor",
        description="Multiplier for Nao's camera resolution (1280x960)",
        default=1.0,
        min=0.1,
        max=2)

    num_balls = IntProperty(
        name="Number of balls",
        description="Number of patches to render with balls",
        default=1,
        min=0,
        max=999999)

    num_no_balls = IntProperty(
        name="Number of no balls",
        description="Number of patches to render without balls",
        default=1,
        min=0,
        max=999999)

    num_images = IntProperty(
        name="Number of images",
        description="Number of images to render",
        default=1,
        min=0,
        max=999999)

    path = StringProperty(
        name="",
        description="Path to directory, where images are saved",
        default="/home/benji/Documents/RoboCup/Projekte",
        maxlen=1024,
        subtype='DIR_PATH')

    use_motion_blur = BoolProperty(
        name="Use motion blur",
        description="Enable or disable motion blur",
        default=True)

    generate_no_ball = BoolProperty(
        name="Generate noball images",
        description="If disabled only numBall ball patches are generated with their masks",
        default=False)

    is_modal_instance_running = False


# ------------------------------------------------------------------------
#    Store state of current rendering instance
# ------------------------------------------------------------------------


class State:
    def __init__(self, addon_settings):
        self.op_mode = None
        self.patch_size = None
        self.res_factor = None
        self.num_balls = None
        self.num_no_balls = None
        self.num_images = None
        self.path = None
        self.use_motion_blur = False
        self.generate_no_ball = False

        # self.store_addon_settings(addon_settings)

        self.engine = 'CYCLES'
        self.use_persistent_data = True
        self.motion_blur_shutter = 0.1
        self.use_border = False
        self.use_crop_to_border = False

        self.border_min_x = 0.380
        self.border_min_y = 0.380
        self.border_max_x = 1 - self.border_min_x
        self.border_max_y = 1 - self.border_min_y
        self.use_mask_output = False

        self.motion_blur_every_1_out_of_x = 4
        self.advanced_image = False

        self.base_path = None
        self.path_ball = None
        self.path_ball_bw = None
        self.path_no_ball = None
        self.path_no_ball_bw = None
        self.path_img = None
        self.path_img_bw = None
        self.path_mask = None

        # Patch with masks paths
        self.path_ball_patch = None
        self.path_ball_patch_bw = None
        self.path_ball_patch_mask = None
        self.path_noball_patch = None
        self.path_noball_patch_bw = None
        self.path_noball_patch_mask = None
        #

        self.old_res_x = 0
        self.old_res_y = 0
        self.res_x = 0
        self.res_y = 0

        self.current_path = None
        self.current_path_bw = None
        self.current_path_mask = None
        self.current_frame = 0
        self.total_frames = 0

        self.allowed_zone_co = None
        self.field_co = None

        self.start_time = 0
        self.end_time = 0
        self.elapsed_time = 0

        self.estimated_exec_time = 0
        self.mean_exec_time_per_frame = 0

        self.used_keyframes = set()
        self.armature_dummy_loc_x = 0
        self.armature_dummy_loc_y = 0
        self.armature_dummy_loc_z = 0

        self.running = False
        self.cancelled = False

        self.is_set = False

    def store_addon_settings(self, addon_settings):
        self.op_mode = addon_settings.op_mode
        self.patch_size = addon_settings.patch_size
        self.res_factor = addon_settings.res_factor
        self.num_balls = addon_settings.num_balls
        self.num_no_balls = addon_settings.num_no_balls
        self.num_images = addon_settings.num_images
        self.path = addon_settings.path
        self.use_motion_blur = addon_settings.use_motion_blur
        self.generate_no_ball = addon_settings.generate_no_ball

    def restore_addon_settings(self, addon_settings):
        addon_settings.op_mode = self.op_mode
        addon_settings.patch_size = self.patch_size
        addon_settings.res_factor = self.res_factor
        addon_settings.num_balls = self.num_balls
        addon_settings.num_no_balls = self.num_no_balls
        addon_settings.num_images = self.num_images
        addon_settings.path = self.path
        addon_settings.use_motions_blur = self.use_motion_blur
        addon_settings.generate_no_ball = self.generate_no_ball

    def apply_scene_settings(self, current_scene):
        scene = current_scene
        scene.render.engine = self.engine
        scene.render.use_persistent_data = self.use_persistent_data
        scene.render.use_border = self.use_border
        scene.render.use_crop_to_border = self.use_crop_to_border
        scene.render.motion_blur_shutter = self.motion_blur_shutter

        scene.render.border_min_x = self.border_min_x
        scene.render.border_min_y = self.border_min_y
        scene.render.border_max_x = self.border_max_x
        scene.render.border_max_y = self.border_max_y

        scene.render.resolution_x = self.res_x
        scene.render.resolution_y = self.res_y

    def get_list_of_output_paths(self):
        return [self.base_path, self.path_ball, self.path_ball_bw, self.path_no_ball,
                self.path_no_ball_bw,
                self.path_img, self.path_img_bw, self.path_mask]


# ------------------------------------------------------------------------
#    IO class containing common functions
# ------------------------------------------------------------------------


class IO:
    @staticmethod
    def mkdirs(list_of_paths):
        for path in list_of_paths:
            IO.mkdir(path)

    @staticmethod
    def mkdir(path):
        if (path is not None) and (os.path.isdir(path) is False):
            os.mkdir(path)

    @staticmethod
    def rename_file(path, old_name, new_name):
        old_path_to_file = os.path.join(path, old_name)
        new_path_to_file = os.path.join(path, new_name)

        if (old_path_to_file is not None) and (os.path.isfile(old_path_to_file) is True):
            os.rename(old_path_to_file, new_path_to_file)
        else:
            print(old_path_to_file + " -> " + new_path_to_file)
            raise RuntimeError("Could not rename, file does not exist.")

    @staticmethod
    def print_frame_counter(frame, total_frames):
        print("Frame " + str(frame) + " / " + str(total_frames))

    @staticmethod
    def print_statistics(elapsed_time, estimated_exec_time):
        print("Render time of current frame: " + str(datetime.timedelta(seconds=elapsed_time)))
        print("Estimated time to completion: " + str(
            datetime.timedelta(seconds=estimated_exec_time)))
        print("")

    @staticmethod
    def print_error(message):
        print("-----------------------")
        print(" " + message)
        print("-----------------------")


# ------------------------------------------------------------------------
#    Scene editing class that helps reduce boiler plate code
# ------------------------------------------------------------------------


class SceneEditing:
    @staticmethod
    def set_file_output_nodes(use_mask_output):
        scene = bpy.context.scene
        if (use_mask_output is False) and len(
                scene.node_tree.nodes["file_output_mask"].inputs[0].links) > 0:
            scene.node_tree.links.remove(
                scene.node_tree.nodes["file_output_mask"].inputs[0].links[0])
        elif use_mask_output is True:
            scene.node_tree.links.new(scene.node_tree.nodes["Mix"].outputs["Image"],
                                      scene.node_tree.nodes["file_output_mask"].inputs[0])

    @staticmethod
    def add_keyframes_to_objs_for(frame_x, used_frames=None):
        for target in bpy.context.scene.objects:
            SceneEditing.add_keyframes_for(target, frame_x)

        if used_frames is not None:
            used_frames.add(frame_x)

    @staticmethod
    def add_keyframes_for(target, frame_x, used_frames=None):
        target.keyframe_insert(data_path="location", frame=frame_x)
        target.keyframe_insert(data_path="rotation_euler", frame=frame_x)
        target.keyframe_insert(data_path="scale", frame=frame_x)

        if (used_frames is not None) and (frame_x in used_frames):
            used_frames.remove(frame_x)

    @staticmethod
    def remove_keyframes_from_objs_for(frame_x, used_frames=None):
        for target in bpy.context.scene.objects:
            SceneEditing.remove_keyframes_for(target, frame_x)

        if (used_frames is not None) and (frame_x in used_frames):
            used_frames.remove(frame_x)

    @staticmethod
    def remove_keyframes_for(target, frame_x):
        target.keyframe_delete(data_path="location", frame=frame_x)
        target.keyframe_delete(data_path="rotation_euler", frame=frame_x)
        target.keyframe_delete(data_path="scale", frame=frame_x)

    @staticmethod
    def get_point_rotated(point_x, point_y, center_x, center_y, angle_in_degree):
        rotated_point_x = (point_x * math.cos(math.radians(angle_in_degree))
                           - point_y * math.sin(math.radians(angle_in_degree))) + center_x
        rotated_point_y = (point_x * math.sin(math.radians(angle_in_degree))
                           + point_y * math.cos(math.radians(angle_in_degree))) + center_y

        return rotated_point_x, rotated_point_y

    @staticmethod
    def get_vertices_world_space(object_name):
        obj = bpy.data.objects[object_name]
        vertices = obj.data.vertices

        obj_co = [vertex.co for vertex in vertices]

        for vertex in obj_co:
            vertex = obj.matrix_world * vertex

        return obj_co

    @staticmethod
    def change_rotation_euler(target, change_x, change_y, change_z, min_x, max_x, min_y, max_y,
                              min_z, max_z):
        rotation_euler = SceneEditing.get_random_rot(min_x, max_x, min_y, max_y, min_z, max_z)

        if change_x is True:
            target.rotation_euler[0] = rotation_euler[0]  # missing index?
        if change_y is True:
            target.rotation_euler[1] = rotation_euler[1]
        if change_z is True:
            target.rotation_euler[2] = rotation_euler[2]

    @staticmethod
    def change_rotation(target, change_x, change_y, change_z, min_x, max_x, min_y, max_y, min_z,
                        max_z):
        rot_x = random.uniform(min_x, max_x)
        rot_y = random.uniform(min_y, max_y)
        rot_z = random.uniform(min_z, max_z)

        if change_x is True:
            target.rotation[0] = math.radians(rot_x)
        if change_y is True:
            target.rotation[1] = math.radians(rot_y)
        if change_z is True:
            target.rotation[2] = math.radians(rot_z)

    @staticmethod
    def change_location(target, change_x, change_y, change_z, min_x, max_x, min_y, max_y, min_z,
                        max_z):
        location = SceneEditing.get_random_loc(min_x, max_x, min_y, max_y, min_z, max_z)

        if change_x is True:
            target.location[0] = location[0]
        if change_y is True:
            target.location[1] = location[1]
        if change_z is True:
            target.location[2] = location[2]

    @staticmethod
    def get_random_loc(min_x, max_x, min_y, max_y, min_z, max_z):
        loc_x = random.uniform(min_x, max_x)
        loc_y = random.uniform(min_y, max_y)
        loc_z = random.uniform(min_z, max_z)

        return mathutils.Vector((loc_x, loc_y, loc_z))

    @staticmethod
    def get_random_rot(min_angle_x, max_angle_x,
                       min_angle_y, max_angle_y,
                       min_angle_z, max_angle_z):
        rot_x = math.radians(random.uniform(min_angle_x, max_angle_x))
        rot_y = math.radians(random.uniform(min_angle_y, max_angle_y))
        rot_z = math.radians(random.uniform(min_angle_z, max_angle_z))

        return mathutils.Euler((rot_x, rot_y, rot_z), "XYZ")


# ------------------------------------------------------------------------
#    Render Setup configures everything necessary to start the rendering
#    It relies heavily on the injected state and scene setup
# ------------------------------------------------------------------------


class RenderSetup:
    def __init__(self, scene_setup):
        # Use local state since UI may change values
        self.scene_setup = scene_setup
        self.state = scene_setup.state

    # def pre_render(self, context):
    def pre_render(self):

        self.state.running = True

        self.scene_setup.set_settings()
        self.scene_setup.apply_settings()

        self.state.start_time = time.time()
        self.scene_setup.create_and_set_folders()

        self.scene_setup.create_checkpoint()
        self.scene_setup.configure_scene()

    @staticmethod
    def render():
        # Dirty hack
        # INVOKE_DEFAULT is used to set the execution context.
        # It allows to display the rendering progress and keeps
        # the UI responsive. Without it the UI locks.
        # Consecutive calls of bpy.ops.render.render() work fine,
        # but with the execution context it sometimes cancels.
        #
        # The quick and dirty solution is to retry it at a later
        # point in time, until it succeeds.
        #
        # The cause for these random failures are currently
        # unknown. It seems that there are still internal steps
        # running even when render_post or render_complete handlers
        # are triggered. Calling the function again could then interfer
        # with the running rendering instance. However that is only
        # speculation and needs further investigation.
        #
        # For a cleaner solution, but non-responsive UI uncomment
        # the following line and comment the other section
        #
        bpy.ops.render.render()

        # repeat = True
        # repeat_max = 10
        # repeat_timeout = 0.1
        # exec_context= "INVOKE_DEFAULT"
        # while repeat:
        #    retVal = bpy.ops.render.render(exec_context)
        #    if (retVal != {'CANCELLED'}):
        #        repeat = False
        #    else:
        #        print("Rendering attempt failed, trying to repeat with alternative mode.")
        #        exec_context = "EXEC_DEFAULT"
        #        time.sleep(repeat_timeout)
        #        repeat_max -= 1
        #        repeat_timeout += 0.1
        #        if(repeat_max == 0):
        #            self._clean_up()
        #            raise RuntimeError("Could not execute rendering function.")

    def post_render(self, context):
        try:
            self.scene_setup.rename_files()
        except RuntimeError as e:
            print(str(e))
        self.scene_setup.increment_current_frame_from_state()
        self.state.end_time = time.time()
        self._calculate_statistics()
        IO.print_statistics(self.state.elapsed_time, self.state.estimated_exec_time)

        self._clean_up()

    def cancel_render(self, context):
        self._clean_up()
        self.state.cancelled = True

    def _clean_up(self):
        keyframes_to_remove = self.state.used_keyframes
        self.scene_setup.restore_checkpoint(keyframes_to_remove)
        self.state.running = False

    def _calculate_statistics(self):
        self.state.elapsed_time = self.state.end_time - self.state.start_time

        if self.state.mean_exec_time_per_frame > 0:
            self.state.mean_exec_time_per_frame = (
                                                              self.state.mean_exec_time_per_frame + self.state.elapsed_time) / 2
        else:
            self.state.mean_exec_time_per_frame = self.state.elapsed_time

        self.state.estimated_exec_time = (self.state.mean_exec_time_per_frame *
                                          (self.state.total_frames - self.state.current_frame))


# ------------------------------------------------------------------------
#    Scene Setup provides a basic recipe for scene configuration
# ------------------------------------------------------------------------


class SceneSetup(ABC):
    def __init__(self, state):
        print("DEBUG: init for SceneSetup")
        # Use local state since UI may change values
        self.state = state
        self.scene = bpy.context.scene

    def apply_settings(self):
        print("DEBUG: apply_settings for SceneSetup")
        self.state.apply_scene_settings(self.scene)

    def create_and_set_folders(self):
        list_of_paths = [self.state.base_path,
                         self.state.path_ball,
                         self.state.path_ball_bw,
                         self.state.path_no_ball,
                         self.state.path_no_ball_bw,
                         self.state.path_img,
                         self.state.path_img_bw,
                         self.state.path_mask]

        IO.mkdirs(list_of_paths)
        SceneEditing.set_file_output_nodes(self.state.use_mask_output)
        self._set_current_paths()

    def create_checkpoint(self):
        self.scene.frame_set(0)
        SceneEditing.add_keyframes_to_objs_for(0)
        bpy.data.worlds['World'].node_tree.nodes["Mapping"].keyframe_insert(data_path="rotation",
                                                                            frame=0)

    def configure_scene(self):
        self.scene.frame_set(2)
        self._calculate_zones()
        self._config_anchor()
        self._config_cameras_and_balls()
        self._config_naos()
        self._config_world()
        SceneEditing.add_keyframes_to_objs_for(1, self.state.used_keyframes)
        SceneEditing.add_keyframes_to_objs_for(2, self.state.used_keyframes)
        bpy.data.worlds['World'].node_tree.nodes["Mapping"].keyframe_insert(data_path="rotation",
                                                                            frame=2)
        self._config_motion_blur()

    def rename_files(self):
        old_name = str(self.state.current_frame) + str(self.scene.frame_current) + ".png"
        new_name = str(self.state.current_frame) + ".png"

        if self.state.current_path is not None:
            IO.rename_file(self.state.current_path, old_name, new_name)
        if self.state.current_path_bw is not None:
            IO.rename_file(self.state.current_path_bw, old_name, new_name)
        if self.state.current_path_mask is not None:
            IO.rename_file(self.state.current_path_mask, old_name, new_name)

    def increment_current_frame_from_state(self):
        self.state.current_frame += 1
        IO.print_frame_counter(self.state.current_frame, self.state.total_frames)

    def restore_checkpoint(self, keyframes_to_remove):
        for frame_x in keyframes_to_remove:
            SceneEditing.remove_keyframes_from_objs_for(frame_x)
        bpy.data.worlds['World'].node_tree.nodes["Mapping"].keyframe_delete(data_path="rotation",
                                                                            frame=2)
        self.scene.frame_set(0)

    @abstractmethod
    def set_settings(self):
        pass

    @abstractmethod
    def _set_current_paths(self):
        pass

    def _calculate_zones(self):
        self.state.allowed_zone_co = SceneEditing.get_vertices_world_space("allowed_zone_patches")
        self.state.field_co = SceneEditing.get_vertices_world_space("field")

    @abstractmethod
    def _config_anchor(self):
        pass

    @abstractmethod
    def _config_cameras_and_balls(self):
        pass

    @abstractmethod
    def _config_naos(self):
        pass

    @staticmethod
    def _config_world():
        env_map = bpy.data.worlds['World'].node_tree.nodes["Mapping"]
        SceneEditing.change_rotation(env_map, True, True, True, 0.0, 360.0, 0.0, 360.0, 0.0, 360.0)

    def _config_motion_blur(self):
        roll_dice = random.randint(0, self.state.motion_blur_every_1_out_of_x)

        if (roll_dice == 0) and (self.state.use_motion_blur is True):
            self.scene.render.use_motion_blur = True
            self.scene.frame_set(1)
            SceneEditing.change_location(bpy.data.objects["ball_top"], True, True, False, 0.0,
                                         0.05, 0.0, 0.05, 0.0,
                                         0.0)
            SceneEditing.change_location(bpy.data.objects["ball_bottom"], True, True, False, 0.0,
                                         0.05, 0.0, 0.05, 0.0,
                                         0.0)
            SceneEditing.change_rotation_euler(bpy.data.objects["ball_top"], True, True, True, 0.0,
                                               360.0, 0.0, 360.0,
                                               0.0, 360.0)
            SceneEditing.change_rotation_euler(bpy.data.objects["ball_bottom"], True, True, True,
                                               0.0, 360.0, 0.0,
                                               360.0, 0.0, 360.0)
            SceneEditing.add_keyframes_for(bpy.data.objects["ball_top"], 1,
                                           self.state.used_keyframes)
            SceneEditing.add_keyframes_for(bpy.data.objects["ball_bottom"], 1,
                                           self.state.used_keyframes)

            self.scene.frame_set(3)
            SceneEditing.change_location(bpy.data.objects["ball_top"], True, True, False, 0.0,
                                         0.05, 0.0, 0.05, 0.0,
                                         0.0)
            SceneEditing.change_location(bpy.data.objects["ball_bottom"], True, True, False, 0.0,
                                         0.05, 0.0, 0.05, 0.0,
                                         0.0)
            SceneEditing.change_rotation_euler(bpy.data.objects["ball_top"], True, True, True, 0.0,
                                               360.0, 0.0, 360.0,
                                               0.0, 360.0)
            SceneEditing.change_rotation_euler(bpy.data.objects["ball_bottom"], True, True, True,
                                               0.0, 360.0, 0.0,
                                               360.0, 0.0, 360.0)
            SceneEditing.add_keyframes_for(bpy.data.objects["ball_top"], 3,
                                           self.state.used_keyframes)
            SceneEditing.add_keyframes_for(bpy.data.objects["ball_bottom"], 3,
                                           self.state.used_keyframes)
        else:
            self.scene.render.use_motion_blur = False

        self.scene.frame_set(2)


# ------------------------------------------------------------------------
#    Specialized scene setup for rendering patches of ball/no ball
# ------------------------------------------------------------------------


class SceneSetupPatch(SceneSetup):
    def __init__(self, state):
        super().__init__(state)

    def set_settings(self):
        if self.state.is_set is True:
            return

        self.state.engine = 'CYCLES'
        self.state.use_persistent_data = True
        self.state.motion_blur_shutter = 0.1
        self.state.use_border = True
        self.state.use_crop_to_border = True

        self.state.border_min_x = 0.420
        self.state.border_min_y = 0.420
        self.state.border_max_x = 1 - self.state.border_min_x
        self.state.border_max_y = 1 - self.state.border_min_y
        self.state.use_mask_output = False

        self.state.motion_blur_every_1_out_of_x = 4
        self.state.advanced_image = False

        self.state.base_path = os.path.join(self.state.path,
                                            datetime.datetime.now().strftime(
                                                '%Y%m%d_%H%M%S') + "_patch")
        self.state.path_ball = os.path.join(self.state.base_path, "ball")
        self.state.path_ball_bw = os.path.join(self.state.base_path, "ball_bw")
        self.state.path_no_ball = os.path.join(self.state.base_path, "no_ball")
        self.state.path_no_ball_bw = os.path.join(self.state.base_path, "no_ball_bw")
        self.state.path_img = None
        self.state.path_img_bw = None
        self.state.path_mask = None

        self.state.old_res_x = self.scene.render.resolution_x
        self.state.old_res_y = self.scene.render.resolution_y
        self.state.res_x = round(self.state.patch_size / (1.0 - self.state.border_min_x * 2.0))
        self.state.res_y = round(self.state.patch_size / (1.0 - self.state.border_min_y * 2.0))

        self.state.current_path = None
        self.state.current_path_bw = None
        self.state.current_path_mask = None
        self.state.current_frame = 0
        self.state.total_frames = self.state.num_balls + self.state.num_no_balls

        self.state.start_time = 0
        self.state.end_time = 0
        self.state.elapsed_time = 0

        self.state.estimated_exec_time = 0
        self.state.mean_exec_time_per_frame = 0

        self.state.used_keyframes = set()

        self.scene.frame_set(0)
        self.state.armature_dummy_loc_x = bpy.data.objects["armature_camera_dummy"].location[0]
        self.state.armature_dummy_loc_y = bpy.data.objects["armature_camera_dummy"].location[1]
        self.state.armature_dummy_loc_z = bpy.data.objects["armature_camera_dummy"].location[2]

        self.state.is_set = True

    def _set_current_paths(self):
        if self.state.current_frame < self.state.num_balls:
            self.state.current_path = self.state.path_ball
            self.state.current_path_bw = self.state.path_ball_bw
        else:
            self.state.current_path = self.state.path_no_ball
            self.state.current_path_bw = self.state.path_no_ball_bw

        self.scene.node_tree.nodes["file_output_col"].base_path = self.state.current_path
        self.scene.node_tree.nodes["file_output_col"].file_slots[0].path = str(
            self.state.current_frame) + "#"
        self.scene.node_tree.nodes["file_output_bw"].base_path = self.state.current_path_bw
        self.scene.node_tree.nodes["file_output_bw"].file_slots[0].path = str(
            self.state.current_frame) + "#"

    def _config_anchor(self):
        anchor = bpy.data.objects["anchor_main"]
        SceneEditing.change_location(anchor, True, True, False,
                                     self.state.allowed_zone_co[0][0],
                                     self.state.allowed_zone_co[3][0],
                                     self.state.allowed_zone_co[0][1],
                                     self.state.allowed_zone_co[3][1],
                                     0, 0)
        SceneEditing.change_rotation_euler(anchor, False, False, True, 0.0, 0.0, 0.0, 0.0, 0.0,
                                           360.0)

    def _config_cameras_and_balls(self):
        select_camera = random.randint(0, 1)

        if select_camera == 0:
            self.scene.camera = bpy.data.objects["camera_top"]
        else:
            self.scene.camera = bpy.data.objects["camera_bottom"]

        if self.state.current_frame >= self.state.num_balls:
            bpy.data.objects["ball_top"].hide_render = True
            bpy.data.objects["ball_bottom"].hide_render = True
        else:
            bpy.data.objects["ball_top"].hide_render = False
            bpy.data.objects["ball_bottom"].hide_render = False
            SceneEditing.change_location(bpy.data.objects["ball_top"], True, True, False, 0.0,
                                         0.05, 0.0, 0.05, 0.0,
                                         0.0)
            SceneEditing.change_location(bpy.data.objects["ball_bottom"], True, True, False, 0.0,
                                         0.05, 0.0, 0.05, 0.0,
                                         0.0)
            SceneEditing.change_rotation_euler(bpy.data.objects["ball_top"], True, True, True, 0.0,
                                               360.0, 0.0, 360.0,
                                               0.0, 360.0)
            SceneEditing.change_rotation_euler(bpy.data.objects["ball_bottom"], True, True, True,
                                               0.0, 360.0, 0.0,
                                               360.0, 0.0, 360.0)

    def _config_naos(self):
        loc_ball = None
        loc_camera = None

        if self.scene.camera.name == "camera_top":
            loc_ball = bpy.data.objects["ball_top"].matrix_world.to_translation()
            loc_camera = bpy.data.objects["camera_top"].matrix_world.to_translation()
        else:
            loc_ball = bpy.data.objects["ball_bottom"].matrix_world.to_translation()
            loc_camera = bpy.data.objects["camera_bottom"].matrix_world.to_translation()

        point_x = loc_ball[0] - loc_camera[0]
        point_y = loc_ball[1] - loc_camera[1]

        rotated_point = SceneEditing.get_point_rotated(point_x, point_y, loc_camera[0],
                                                       loc_camera[1], 180.0)
        limit_co = self.state.field_co

        for target in self.scene.objects:
            if target.type == "ARMATURE" and target.name != "armature_camera_dummy":
                loc_armature = SceneEditing.get_random_loc(limit_co[0][0], limit_co[3][0],
                                                           limit_co[0][1], limit_co[3][1],
                                                           0.0, 0.0)

                if point_x < rotated_point[0]:
                    if point_x <= loc_armature[0] <= rotated_point[0] and point_y <= loc_armature[
                        1] <= rotated_point[1]:
                        loc_armature[0] += rotated_point[0]
                else:
                    if rotated_point[0] <= loc_armature[0] <= point_x and rotated_point[1] <= \
                            loc_armature[1] <= point_y:
                        loc_armature[0] += point_x

                target.location[0] = loc_armature[0]
                target.location[1] = loc_armature[1]
                SceneEditing.change_rotation_euler(target, False, False, True, 0.0, 0.0, 0.0, 0.0,
                                                   0.0, 360.0)

    def rename_files(self):
        if self.state.current_frame < self.state.num_balls:
            frame = self.state.current_frame
        else:
            frame = self.state.current_frame - self.state.num_balls

        old_name = str(self.state.current_frame) + str(self.scene.frame_current) + ".png"
        new_name = str(frame) + ".png"

        if self.state.current_path is not None:
            IO.rename_file(self.state.current_path, old_name, new_name)
        if self.state.current_path_bw is not None:
            IO.rename_file(self.state.current_path_bw, old_name, new_name)
        if self.state.current_path_mask is not None:
            IO.rename_file(self.state.current_path_mask, old_name, new_name)


# ------------------------------------------------------------------------
#    Specialized scene setup for rendering patches and ball masks
# ------------------------------------------------------------------------
class SceneSetupPatchMask(SceneSetup):
    def __init__(self, state):
        super().__init__(state)

    def set_settings(self):
        print("DEBUG: set settings for SceneSetupPatchMask")
        if self.state.is_set is True:
            return

        self.state.engine = 'CYCLES'
        self.state.use_persistent_data = True
        self.state.motion_blur_shutter = 0.1
        self.state.use_border = True
        self.state.use_crop_to_border = True

        self.state.border_min_x = 0.420
        self.state.border_min_y = 0.420
        self.state.border_max_x = 1 - self.state.border_min_x
        self.state.border_max_y = 1 - self.state.border_min_y
        self.state.use_mask_output = True

        self.state.motion_blur_every_1_out_of_x = 4
        self.state.advanced_image = False

        self.state.base_path = os.path.join(self.state.path,
                                            datetime.datetime.now().strftime(
                                                '%Y%m%d_%H%M%S') + "_patchMask")

        if self.state.generate_no_ball:
            self.state.path_noball_patch = os.path.join(self.state.base_path, "noball_patch")
            self.state.path_noball_patch_bw = os.path.join(self.state.base_path, "noball_patch_bw")
            self.state.path_noball_patch_mask = os.path.join(self.state.base_path,
                                                             "noball_patch_mask")

        self.state.path_ball_patch = os.path.join(self.state.base_path, "ball_patch")
        self.state.path_ball_patch_bw = os.path.join(self.state.base_path, "ball_patch_bw")
        self.state.path_ball_patch_mask = os.path.join(self.state.base_path, "ball_patch_mask")

        self.state.old_res_x = self.scene.render.resolution_x
        self.state.old_res_y = self.scene.render.resolution_y
        self.state.res_x = round(self.state.patch_size / (1.0 - self.state.border_min_x * 2.0))
        self.state.res_y = round(self.state.patch_size / (1.0 - self.state.border_min_y * 2.0))

        self.state.current_path = None
        self.state.current_path_bw = None
        self.state.current_path_mask = None
        self.state.current_frame = 0
        if self.state.generate_no_ball:
            self.state.total_frames = self.state.num_images * 2
        else:
            self.state.total_frames = self.state.num_images

        self.state.start_time = 0
        self.state.end_time = 0
        self.state.elapsed_time = 0

        self.state.estimated_exec_time = 0
        self.state.mean_exec_time_per_frame = 0

        self.state.used_keyframes = set()

        self.scene.frame_set(0)
        self.state.armature_dummy_loc_x = bpy.data.objects["armature_camera_dummy"].location[0]
        self.state.armature_dummy_loc_y = bpy.data.objects["armature_camera_dummy"].location[1]
        self.state.armature_dummy_loc_z = bpy.data.objects["armature_camera_dummy"].location[2]

        self.state.is_set = True

    def _set_current_paths(self):
        if self.state.current_frame < self.state.num_images:
            self.state.current_path = self.state.path_ball_patch
            self.state.current_path_bw = self.state.path_ball_patch_bw
            self.state.current_path_mask = self.state.path_ball_patch_mask
        else:
            self.state.current_path = self.state.path_noball_patch
            self.state.current_path_bw = self.state.path_noball_patch_bw
            self.state.current_path_mask = self.state.path_noball_patch_mask

        self.scene.node_tree.nodes["file_output_col"].base_path = self.state.current_path
        self.scene.node_tree.nodes["file_output_col"].file_slots[0].path = str(
            self.state.current_frame) + "#"
        self.scene.node_tree.nodes["file_output_bw"].base_path = self.state.current_path_bw
        self.scene.node_tree.nodes["file_output_bw"].file_slots[0].path = str(
            self.state.current_frame) + "#"
        self.scene.node_tree.nodes["file_output_mask"].base_path = self.state.current_path_mask
        self.scene.node_tree.nodes["file_output_mask"].file_slots[0].path = str(
            self.state.current_frame) + "#"

    def _config_anchor(self):
        anchor = bpy.data.objects["anchor_main"]
        SceneEditing.change_location(anchor, True, True, False,
                                     self.state.field_co[0][0], self.state.field_co[3][0],
                                     self.state.field_co[0][1], self.state.field_co[3][1],
                                     0, 0)
        SceneEditing.change_rotation_euler(anchor, False, False, True, 0.0, 0.0, 0.0, 0.0, 0.0,
                                           360.0)

    def _config_cameras_and_balls(self):
        select_camera = random.randint(0, 1)

        if select_camera == 0:
            self.scene.camera = bpy.data.objects["camera_top"]
        else:
            self.scene.camera = bpy.data.objects["camera_bottom"]

        if self.state.current_frame >= self.state.num_images:
            bpy.data.objects["ball_top"].hide_render = True
            bpy.data.objects["ball_bottom"].hide_render = True
        else:
            bpy.data.objects["ball_top"].hide_render = False
            bpy.data.objects["ball_bottom"].hide_render = False
            SceneEditing.change_location(bpy.data.objects["ball_top"], True, True, False, 0.0,
                                         0.05, 0.0, 0.05, 0.0,
                                         0.0)
            SceneEditing.change_location(bpy.data.objects["ball_bottom"], True, True, False, 0.0,
                                         0.05, 0.0, 0.05, 0.0,
                                         0.0)
            SceneEditing.change_rotation_euler(bpy.data.objects["ball_top"], True, True, True, 0.0,
                                               360.0, 0.0, 360.0,
                                               0.0, 360.0)
            SceneEditing.change_rotation_euler(bpy.data.objects["ball_bottom"], True, True, True,
                                               0.0, 360.0, 0.0,
                                               360.0, 0.0, 360.0)

    def _config_naos(self):
        for target in self.scene.objects:
            if target.type == "ARMATURE" and target.name != "armature_camera_dummy":
                loc_armature = SceneEditing.get_random_loc(self.state.field_co[0][0],
                                                           self.state.field_co[3][0],
                                                           self.state.field_co[0][1],
                                                           self.state.field_co[3][1],
                                                           0.0, 0.0)

                target.location[0] = loc_armature[0]
                target.location[1] = loc_armature[1]
                SceneEditing.change_rotation_euler(target, False, False, True, 0.0, 0.0, 0.0, 0.0,
                                                   0.0, 360.0)

    def rename_files(self):
        print("DEBUG: rename_files for SceneSetupPatchMask")
        if self.state.current_frame < self.state.num_images:
            frame = self.state.current_frame
        else:
            frame = self.state.current_frame - self.state.num_images

        old_name = str(self.state.current_frame) + str(self.scene.frame_current) + ".png"

        # get the camera id
        camID = str(self.scene.camera)
        if "top" in camID:
            new_name = str(frame) + "_top" + ".png"
        elif "bottom" in camID:
            new_name = str(frame) + "_bottom" + ".png"

        if self.state.current_path is not None:
            IO.rename_file(self.state.current_path, old_name, new_name)
        if self.state.current_path_bw is not None:
            IO.rename_file(self.state.current_path_bw, old_name, new_name)
        if self.state.current_path_mask is not None:
            IO.rename_file(self.state.current_path_mask, old_name, new_name)


# ------------------------------------------------------------------------
#    Specialized scene setup for rendering images and ball masks
# ------------------------------------------------------------------------


class SceneSetupImageMask(SceneSetup):
    def __init__(self, state):
        super().__init__(state)

    def set_settings(self):
        if self.state.is_set is True:
            return

        self.state.engine = 'CYCLES'
        self.state.use_persistent_data = True
        self.state.motion_blur_shutter = 0.5
        self.state.use_border = False
        self.state.use_crop_to_border = False

        self.state.border_min_x = 0.380
        self.state.border_min_y = 0.380
        self.state.border_max_x = 1 - self.state.border_min_x
        self.state.border_max_y = 1 - self.state.border_min_y
        self.state.use_mask_output = True

        self.state.motion_blur_every_1_out_of_x = 4
        self.state.advanced_image = True

        self.state.base_path = os.path.join(self.state.path,
                                            datetime.datetime.now().strftime(
                                                '%Y%m%d_%H%M%S') + "_imageMask")
        self.state.path_ball = None
        self.state.path_ball_bw = None
        self.state.path_no_ball = None
        self.state.path_no_ball_bw = None
        self.state.path_img = os.path.join(self.state.base_path, "img")
        self.state.path_img_bw = os.path.join(self.state.base_path, "img_bw")
        self.state.path_mask = os.path.join(self.state.base_path, "mask")

        self.state.old_res_x = self.scene.render.resolution_x
        self.state.old_res_y = self.scene.render.resolution_y
        self.state.res_x = round(1280 * self.state.res_factor)
        self.state.res_y = round(960 * self.state.res_factor)

        self.state.current_path = None
        self.state.current_path_bw = None
        self.state.current_path_mask = None
        self.state.current_frame = 0
        self.state.total_frames = self.state.num_images

        self.state.start_time = 0
        self.state.end_time = 0
        self.state.elapsed_time = 0

        self.state.estimated_exec_time = 0
        self.state.mean_exec_time_per_frame = 0

        self.state.used_keyframes = set()

        self.scene.frame_set(0)
        self.state.armature_dummy_loc_x = bpy.data.objects["armature_camera_dummy"].location[0]
        self.state.armature_dummy_loc_y = bpy.data.objects["armature_camera_dummy"].location[1]
        self.state.armature_dummy_loc_z = bpy.data.objects["armature_camera_dummy"].location[2]

        self.state.is_set = True

    def _set_current_paths(self):
        self.state.current_path = self.state.path_img
        self.state.current_path_bw = self.state.path_img_bw
        self.state.current_path_mask = self.state.path_mask

        self.scene.node_tree.nodes["file_output_col"].base_path = self.state.current_path
        self.scene.node_tree.nodes["file_output_col"].file_slots[0].path = str(
            self.state.current_frame) + "#"
        self.scene.node_tree.nodes["file_output_bw"].base_path = self.state.current_path_bw
        self.scene.node_tree.nodes["file_output_bw"].file_slots[0].path = str(
            self.state.current_frame) + "#"
        self.scene.node_tree.nodes["file_output_mask"].base_path = self.state.current_path_mask
        self.scene.node_tree.nodes["file_output_mask"].file_slots[0].path = str(
            self.state.current_frame) + "#"

    def _config_anchor(self):
        anchor = bpy.data.objects["anchor_main"]
        SceneEditing.change_location(anchor, True, True, False,
                                     self.state.field_co[0][0], self.state.field_co[3][0],
                                     self.state.field_co[0][1], self.state.field_co[3][1],
                                     0, 0)
        SceneEditing.change_rotation_euler(anchor, False, False, True, 0.0, 0.0, 0.0, 0.0, 0.0,
                                           360.0)

    def _config_cameras_and_balls(self):
        select_camera = random.randint(0, 1)

        if select_camera == 0:
            self.scene.camera = bpy.data.objects["camera_top"]
        else:
            self.scene.camera = bpy.data.objects["camera_bottom"]

        hide_ball = random.randint(0, 1)

        if (hide_ball == 1) and (self.state.advanced_image is False):
            bpy.data.objects["ball_top"].hide_render = True
            bpy.data.objects["ball_bottom"].hide_render = True
        else:
            bpy.data.objects["ball_top"].hide_render = False
            bpy.data.objects["ball_bottom"].hide_render = False

        if self.state.advanced_image is False:
            SceneEditing.change_location(bpy.data.objects["ball_top"], True, True, False, 0.0,
                                         0.05, 0.0, 0.05, 0.0,
                                         0.0)
            SceneEditing.change_location(bpy.data.objects["ball_bottom"], True, True, False, 0.0,
                                         0.05, 0.0, 0.05, 0.0,
                                         0.0)
        else:
            choose_ball = random.randint(0, 1)
            choose_mode = random.randint(0, 1)

            if choose_ball == 0:
                bpy.data.objects["ball_top"].hide_render = False
                bpy.data.objects["ball_bottom"].hide_render = True
            else:
                bpy.data.objects["ball_top"].hide_render = True
                bpy.data.objects["ball_bottom"].hide_render = False

            if select_camera == 0 and choose_ball == 0 and choose_mode == 0:
                x_rot = random.uniform(65.0, 117.0)
                z_rot = random.uniform(-114.0, -66.0)
                bpy.data.objects["camera_top"].rotation_euler[0] = math.radians(x_rot)
                bpy.data.objects["camera_top"].rotation_euler[2] = math.radians(z_rot)
                bpy.data.objects["armature_camera_dummy"].rotation_euler[2] = math.radians(
                    z_rot) + math.radians(90.0)

            elif select_camera == 0 and choose_ball == 0 and choose_mode == 1:
                x_loc = random.uniform(-1.75, -0.42)
                z_rot = random.uniform(-115.0, -65.0)
                bpy.data.objects["camera_top"].location[0] = x_loc
                bpy.data.objects["camera_top"].rotation_euler[2] = math.radians(z_rot)
                bpy.data.objects["armature_camera_dummy"].location[
                    0] = x_loc + self.state.armature_dummy_loc_x
                bpy.data.objects["armature_camera_dummy"].rotation_euler[2] = math.radians(
                    z_rot) + math.radians(90.0)

            elif select_camera == 0 and choose_ball == 1 and choose_mode == 0:
                x_rot = random.uniform(35.0, 70.0)
                z_rot = random.uniform(-120.0, -40.0)
                bpy.data.objects["camera_top"].rotation_euler[0] = math.radians(x_rot)
                bpy.data.objects["camera_top"].rotation_euler[2] = math.radians(z_rot)
                bpy.data.objects["armature_camera_dummy"].rotation_euler[2] = math.radians(
                    z_rot) + math.radians(90.0)

            elif select_camera == 0 and choose_ball == 1 and choose_mode == 1:
                x_loc = random.uniform(-1.75, -0.42)
                z_rot = random.uniform(-115.0, -65.0)
                bpy.data.objects["camera_top"].location[0] = x_loc
                bpy.data.objects["camera_top"].rotation_euler[2] = math.radians(z_rot)
                bpy.data.objects["armature_camera_dummy"].location[
                    0] = x_loc + self.state.armature_dummy_loc_x
                bpy.data.objects["armature_camera_dummy"].rotation_euler[2] = math.radians(
                    z_rot) + math.radians(90.0)

            elif select_camera == 1 and choose_ball == 0:
                x_rot = random.uniform(68.0, 120.0)
                z_rot = random.uniform(-128.0, -52.0)
                bpy.data.objects["camera_bottom"].rotation_euler[0] = math.radians(x_rot)
                bpy.data.objects["camera_bottom"].rotation_euler[2] = math.radians(z_rot)
                bpy.data.objects["armature_camera_dummy"].rotation_euler[2] = math.radians(
                    z_rot) + math.radians(90.0)

            elif select_camera == 1 and choose_ball == 1:
                x_loc = random.uniform(-2.0, 0.3)
                z_rot = random.uniform(-115.0, -65.0)
                bpy.data.objects["camera_bottom"].location[0] = x_loc
                bpy.data.objects["camera_bottom"].rotation_euler[2] = math.radians(z_rot)
                bpy.data.objects["armature_camera_dummy"].location[
                    0] = x_loc + self.state.armature_dummy_loc_x
                bpy.data.objects["armature_camera_dummy"].rotation_euler[2] = math.radians(
                    z_rot) + math.radians(90.0)

        SceneEditing.change_rotation_euler(bpy.data.objects["ball_top"], True, True, True, 0.0,
                                           360.0, 0.0, 360.0, 0.0,
                                           360.0)
        SceneEditing.change_rotation_euler(bpy.data.objects["ball_bottom"], True, True, True, 0.0,
                                           360.0, 0.0, 360.0,
                                           0.0, 360.0)

    def _config_naos(self):
        for target in self.scene.objects:
            if target.type == "ARMATURE" and target.name != "armature_camera_dummy":
                loc_armature = SceneEditing.get_random_loc(self.state.field_co[0][0],
                                                           self.state.field_co[3][0],
                                                           self.state.field_co[0][1],
                                                           self.state.field_co[3][1],
                                                           0.0, 0.0)

                target.location[0] = loc_armature[0]
                target.location[1] = loc_armature[1]
                SceneEditing.change_rotation_euler(target, False, False, True, 0.0, 0.0, 0.0, 0.0,
                                                   0.0, 360.0)

    def rename_files(self):
        old_name = str(self.state.current_frame) + str(self.scene.frame_current) + ".png"
        new_name = str(self.state.current_frame) + ".png"

        if self.state.current_path is not None:
            IO.rename_file(self.state.current_path, old_name, new_name)
        if self.state.current_path_bw is not None:
            IO.rename_file(self.state.current_path_bw, old_name, new_name)
        if self.state.current_path_mask is not None:
            IO.rename_file(self.state.current_path_mask, old_name, new_name)


# ------------------------------------------------------------------------
#    Start Rendering starts the modal rendering operator
# ------------------------------------------------------------------------


class StartRendering(bpy.types.Operator):
    bl_idname = "wm.start_rendering"
    bl_label = "Start Rendering"

    def execute(self, context):

        if UISettings.is_modal_instance_running is False:
            print("-------------------------------------------------------")
            print(" Automated Rendering for Deep Learning Data Generation ")
            print("-------------------------------------------------------")
            UISettings.is_modal_instance_running = True
            ManageRender.addon_settings = context.scene.addon_settings
            bpy.ops.render.manage()
        else:
            self.report({"WARNING"}, "Program is already running, to abort press 'ESC'")

        return {'FINISHED'}


# ------------------------------------------------------------------------
#    Manage Render is a modal operator class which doesn't lock the UI
# ------------------------------------------------------------------------
class ManageRender(bpy.types.Operator):
    bl_idname = "render.manage"
    bl_label = "Manage automated rendering"

    state = None
    scene_setup = None
    render_setup = None
    addon_settings = None

    timer = None

    foo = BoolProperty()

    def execute(self, context):

        if (self.state is not None) and (self.state.running is True):
            print("Instance already running")
            return {"RUNNING_MODAL"}

        self.state = State(self.addon_settings)

        if self.foo is True:
            print("Use custom arguments:")
            self.state.op_mode = TestArguments.op_mode
            self.state.path = TestArguments.path
            self.state.patch_size = TestArguments.patch_size
            self.state.num_images = TestArguments.num_images
            self.state.generate_no_ball = TestArguments.generate_no_ball
            self.state.use_motion_blur = True
            print("self.state.op_mode", self.state.op_mode)
            print("self.state.path", self.state.path)
            print("self.state.patch_size", self.state.patch_size)
            print("self.state.num_images", self.state.num_images)
            print("self.state.generate_no_ball", self.state.generate_no_ball)

        if self.state.op_mode == "OP1":
            self.scene_setup = SceneSetupPatch(self.state)
        elif self.state.op_mode == "OP2":
            self.scene_setup = SceneSetupPatchMask(self.state)
        elif self.state.op_mode == "OP3":
            self.scene_setup = SceneSetupImageMask(self.state)

        self.render_setup = RenderSetup(self.scene_setup)

        bpy.app.handlers.render_post.append(self.render_setup.post_render)
        bpy.app.handlers.render_cancel.append(self.render_setup.cancel_render)

        self.timer = context.window_manager.event_timer_add(1.0, context.window)
        context.window_manager.modal_handler_add(self)

        return {"RUNNING_MODAL"}

    def modal(self, context, event):
        if event.type == 'TIMER':
            if ((self.state.cancelled is True) or
                    (
                            self.state.current_frame == self.state.total_frames and self.state.total_frames != 0)):
                bpy.app.handlers.render_post.remove(self.render_setup.post_render)
                bpy.app.handlers.render_cancel.remove(self.render_setup.cancel_render)
                context.window_manager.event_timer_remove(self.timer)

            if self.state.cancelled is True:
                self.state.cancelled = False
                UISettings.is_modal_instance_running = False
                print("Rendering cancelled.")
                self.report({"INFO"}, "Rendering cancelled.")
                return {"CANCELLED"}
            elif self.state.current_frame == self.state.total_frames and self.state.total_frames != 0:
                UISettings.is_modal_instance_running = False
                print("Rendering completed")
                self.report({"INFO"}, "Rendering completed")
                # HACK try to close blender - only needed until background option works
                # FIXME
                bpy.ops.wm.quit_blender()
                return {"FINISHED"}
            elif self.state.running is False:
                try:
                    self.render_setup.pre_render()
                    self.render_setup.render()
                except Exception as e:
                    # If something went really bad and wasn't caught,
                    # try to exit gracefully
                    print(str(e))
                    self.report({"ERROR"}, str(e))
                    self.state.cancelled = True

        return {"PASS_THROUGH"}


# ------------------------------------------------------------------------
#    Panel that contains all UI controls of this AddOn
# ------------------------------------------------------------------------


class UIAddonPanel(Panel):
    bl_idname = "UIAddonPanel"
    bl_label = "RoboCup Automated Rendering"
    bl_space_type = "VIEW_3D"
    bl_region_type = "TOOLS"
    bl_category = "Tools"
    bl_context = "objectmode"

    @classmethod
    def poll(cls, context):
        return context.object is not None

    def draw(self, context):
        layout = self.layout
        scene = context.scene
        addon_settings = scene.addon_settings

        layout.label("Operation mode")
        layout.prop(addon_settings, "op_mode", expand=True)
        layout.label("Output base path")
        layout.prop(addon_settings, "path", text="")

        if addon_settings.op_mode == "OP1":
            layout.prop(addon_settings, "patch_size")
            layout.prop(addon_settings, "num_balls")
            layout.prop(addon_settings, "num_no_balls")

        elif addon_settings.op_mode == "OP2":
            layout.prop(addon_settings, "patch_size")
            layout.prop(addon_settings, "num_images")
            layout.prop(addon_settings, 'generate_no_ball')

        elif addon_settings.op_mode == "OP3":
            layout.prop(addon_settings, "res_factor")
            layout.prop(addon_settings, "num_images")

        layout.prop(addon_settings, "use_motion_blur")

        layout.operator("wm.start_rendering")


# ------------------------------------------------------------------------
#    Register and unregister
# ------------------------------------------------------------------------
def register(arguments):
    bpy.utils.register_module(__name__)
    bpy.types.Scene.addon_settings = PointerProperty(type=UISettings)

    if arguments.cmd is True:
        UISettings.is_modal_instance_running = True
        # calls ManageRender execute
        bpy.ops.render.manage(foo=True)


def unregister():
    bpy.utils.unregister_module(__name__)
    del bpy.types.Scene.addon_settings


if __name__ == "__main__":
    # get the args passed to blender after "--", all of which are ignored by
    # blender so scripts may receive their own arguments
    argv = sys.argv

    if "--" not in argv:
        argv = []  # as if no args are passed
    else:
        argv = argv[argv.index("--") + 1:]  # get all args after "--"

    parser = argparse.ArgumentParser()

    # TODO hierarchically arguments
    parser.add_argument('-c', '--cmd', default=True, action='store_true',
                        help='run from command line')
    parser.add_argument('-m', '--op_mode', type=str, default="OP2", help='Render Mode')
    parser.add_argument('-p', '--path', type=str, default='.', help='Output Path')
    parser.add_argument('-ps', '--patch_size', type=int, default=16, help='Patch Size')
    parser.add_argument('-n', '--num_images', type=int, default=1, help='Number of images')
    parser.add_argument('-gnb', '--generate_no_ball', default=True, action='store_true',
                        help='Flag for generating masks for noball patches')

    args = parser.parse_args(argv)

    TestArguments.op_mode = args.op_mode
    TestArguments.path = str(Path(args.path).absolute())
    TestArguments.patch_size = args.patch_size
    TestArguments.num_images = args.num_images
    TestArguments.generate_no_ball = args.generate_no_ball

    register(args)
