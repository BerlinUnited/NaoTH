import os

from naoth.log import Reader as LogReader
from naoth.log import Parser

def create_jpeg_image_log_dict(image_log):
    """
    Return a dictionary with frame numbers as key and image data as values.
    """
    images_by_frame = {}

    myParser = Parser()
    myParser.register("ImageJPEG"   , "Image")
    myParser.register("ImageJPEGTop", "Image")

    reader = LogReader(image_log, parser=myParser)

    for frame in reader.read():
        images = {}

        if "ImageJPEG" in frame.get_names():
            images["ImageJPEG"] = frame["ImageJPEG"]

        if "ImageJPEGTop" in frame.get_names():
            images["ImageJPEGTop"] = frame["ImageJPEGTop"]

        
        images_by_frame[frame.number] = images

    return images_by_frame
    

if __name__ == "__main__":
   
   game_log = "tmp/game.log"
   jpeg_log = "tmp/images_jpeg.log"
   
   combined_log = f"{game_log}.combined.log"
   
   # load the images into a registry
   image_log_index = create_jpeg_image_log_dict(jpeg_log)
   
   
   with LogReader(game_log) as game_reader, open(combined_log, 'wb') as output:
        
        for frame in game_reader.read():
        
            if frame.number not in image_log_index:
                print('Frame {} has no corresponding image data.'.format(frame.number))
            else:
                # contains 'ImageTop' and 'Image'
                images = image_log_index[frame.number]
                
                for image_name, image_msg in images.items():
                    frame.add_field(image_name, image_msg)
                    
                # write the modified frame to the new log
                output.write(bytes(frame))
                
                
                # HACK: Frames are indexed by the log reader. Remove the image of already processed frames to preserve memory.
                for image_name in image_log_index[frame.number]:
                    frame.remove(image_name)
        
