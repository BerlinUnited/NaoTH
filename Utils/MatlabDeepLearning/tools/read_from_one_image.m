function res = read_from_one_image( path )

img = imread(path);
img = img(:,:,1);
s = 16;

w = size(img,1)/s;
h = size(img,2)/s;

res = zeros(s,s,1,w*h);
for i = 0:(w-1)
    for j = 0:(h-1)
        res(:,:,:,1+i*h+j) = img((1+i*s):((i+1)*s),(1+j*s):((j+1)*s));
    end
end

end

