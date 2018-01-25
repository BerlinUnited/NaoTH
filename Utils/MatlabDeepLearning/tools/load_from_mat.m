function [ data_all, labels_all ] = load_from_mat( varargin )

% load the first set
disp(['Load: ' varargin{1}]);
load(varargin{1});
data_all = data;
labels_all = labels;

% append the others
for i = 2:length(varargin)
    disp(['Load: ' varargin{i}]);
    load(varargin{i});
    data_all = cat(4, data_all, data);
    labels_all = cat(1, labels_all, labels);
end

end