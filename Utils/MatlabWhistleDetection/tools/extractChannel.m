clear all
clc
%%
file = 'D:\Downloads\Whistle-db\capture_recordings\rc17_48k_captures\test_event\test_half\test_robot\capture_2017-7-29-6-31_48kHz_1channels.raw';

fileID = fopen (file, 'r');
    raw = fread(fileID, 'int16', 'ieee-le');
fclose(fileID);

new_samples = raw(1:4:end);

fileID = fopen('capture_2017-7-29-6-31_48kHz_1channels.raw', 'w');
    fwrite(fileID, new_samples, 'int16', 'ieee-le');
fclose(fileID);