import os
import sys
import getopt
from shutil import copyfile

"""
  Extracts the audio from our game videos. This script expects that ffmpeg is installed and in the PYTHONPATH. 

  Usage: python extract_wav.py -i <path_to_folder_where_mp4_files_are>
"""


def parse_arguments(argv):
    input_path = ''
    try:
        opts, args = getopt.getopt(argv, "hi:", ["ifile="])
    except getopt.GetoptError:
        print('python extract_wav.py -i <path>')
        sys.exit(2)
    if opts is None:
        print('python extract_wav.py -i <path>')
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print('python extract_wav.py -i <path>')
            sys.exit()
        elif opt in ("-i", "--ifile"):
            input_path = arg

    return input_path


def extract_wav(input_path):

  for root, subFolders, files in os.walk(input_path):
    for file in files:
        if file.endswith(".wav") or file.endswith(".WAV"):
            file = os.path.join(root, file)
            temp_filename = os.path.splitext(file)[0] + "-temp"
            temp_filename = temp_filename + ".wav"
            print("File: ", file)
            print("Filename: ", temp_filename)
            """
            -map_channel:
              The first 0 is the input file id
              The next 1 is the stream specifier - should be the audio stream, 0 is video
              The next 0 is the channel id
      
            -ar 8000 resamples the channel to 8kHz
            """

            # save resampled wav in a temp file
            os.system("ffmpeg -i {0} -map_channel 0.0.0 -ar 8000 {1}".format(file, temp_filename))
            # copy temp over original
            copyfile(temp_filename, file)

            # delete temp file
            os.remove(temp_filename)

        else:
            continue


if __name__ == '__main__':
    path = parse_arguments(sys.argv[1:])

    extract_wav(path)
