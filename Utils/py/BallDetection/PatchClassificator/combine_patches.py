#!/usr/bin/python
import os
import sys
from scipy import misc
import numpy as np
import json

import naoth.matlab as mat
import patchReader as patchReader

patch_size = (12, 12)  # width, height


def load_labels(patchdata, file):
    if not os.path.isfile(file):
        print(
            'Label file does not exist. To export the patches regardless run this file with the --all option')
        return

    patch_to_label = np.negative(np.ones((len(patchdata),), dtype=np.int))
    label_ids = []

    with open(file, 'r') as data_file:
        labels = json.load(data_file)

        for name in labels:
            patch_to_label[labels[name]] = len(label_ids)
            label_ids += [name]

    return patch_to_label, label_ids


def log_to_matrix(path):
    print("Process: {}".format(path))

    # type: 0-'Y', 1-'YUV', 2-'YUVC'
    patchtype = 0
    patchdata, _ = patchReader.read_all_patches_from_log(path, type=patchtype)
    if len(patchdata) == 0:
        print("ERROR: file doesn't contain any patches of the type {0}".format(patchtype))
        return

    # load the label file
    base_file, file_extension = os.path.splitext(path)
    label_file = base_file + '.json'
    if not os.path.exists(label_file):
        print("ERROR: file with labels does not exists: {}".format(label_file))
        return

    patch_to_label, label_ids = load_labels(patchdata, label_file)

    # export the patches
    imgs = {}

    for i in range(len(patchdata)):
        p = patchdata[i]
        if len(p) == 4 * patch_size[0] * patch_size[1]:
            a = np.array(p[0::4]).astype(float)
            a = np.transpose(np.reshape(a, patch_size))

            b = np.array(p[3::4]).astype(float)
            b = np.transpose(np.reshape(b, patch_size))
        else:
            a = np.array(p).astype(float)
            a = np.transpose(np.reshape(a, patch_size))

        im = np.stack((a, a, a), axis=2)
        if patch_to_label[i] not in imgs:
            imgs[patch_to_label[i]] = [im]
        else:
            imgs[patch_to_label[i]] += [im]

        # rgba
        '''
        rgba = np.zeros((patch_size[0],patch_size[1],4), dtype=np.uint8)
        rgba[:,:,0] = a
        rgba[:,:,1] = a
        rgba[:,:,2] = a
        rgba[:,:,3] = np.not_equal(b, 7)*255
        cv2.imwrite(file_path, rgba)
        '''

        # grayscale
        # yuv888 = np.zeros(patch_size[0]*patch_size[1], dtype=np.uint8)
        # yuv888 = np.reshape(a, patch_size[0]*patch_size[1])
        # gray_image = cv2.cvtColor(yuv888, cv2.COLOR_BGR2GRAY)

        # remove green:
        # gray + set green to 0 (used for balls)
        # a = np.multiply(np.not_equal(b, 7), a)

        # cv2.imwrite(file_path, a)

    for i in imgs:
        if i < 0:
            name = "none"
        else:
            name = label_ids[i]
        save_images(imgs[i], "{0}_{1}.png".format(os.path.basename(base_file), name))


def save_images(imgs, path):
    if len(imgs) == 0:
        print("ERROR: no patches to export")

    # HACK: for matlab we need a different shape
    b = np.stack(imgs, axis=3)
    mat.savemat("./test.mat", {"images": b})

    b = np.stack(imgs)

    # HACK: multiply the channel
    print(b.shape)
    if len(imgs[0].shape) == 2:
        b = np.stack((b, b, b), axis=3)
    print(b.shape)

    print(imgs[0].shape)
    # export a matrix
    s = imgs[0].shape[0]
    assert (s == imgs[0].shape[1])
    n = 100
    m = int(b.shape[0] / n)
    if m * n < b.size:
        m += 1

    print(s, n, m)

    M = np.zeros((s * m, s * n, 3))
    for i in range(0, b.shape[0]):
        x = i % n
        y = int(i / n)
        M[s * y:s * (y + 1), s * x:s * (x + 1), :] = b[i, :, :, :]

    print(M.shape)
    misc.imsave(path, M)


def directory_to_matrix(path, name=None):
    if name is None:
        name = os.path.basename(path)

    print("Process: {}".format(path))
    imgs = []
    for filename in os.listdir(path):
        file_path = os.path.join(path, filename)
        if filename.endswith('.png'):
            im = misc.imread(file_path)
            # add the channel dimension
            if len(im.shape) == 2:
                im = np.expand_dims(im, axis=2)
            imgs += [im]
        elif os.path.isdir(file_path):
            directory_to_matrix(file_path, name + '_' + filename)
        elif filename.endswith(".log"):
            log_to_matrix(file_path)

    if len(imgs) == 0:
        print("No images found")
        return None
    else:
        print("Images found: {}".format(len(imgs)))

    if not os.path.isdir('./export'):
        os.mkdir('./export')

    save_images(imgs, './export/{0}.png'.format(name))


if __name__ == "__main__":

    if len(sys.argv) > 1:
        path = sys.argv[-1]
        if os.path.exists(path) and os.path.isdir(path):
            directory_to_matrix(path)
        elif path.endswith(".log"):
            log_to_matrix(path)
        else:
            print("ERROR: path doesn't exist ", path)
