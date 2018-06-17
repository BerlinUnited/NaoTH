import sys
from os import listdir
from os.path import isfile, join, splitext, isdir, exists
import cv2
import numpy as np
import operator
import os
import random
import string

# HACK: this parameter has different name in opencv3
MY_CV_LOAD_IMAGE_UNCHANGED = -1

# TODO: move changed images in seperate subfolder
'''
folder = "."
onlyfiles = [f for f in listdir(folder) if isfile(join(folder, f))]
png_files = []
for f in onlyfiles:
  if splitext(f)[-1] == '.png':
    png_files.append(f)
'''

def get_png_files_from_folder(folder):
  files = [os.path.join(root, name) for root, dirs, files in os.walk(folder, topdown=True) for name in files if splitext(name)[-1] == '.png']
  return files

def contains_files_for_classes(path, classes):
  splitted_path = path.split('/')
  splitted_path.extend(path.split('\\'))
  return any((True for x in classes if x in splitted_path))

def shuffle_files(files):
  random.shuffle(files)
  return files

def get_classes_files(files, classes):
  classes_files = {}
  for c in classes:
    classes_files[c] = shuffle_files([f for f in files if contains_files_for_classes(f, [c])])
  return classes_files

def copy_images(picture_files, options, dir_to_copy):
  if options['active']:
    for i, f in enumerate(picture_files):
      # Mirror Images
      img = cv2.imread(f, MY_CV_LOAD_IMAGE_UNCHANGED)
      path = splitext(f)[0]
      splitted_path = path.split('/')
      splitted_path.extend(path.split('\\'))
      filename_new = dir_to_copy + splitted_path[-2] + '_' + str(i) + '_original.png'
      cv2.imwrite(filename_new, img)

def motion_blur_images(picture_files, options, dir_to_copy):
  if options['active']:
    sizes = options['filter-sizes']
    for size in sizes:
      # generating the kernel
      kernel_motion_blur = np.zeros((size, size))
      kernel_motion_blur[int((size - 1) / 2), :] = np.ones(size)
      kernel_motion_blur = kernel_motion_blur / size
      for i, f in enumerate(picture_files):
        # Mirror Images
        img = cv2.imread(f, MY_CV_LOAD_IMAGE_UNCHANGED)
        path = splitext(f)[0]
        splitted_path = path.split('/')
        splitted_path.extend(path.split('\\'))

        # applying the kernel to the input image
        img = cv2.filter2D(img, -1, kernel_motion_blur)
        filename_new = dir_to_copy + splitted_path[-2] + '_' + str(i) + '_motion_blur_f' + str(size) + '.png'
        cv2.imwrite(filename_new, img)

def mirror_images(picture_files, options, dir_to_copy):
  mirrored_image_files = []
  if options['active']:
    for i, f in enumerate(picture_files):
      # Mirror Images
      img = cv2.imread(f, MY_CV_LOAD_IMAGE_UNCHANGED)
      img_flip = cv2.flip(img, 1)

      path = splitext(f)[0]
      splitted_path = path.split('/')
      splitted_path.extend(path.split('\\'))
      filename_new = dir_to_copy + splitted_path[-2] + '_' + str(i) + '_flipped.png'
      cv2.imwrite(filename_new,img_flip)
      mirrored_image_files.append(filename_new)
  return mirrored_image_files

def farball_images(picture_files, options, dir_to_copy):
  farball_image_files = []
  if options['active']:
    for i, f in enumerate(picture_files):
      # resize image to half size then use original size again
      img = cv2.imread(f, MY_CV_LOAD_IMAGE_UNCHANGED)
      img_small = cv2.resize(img, (0,0), fx=0.5, fy=0.5, interpolation=cv2.INTER_LINEAR) 
      img_resized = cv2.resize(img_small, (0,0), fx=2.0, fy=2.0, interpolation=cv2.INTER_NEAREST)

      path = splitext(f)[0]
      splitted_path = path.split('/')
      splitted_path.extend(path.split('\\'))
      filename_new = dir_to_copy + splitted_path[-2] + '_' + str(i) + '_far.png'
      cv2.imwrite(filename_new,img_resized)
      farball_image_files.append(filename_new)
  return farball_image_files

def rotate_images(picture_files, options, dir_to_copy):
  rotated_image_files = []
  if options['active']:
    rotations = options['rotations']
    for i, f in enumerate(picture_files):
      img = cv2.imread(f, MY_CV_LOAD_IMAGE_UNCHANGED)
      for r in rotations:
        rows, cols = img.shape
        M = cv2.getRotationMatrix2D((cols / 2, rows / 2), r, 1)
        im_rot = cv2.warpAffine(img, M, (cols, rows))

        path = splitext(f)[0]
        splitted_path = path.split('/')
        splitted_path.extend(path.split('\\'))
        filename_new = dir_to_copy + splitted_path[-2] + '_' + str(i) + '_rotated_with_' + str(r) + '.png'
        cv2.imwrite(filename_new, im_rot)
        rotated_image_files.append(filename_new)
  return rotated_image_files


def scale_brightness(picture_files, options, dir_to_copy):
  if options['active']:
    scales=options['scales']
    for i, f in enumerate(picture_files):
      img = cv2.imread(f, MY_CV_LOAD_IMAGE_UNCHANGED)
      # Scale Brightness
      for s in scales:
        # Assumes that images are in gray scale
        img_scale = img * s
        img_scale[img_scale > 254] = 254
        img_scale = img_scale.astype(np.uint8)

        path = splitext(f)[0]
        splitted_path = path.split('/')
        splitted_path.extend(path.split('\\'))
        filename_new = dir_to_copy + splitted_path[-2] + '_' + str(i) + '_scaled_with_' + str(s) + '.png'
        cv2.imwrite(filename_new, img_scale)

