
addpath '../tools'

%load('./new/result_result_CNN_ND_all_91_retrained_small_5_epochs/convnet.mat');
%im_in = imread('../data/ball24.png');

bs = size(im_in,1);
ns = 16;
im = ones(bs+ns)*mean(mean(im_in));
im(ns/2+(1:bs), ns/2+(1:bs)) = im_in;

% apply a simple mean filter
fsize = 2;
im = imfilter(im,ones(fsize)*1/(fsize^2));

figure
imshow(uint8(im));

figure
imshow(uint8(im(ns/2+(1:bs), ns/2+(1:bs))));

res = zeros(bs);

for x = 1:bs
    for y = 1:bs
        s = im(x+(1:ns),y + (1:ns));
        r = predict(convnet, s);
        res(x,y) = r(1);
    end
end

figure
imshow(res);

figure
imshow(double(res).*double(im_in)/255.0);


[ix, x] = max(mean(res,1), [], 2);
[iy, y] = max(mean(res,2), [], 1);

x = uint8(sum(mean(res,1)/sum(mean(res,1)).*(1:bs))+0.5);
y = uint8(sum(mean(res,2)'/sum(mean(res,2)).*(1:bs))+0.5);

mm = zeros(bs);
mm(y, x) = 1;


figure
plot(mean(res,1))
figure
plot(mean(res,2))

figure
imshow(mm);