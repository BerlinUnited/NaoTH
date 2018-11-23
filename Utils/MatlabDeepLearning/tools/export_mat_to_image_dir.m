function export_mat_to_image_dir( img_path, mat_path )
    load(mat_path,'data','labels')
    % get unique labels
    unique_labels = unique(labels);
    % create folders for extracting
    for i=1:length(unique_labels)    
        f = fullfile(img_path, char(unique_labels(i)));
        mkdir(f);
    end
    %
    datasize = size(data);
    for i=1:datasize(4)
        A = data(:,:,:,i)/255;    
        filename = strcat(int2str(i),'.png');
        filepath = fullfile(img_path,char(labels(i)),filename);
        imwrite(A, filepath);
    end
end
