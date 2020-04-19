from optparse import OptionParser
import os
import re
import sys
import random
import math
import numpy as np
import cv2

# This function is used for numerical sorting of file names (strings)
numbers = re.compile(r'(\d+)')


def numerical_sort(value):
    parts = numbers.split(value)
    parts[1::2] = map(int, parts[1::2])
    return parts


def update_progress(message, progress):
    sys.stdout.write('\r')
    sys.stdout.write(message + "[%-50s] %d%%" % ('=' * int(np.floor(0.5 * progress)), 1 * progress))
    sys.stdout.flush()


class DataSetExtractor:
    def __init__(self, path_to_images, path_to_ground_truth, patch_size, output_path):
        self.pathToImages = path_to_images
        self.pathToGroundTruth = path_to_ground_truth
        self.patchSize = patch_size
        self.output_path = output_path

        self.imageNames = sorted(self.__load_filenames(path_to_images, ".png"), key=numerical_sort)
        self.maskNames = sorted(self.__load_filenames(path_to_ground_truth, ".txt"), key=numerical_sort)
        self.legendFileName = self.__load_filenames(path_to_ground_truth, ".leg")

        self.labelDict = self.__load_label_config()
        self.legendDict = self.__read_legend_file()

        self.imageArray = None
        self.labelArray = None
        return

    #################################################
    # PUBLIC:
    #################################################
    def extract_dataset(self, is_patch_number_evening=False, rgb_format=False):
        """
        Extracts dataset using all images and masks available in the given directory.
        Use the "useOnePatchPerClass" variable if needed to even out the data sets per class
        distribution.
        """
        all_patch_array = []
        all_label_array = []
        num_images = len(self.imageNames)
        for i in range(num_images):
            curr_image_array, curr_label_array = self.extract_patches_for_single_image(i)
            for j in range(len(curr_image_array)):
                all_patch_array.append(curr_image_array[j])
                all_label_array.append(curr_label_array[j])
            progress = int(np.floor(i / float(num_images - 1) * 100))
            update_progress("Images processed: ", progress)

            # save images iteratively instead of putting every image im memory first
            if i % 100 == 0:
                # double the number of goalposts
                self.__do_vflip_for_image_with_label(all_patch_array, all_label_array, 3)

                # even the number of examples per class in the data set
                if is_patch_number_evening:
                    all_patch_array, all_label_array = self.__even_patch_numbers(all_patch_array, all_label_array)

                #
                all_patch_array, all_label_array = self.__resize_images(all_patch_array, all_label_array)

                # save images to directory
                dataSetExtractor.save_to_dir_structure(all_patch_array, all_label_array)

                # reset arrays for images
                all_patch_array = []
                all_label_array = []
        print("\n")

        # return all_patch_array, all_label_array

    def extract_patches_for_single_image(self, image_index):
        # get image and mask for given imageIndex
        curr_mask_name = self.maskNames[image_index]
        curr_image_name = self.imageNames[image_index]
        curr_image = cv2.imread(self.pathToImages + curr_image_name)

        # load mask into array structure
        curr_mask_array = self.__process_mask(curr_mask_name, curr_image.shape[0])
        curr_bounding_box_map = self.__extract_all_bounding_boxes(curr_mask_array)
        curr_bounding_box_map = self.__augment_bounding_boxes(curr_bounding_box_map)

        # generate image patches from currBoundingBoxMap
        curr_image_array, curr_label_array = self.__generate_image_crops(curr_image, curr_bounding_box_map)

        # generated random crop for "background"-class from image
        curr_random_background_patch, background_label = self.__get_random_background_patch(curr_image, curr_mask_array,
                                                                                            label=0)
        # append curr_random_background_patch to array
        curr_image_array.append(curr_random_background_patch)
        curr_label_array.append(background_label)

        # prepare image crops for LMDB
        # currImageArray, currLabelArray = self.__transformImagesForTraining(currImageArray, currLabelArray);

        # change image and label array data format(needed when this function is used alone)
        # currImageArray = np.asarray(currImageArray, dtype="uint8");
        # currLabelArray = np.asarray(currLabelArray, dtype="int64");
        return curr_image_array, curr_label_array

    def save_to_dir_structure(self, image_array, label_array):
        """
        Saves given imageArray(uint8) and labelArray(int64)
        to a sub-directory structure
        """
        dir_name_dict = {0: "/background", 1: "/ball", 2: "/robot", 3: "/goalpost"}

        # create sub dirs based on dir_name_dict
        self.__create_sub_dirs(dir_name_dict)

        # save image patches to sub-dir structure
        for i in range(len(image_array)):
            curr_image_path = self.output_path + dir_name_dict[label_array[i]] + "/img" + str(i) + ".png"
            cv2.imwrite(curr_image_path, image_array[i])

        return

    #################################################
    # Private
    #################################################
    def __resize_images(self, image_array, label_array):
        resized_image_array = []
        for img in image_array:
            img = cv2.resize(img, (self.patchSize, self.patchSize))
            resized_image_array.append(img)

        return resized_image_array, label_array

    def __transform_images_for_training(self, image_array, label_array):
        """
        Warps image crops to match the given network input and change image
        data structure (e.g. from [32,32,3] to [3,32,32]).
        """
        transformed_image_array = []
        for img in image_array:
            img = cv2.resize(img, (self.patchSize, self.patchSize))
            img = np.asarray([img[:, :, 0], img[:, :, 1], img[:, :, 2]])
            transformed_image_array.append(img)

        return transformed_image_array, label_array

    def __generate_image_crops(self, image, bb_map):
        """
        Crops all patches given by bbMap from given image
        """
        image_array = []
        label_array = []
        for key in bb_map:
            start_x, start_y, end_x, end_y = bb_map[key]
            curr_image_bb = image[start_y:end_y, start_x:end_x]
            curr_label = self.__get_label(key)
            image_array.append(curr_image_bb)
            label_array.append(curr_label)

        return image_array, label_array

    @staticmethod
    def __do_vflip_for_image_with_label(image_array, label_array, label):
        """
        Applies vflip to every image with given label.
        """
        len_image_array = len(image_array)
        for i in range(len_image_array):
            if label == label_array[i]:
                curr_image = image_array[i]
                flip_image = cv2.flip(curr_image, 1)
                image_array.append(flip_image)  # vflip
                label_array.append(label)

    def __even_patch_numbers(self, image_array, label_array):
        even_image_array = []
        even_label_array = []
        # divide image patches by label
        image_dict = {}
        for i in range(len(image_array)):
            if not label_array[i] in image_dict:
                image_dict[label_array[i]] = []
                image_dict[label_array[i]].append(image_array[i])
            else:
                image_dict[label_array[i]].append(image_array[i])

        # get class with minimum patches
        class_counter_array = []
        for key in image_dict:
            class_counter_array.append(len(image_dict[key]))
        # print(class_counter_array)
        max_patch_num_per_class = min(class_counter_array)

        # extract the given number of random patches for each class
        for key in image_dict:
            curr_even_batch, curr_even_labels = self.__extract_random_patches_uniform(image_dict[key], key,
                                                                                      max_patch_num_per_class)
            # print(len(curr_even_batch))
            even_image_array += curr_even_batch
            even_label_array += curr_even_labels

        return even_image_array, even_label_array

    @staticmethod
    def __extract_random_patches_uniform(image_array, label, number_of_patches):
        """
        Extracts 'numberOfPatches' elements using a uniform-distribution from the given 'imageArray'.
        @see __evenPatchNumbers()
        """
        new_image_array = []
        new_label_array = []
        len_image_array = len(image_array) - 1
        # check for out of bounds error
        if number_of_patches - 1 > len_image_array:
            print(
                "Cannot extract " + str(number_of_patches) + " patches from image array of length " +
                str(len_image_array))
            sys.exit()

        for i in range(number_of_patches):
            curr_random_index = int(np.floor(np.random.uniform(0, len_image_array)))
            new_image_array.append(image_array.pop(curr_random_index))
            new_label_array.append(label)
            len_image_array -= 1

        return new_image_array, new_label_array

    @staticmethod
    def __load_label_config():
        """
        Reads LabelConfig.txt to dictionary
        """
        label_dict = {}
        with open("./LabelConfig.txt") as file:
            data = file.readlines()
            data = [x.replace("\n", "") for x in data]
            data = [x.split(":") for x in data]
            for i in data:
                label_dict[i[0]] = i[1]

        return label_dict

    @staticmethod
    def __load_filenames(files_path, extension, is_sorted=True, sorting_criterion=None):
        """
        Load all file names in the given directory with the given extention into a list
        """
        print("loading files: " + extension)
        file_list = []
        # if any files in folder
        if len(os.walk(files_path).__next__()[2]) > 0:  # 1:folder, 2:files
            all_file_list = os.walk(files_path).__next__()[2]

            for file in all_file_list:
                if file.endswith(extension):
                    file_list.append(file)

        if is_sorted:
            return sorted(file_list, key=sorting_criterion)

        return file_list

    def __read_legend_file(self):
        """
        Loads the legend file generated by UETrainingSetGenerator into a 
        dictionary structure
        """
        legend_dict = {}
        with open(self.pathToGroundTruth + self.legendFileName[0], "r") as currFile:
            file_data = currFile.readline().split(" ")
            curr_legend_index = 0
            for i in file_data:
                i = i.split(":")
                if len(i) < 2:  # catching occurring whitespaces at file endings
                    continue

                curr_legend_index += int(i[0])
                legend_dict[str(curr_legend_index)] = i[1]
        return legend_dict

    def __get_tag(self, key):
        legend_key_array = sorted(map(int, self.legendDict.keys()))
        for legendKey in legend_key_array:
            if key - 1 < legendKey:
                return self.legendDict[str(legendKey)]

    def __get_label(self, key):
        curr_tag = self.__get_tag(key)
        return int(self.labelDict[curr_tag])

    @staticmethod
    def __bounding_box_sanity_check(start_x_pos, start_y_pos, end_x_pos, end_y_pos):
        """
        Check for too small bounding boxes 
        """
        if (end_x_pos - start_x_pos) <= 20:
            return False
        if (end_y_pos - start_y_pos) <= 20:
            return False
        return True

    def __extract_all_bounding_boxes(self, mask_array):
        """
        Extracts all bounding boxes from the given maskArray
        based on the semantic segmentation given by the mask
        """
        bb_map = {}

        start_xpos = 3000
        start_ypos = 3000
        end_xpos = 0
        end_ypos = 0

        # run through all "pixels" of the maskArray
        for y in range(len(mask_array)):
            for x in range(len(mask_array[y])):
                # get curr_index from current maskFile
                if bool(re.search(r'\d', mask_array[y][x])):
                    curr_index = int(mask_array[y][x])
                    if curr_index == 0:
                        continue
                else:
                    continue

                # if curr_index has not been already seen -> add map entry
                if not (curr_index in bb_map):
                    bb_map[curr_index] = [start_xpos, start_ypos, end_xpos, end_ypos]

                # if curr_index has been seen -> update map entry
                if curr_index in bb_map:
                    if x < bb_map[curr_index][0]:  # start_xpos
                        bb_map[curr_index][0] = x
                    if x > bb_map[curr_index][2]:  # end_xpos
                        bb_map[curr_index][2] = x
                    if y < bb_map[curr_index][1]:  # start_ypos
                        bb_map[curr_index][1] = y
                    if y > bb_map[curr_index][3]:  # end_ypos
                        bb_map[curr_index][3] = y

        # check for overlappings in patches for fieldmarkings and goalposts
        # if any overlapping is found -> for now: this patch is omitted //TODO: Change to IOU function
        kill_array = []
        for key in bb_map:
            if self.__get_tag(key) == "fieldmarkings" or self.__get_tag(key) == "goalpost":
                stop = False
                start_x, start_y, end_x, end_y = bb_map[key]
                for y in range(start_y, end_y):
                    if stop is True:
                        break
                    for x in range(start_x, end_x):
                        if stop is True:
                            break
                        curr_index = int(mask_array[y][x])
                        if curr_index != 0 and self.__get_tag(curr_index) != self.__get_tag(key):
                            kill_array.append(key)
                            stop = True

        for key in kill_array:
            del bb_map[key]

        return bb_map

    def __augment_bounding_boxes(self, bb_map):
        """
        Applies small shifts in position and size to the bounding box to prevent 
        centered object positions thoughout the generated data set
        """
        kill_array = []
        for key in bb_map:
            start_xpos, start_ypos, end_xpos, end_ypos = bb_map[key]
            if not self.__bounding_box_sanity_check(start_xpos, start_ypos, end_xpos, end_ypos):
                kill_array.append(key)
                continue

            if self.__get_tag(key) == "ball":  # ball
                # random scale bounding box
                r = random.uniform(-0.2, 1)
                random_offset = int(r * 10)
                start_xpos = start_xpos - random_offset
                start_ypos = start_ypos - random_offset
                end_xpos = end_xpos + random_offset
                end_ypos = end_ypos + random_offset

                # get width and height of current bounding box
                curr_width = abs(start_xpos - end_xpos)
                curr_height = abs(start_ypos - end_ypos)

                # random translate bounding box
                r_x = random.uniform(-0.25, 0.25)  # random x translation factor
                r_y = random.uniform(-0.25, 0.25)  # random y translation factor
                r_x *= curr_width
                r_y *= curr_height
                if not (start_xpos + r_x < 0 or start_xpos + r_x >= 640):
                    if not (start_ypos + r_y < 0 or start_ypos + r_y >= 480):
                        start_xpos += r_x
                        start_ypos += r_y
                        end_xpos += r_x
                        end_ypos += r_y

            if self.__get_tag(key) == "crossing":
                # random scale bounding box
                r = random.uniform(0, 1)
                random_offset = int(r * 50)
                start_xpos = start_xpos - random_offset
                start_ypos = start_ypos - random_offset
                end_xpos = end_xpos + random_offset
                end_ypos = end_ypos + random_offset

            if self.__get_tag(key) == "fieldmarkings":
                start_xpos, start_ypos, end_xpos, end_ypos = self.__random_crop_bounding_box(start_xpos, start_ypos,
                                                                                             end_xpos, end_ypos)

            # prevent bounding box breaking image borders
            if start_xpos < 0:
                start_xpos = 0
            if start_ypos < 0:
                start_ypos = 0
            if end_xpos > 640:
                end_xpos = 640
            if end_ypos > 480:
                end_ypos = 480

            # return values to bbMap
            bb_map[key] = [int(start_xpos), int(start_ypos), int(end_xpos), int(end_ypos)]

        # delete all entries that failed the sanity check
        for key in kill_array:
            bb_map.pop(key, None)

        return bb_map

    @staticmethod
    def __get_random_background_patch(image, mask_array, label=0, max_width=256, max_height=256):
        """
        Cuts random bounding box from image where no other class is overlapping.
        """
        is_found = False
        start_xpos = None
        start_ypos = None
        end_xpos = None
        end_ypos = None

        img_width = image.shape[1]
        img_height = image.shape[0]
        # search for random patch that is no containing too much of other classes
        while not is_found:
            # get random number between 0 and 1
            r = random.uniform(0.1, 1)
            # apply random crop to max bounding box
            bb_width = int(math.floor(r * max_width))
            bb_height = int(math.floor(r * max_height))
            # get random bounding box "start points" (top-left-corner)
            start_xpos = int(random.randint(0, img_width - 1))
            start_ypos = int(random.randint(0, img_height - 1))
            end_xpos = int(start_xpos + bb_width)
            end_ypos = int(start_ypos + bb_height)
            # prevent out of bounds exception
            if end_xpos >= img_width or end_ypos >= img_height:
                continue

            not_field_counter = 0
            for y in range(start_ypos, (end_ypos - 1)):
                for x in range(start_xpos, (end_xpos - 1)):
                    # search bounding box for object pixels, if too many object pixels -> search for new bounding box
                    if int(mask_array[y][x]) != 0:
                        not_field_counter += 1

            bb_area = ((end_xpos - start_xpos) * (end_xpos - start_xpos)) + (
                    (end_ypos - start_ypos) * (end_ypos - start_ypos))

            if not_field_counter <= (bb_area * 0.01):
                is_found = True

        # cut patch from image
        curr_patch = image[start_ypos:end_ypos, start_xpos:end_xpos]

        return curr_patch, label

    def __create_sub_dirs(self, dir_name_dict):
        # create root dir
        if not os.path.exists(self.output_path):
            os.makedirs(self.output_path, exist_ok=True)

        # create all dirs corresponding to tags
        for key in dir_name_dict:
            os.makedirs(self.output_path + dir_name_dict[key], exist_ok=True)

        return

    def __random_crop_bounding_box(self, start_x_pos, start_y_pos, end_x_pos, end_y_pos):
        """
        Cuts a random crop from inside a given bounding box bounds.
        The minimum size of the random crop area is the given patchsize
        """
        if abs(start_x_pos - end_x_pos) > self.patchSize and abs(start_y_pos - end_y_pos) > self.patchSize:
            # get top-left random point in bounding box with respect to patch size
            x1 = np.random.uniform(start_x_pos, end_x_pos - self.patchSize)
            y1 = np.random.uniform(start_y_pos, end_y_pos - self.patchSize)

            # get width and height of maximum bounding box with respect to x1 and y1
            w1 = abs(x1 - end_x_pos)
            h1 = abs(y1 - end_y_pos)

            # get bottom-right random point
            x2 = np.random.uniform(x1, (x1 + w1) - self.patchSize) + self.patchSize
            y2 = np.random.uniform(y1, (y1 + h1) - self.patchSize) + self.patchSize

            return x1, y1, x2, y2

        return start_x_pos, start_y_pos, end_x_pos, end_y_pos

    def __process_mask(self, mask_name, image_height):
        """
        Processes given maskFile into 2d-array structure.
        """
        mask_array = []
        with open(self.pathToGroundTruth + mask_name, "r") as currFile:
            for i in range(image_height):  # 480
                # read line from segMaskFile
                curr_line_data = currFile.readline()
                # gather segNames from File
                curr_line_data = curr_line_data.split(" ")
                mask_array.append(curr_line_data)
        return mask_array


