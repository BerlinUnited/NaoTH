clear all;

load convnet.mat
cn = convnet;

ball = imread('../data/ball.png');

%% 
% check the InputLayer
% input cn
outputCN1 = activations(cn, ball, 1);

% input manual
averageImage = struct(cn.Layers(1)).PrivateLayer.AverageImage;
output1 = reshape(single(ball) - averageImage, 1, 16*16);

diff = max(max(abs(output1 - outputCN1)));
fprintf('InputLayer error: %f\n',diff);

%% check the convolution layer
% conv cn
outputCN2 = activations(cn, ball, 2);

% conv manual
w = cn.Layers(2).Weights;
b = cn.Layers(2).Bias;
output2 = filter2(w, reshape(output1,16,16), 'valid') + b;
output2 = reshape(output2,1,14*14);

diff = max(max(abs(output2 - outputCN2)));
fprintf('ConvilutionLayer error: %f\n',diff);

%% check reluLayer
% relu cn
outputCN3 = activations(cn, ball, 3);

% relu manual
output3 = output2;
output3(output3 < 0) = 0;

diff = max(max(abs(output3 - outputCN3)));
fprintf('ReLU error: %f\n',diff);

%% check fully connected
% fc cn
outputCN4 = activations(cn, ball, 4);

% fc manual
w = cn.Layers(4).Weights;
b = cn.Layers(4).Bias;
output4 = (w*output3' + b)';

diff = max(max(abs(output4 - outputCN4)));
fprintf('FullyConnected error: %f\n',diff);


%% check softmax
% softmax cn
outputCN5 = activations(cn, ball, 5);

% softmax manual
exponents = output4 - max(output4);
expX = exp(exponents);
output5 = expX./sum(expX);

diff = max(max(abs(output5 - outputCN5)));
fprintf('SoftMax error: %f\n',diff);





