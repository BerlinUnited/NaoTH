
addpath '../tools'

[data_test, labels_test] = load_from_mat('../data/test');
load('./new/result_CNN_ND_all_91_retrained_small_5_epochs/convnet.mat');

%{
fsize = 2;
for i = 1:length(labels_test)
    data_test(:,:,:,i) = imfilter(data_test(:,:,:,i), ones(fsize)*1/(fsize^2));
end
%}

r = classify(convnet, data_test);
t = (labels_test == 'ball');
s = (r == 'ball');
figure;
plotconfusion(t', s')

fp = find(r == 'ball' & labels_test == 'noball');
figure;
montage(uint8(data_test(:,:,:,fp)))

fn = find(r == 'noball' & labels_test == 'ball');
figure;
montage(uint8(data_test(:,:,:,fn)))
% montage(uint8(data_test(:,:,:,predict_noball(fn))))