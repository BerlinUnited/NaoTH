function data = imgReader(filename)
    data = imread(filename);
    if size(data,3) > 1
        data = data(:,:,1);
    end
end