def shift_images(picture_files, options, dir_to_copy):
  if options['active']:
    shift_offsets = options['offsets']
    for i, f in enumerate(picture_files):
      img = cv2.imread(f, MY_CV_LOAD_IMAGE_UNCHANGED)
      for shift in shift_offsets:
        img_shift0 = img[shift:, shift:]
        img_shift1 = img[shift:, :-shift]
        img_shift2 = img[:-shift, :-shift]
        img_shift3 = img[:-shift, shift:]

        path = splitext(f)[0]
        splitted_path = path.split('/')
        splitted_path.extend(path.split('\\'))
        cv2.imwrite(dir_to_copy + splitted_path[-2] + '_' + str(i) + '_shiftversion_1_' + str(shift) + '.png',img_shift0)
        cv2.imwrite(dir_to_copy + splitted_path[-2] + '_' + str(i) + '_shiftversion_2_' + str(shift) + '.png',img_shift1)
        cv2.imwrite(dir_to_copy + splitted_path[-2] + '_' + str(i) + '_shiftversion_3_' + str(shift) + '.png',img_shift2)
        cv2.imwrite(dir_to_copy + splitted_path[-2] + '_' + str(i) + '_shiftversion_4_' + str(shift) + '.png',img_shift3)

def augment_files(src_root_folder, dir_to_copy, augment_options, amounts_per_class):
  classes = [c for c in amounts_per_class]
  png_files = get_png_files_from_folder(src_root_folder)
  files = get_classes_files(png_files, classes)

  for class_ in amounts_per_class:
    print str(amounts_per_class[class_]) + ' files for class ' + class_ + ' requested'
    if amounts_per_class[class_] > len(files[class_]):
      amounts_per_class[class_] = len(files[class_])
    print str(len(files[class_])) + ' files for class ' + class_ + 'found\n'
    directory = os.path.dirname(dir_to_copy + '/' + class_ + '/')
    try:
      os.stat(directory)
    except:
      os.makedirs(directory)

  if not exists(dir_to_copy) or not isdir(dir_to_copy):
    print "Output, directory does not exists: {0}".format(dir_to_copy)
    quit()

  for option in augment_options:
    if option == 'copy_original':
      for c in amounts_per_class:
        copy_images(files[c][0:amounts_per_class[c]], augment_options[option], dir_to_copy + '/' + c + '/')

    if option == 'motion_blur':
      for c in amounts_per_class:
        motion_blur_images(files[c][0:amounts_per_class[c]], augment_options[option], dir_to_copy + '/' + c + '/')

    if option == 'rotate':
      for c in amounts_per_class:
        rotated_files = rotate_images(files[c][0:amounts_per_class[c]], augment_options[option], dir_to_copy + '/' + c + '/')
        if rotated_files != []:
          scale_brightness(rotated_files, augment_options['brightness'], dir_to_copy + '/' + c + '/')
          shift_images(rotated_files, augment_options['shift'], dir_to_copy + '/' + c + '/')

    if option == 'mirror':
      for c in amounts_per_class:
        mirrored_files = mirror_images(files[c][0:amounts_per_class[c]], augment_options[option], dir_to_copy + '/' + c + '/')
        if mirrored_files != []:
          scale_brightness(mirrored_files, augment_options['brightness'], dir_to_copy + '/' + c + '/')
          shift_images(mirrored_files, augment_options['shift'], dir_to_copy + '/' + c + '/')

    if option == 'brightness':
      for c in amounts_per_class:
        scale_brightness(files[c][0:amounts_per_class[c]], augment_options[option], dir_to_copy + '/' + c + '/')

    if option == 'shift':
      for c in amounts_per_class:
        shift_images(files[c][0:amounts_per_class[c]], augment_options[option], dir_to_copy + '/' + c + '/')

    if option == 'farball':
      for c in amounts_per_class:
        farball_files = farball_images(files[c][0:amounts_per_class[c]], augment_options[option], dir_to_copy + '/' + c + '/')
        if farball_files != []:
          scale_brightness(farball_files, augment_options['brightness'], dir_to_copy + '/' + c + '/')
          shift_images(farball_files, augment_options['shift'], dir_to_copy + '/' + c + '/')

########################################################################################################################
#   Main
########################################################################################################################
if __name__ == '__main__':
  dir_from = '../data/basic/16x16'
  dir_to   = '../data/augmented/16x16/test1'
  
  if len(sys.argv) > 2:
	dir_from = sys.argv[1]
	dir_to = sys.argv[2]
	
  if not exists(dir_from) or not isdir(dir_from):
	print "Input, directory does not exists: {0}".format(dir_from)
	quit()

  # sample for augmenting the classes ball and noball differently
  # balls with rotated images + brightness scaling and noball only brightness scaling
  settings_ball = {'copy_original':{'active':True},
              'mirror':{'active':False},
              'rotate':{'active':True, 'rotations':[90, 180, 270]},
              'brightness':{'active':True, 'scales':[0.7, 1.3]},
              'shift':{'active':False, 'offsets':[1, 2]},
              'farball': {'active': True},
              'motion_blur':{'active':True, 'filter-sizes':[3, 5, 7]}
              }
  settings_noball = {'copy_original':{'active':True},
              'mirror':{'active':False},
              'rotate':{'active':False, 'rotations':[90, 180, 270]},
              'brightness':{'active':True, 'scales':[0.7, 1.3]},
              'shift':{'active':False, 'offsets':[1, 2]},
              'farball': {'active': True},
              'motion_blur':{'active':True, 'filter-sizes':[3, 5, 7]}
              }
  augment_files(dir_from, dir_to, settings_ball, {'ball':10})
  augment_files(dir_from, dir_to, settings_noball, {'noball': 40})
