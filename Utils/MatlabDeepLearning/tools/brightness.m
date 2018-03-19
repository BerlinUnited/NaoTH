function [ data, labels ] = brightness(data_all, labels_all)
    disp('Brightness was called')
    % TODO dont apply changes when to bright or too dark

    brightness_val = 1.2;

    channels = size(data_all,3);
    %TODO should every image be augmented, is the brightness setting fixed or
    %rando between some boundaries
    %numBrightnessSettings = 1;

    % allocate space for calculation of one augmented image per input image
    % this allows to copy the labels_all array
    new_data = zeros(size(data_all,1),size(data_all,2),channels,size(data_all,4));

    %iterate over all patches
    for i=1:size(data_all,4)
        a = data_all(:,:,channels,i);
        a = a * brightness_val;
        new_data(:,:,channels,i) = a;
        %imshow(uint8(new_data(:,:,channels,i)));
    end

    data = [data_all; new_data];
    labels = [labels_all; labels_all]; 
end