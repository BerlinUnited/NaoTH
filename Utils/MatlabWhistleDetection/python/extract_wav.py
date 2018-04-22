import os
import sys
import getopt
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
    for file in os.listdir(input_path):
        if file.endswith(".mp4") or file.endswith(".MP4"):
            file = os.path.join(input_path, file)
            filename = os.path.splitext(file)[0]
            print("Filename: ", filename)
            """
            -map_channel:
              The first 0 is the input file id
              The next 1 is the stream specifier - should be the audio stream, 0 is video
              The next 0 is the channel id
              
            -ar 8000 resamples the channel to 8kHz
            """

            os.system("ffmpeg -i {0} -map_channel 0.1.0 -ar 8000{1}.wav".format(file, filename))
        else:
            continue


if __name__ == '__main__':

    path = parse_arguments(sys.argv[1:])

    extract_wav(path)
