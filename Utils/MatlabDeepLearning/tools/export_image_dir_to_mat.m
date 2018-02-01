function export_image_dir_to_mat( img_path, mat_path )

% NOTE: imgReader makes sure that we always read only one channel images
digitData = imageDatastore(img_path, ...
        'IncludeSubfolders',true,...
        'LabelSource','foldernames',...
        'ReadFcn',@imgReader);

channels = 1;
imgs = readall(digitData);
data = zeros(size(imgs{1},1),size(imgs{1},2),channels,length(imgs));
data(:,:,:,:) = cat(3,imgs{:});

labels = digitData.Labels;

save(mat_path, 'data', 'labels');

end

