function [ data_all, labels_all ] = load_from_mat( varargin )


files = {};
if isdir(varargin{1})
    basic_path = varargin{1};
    list = dir(basic_path);
    names = {list(:).name};
    
    for i = 1:length(names)
        name = names{i};
        if length(name) > 4 && strcmp(name(length(name)-3:end), '.mat') == 1
            files{length(files)+1} = [basic_path, '/', name];
        end
    end
else
    files = varargin;
end

% load the first set
disp(['Load: ' files{1}]);
load(files{1});
data_all = data;
labels_all = labels;

% append the others
for i = 2:length(files)
    disp(['Load: ' files{i}]);
    load(files{i});
    data_all = cat(4, data_all, data);
    labels_all = cat(1, labels_all, labels);
end

end