#########################################################
# Program
#########################################################
if __name__ == "__main__":
    argv = sys.argv

    parser = OptionParser()
    parser.add_option("-i", "--imgData", action="store", type="string", dest="pathToImages",
                      default="data/Screenshots/")
    parser.add_option("-g", "--groundTruth", action="store", type="string", dest="pathToGroundTruth",
                      default="data/ScreenshotMasks/")
    parser.add_option("-p", "--patchSize", action="store", type="int", dest="patchSize", default=16)
    parser.add_option("-s", "--saveTo", action="store", type="string", dest="output", default="./extracted_patches")
    (options, args) = parser.parse_args(sys.argv)

    DATA_SET_SOURCE = options.pathToImages
    GT_SOURCE = options.pathToGroundTruth
    PATCH_SIZE = options.patchSize
    OUTPUT_FOLDER = options.output

    print("")
    if DATA_SET_SOURCE is not None:
        print("Loading images from: " + DATA_SET_SOURCE)
    else:
        print("No path to images given...")
        print("use -h for help")
        sys.exit(0)
    if GT_SOURCE is not None:
        print("Loading ground truth from: " + GT_SOURCE)
    else:
        print("No path to ground truth data given...")
        print("use -h for help")
    print("Using patch size: " + str(PATCH_SIZE) + "*" + str(PATCH_SIZE))
    print("Saving data set to: " + OUTPUT_FOLDER)
    print("")

    dataSetExtractor = DataSetExtractor(DATA_SET_SOURCE, GT_SOURCE, PATCH_SIZE, OUTPUT_FOLDER)

    # extract data to folder
    dataSetExtractor.extract_dataset(is_patch_number_evening=True, rgb_format=True)
