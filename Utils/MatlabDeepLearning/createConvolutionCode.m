clear all;
clc;
%%
% Generates a cpp file which calculates a convolution for ball.png with a predefined
% filter
%%
% First read an image and print it as 2D array -> this is for testing only
ball = imread('data/ball.png');
ball = im2single(ball); %Normalize values to [0,1]

rows = size(ball,1);
cols = size(ball,2);

fileID = fopen('generated/genConv.cpp','w');

fprintf(fileID, '#include <iostream>\n\n');
fprintf(fileID, 'int main()\n');
fprintf(fileID, '{\n');

fprintf(fileID, '\tdouble ball[%d][%d] = {\n', rows, cols);

for x= 1:size(ball,1)
    fprintf(fileID,'\t\t{');
    for y=1:size(ball,2)
        if y == size(ball,2) && x== size(ball,1) % Omitt the other last comma
            fprintf(fileID, '%f ',ball(x, y));
        else
            fprintf(fileID, '%f, ',ball(x, y));
        end
    end
    if y == size(ball,2) && x== size(ball,1) % Omitt the other last comma
        fprintf(fileID, '}\n');
    else
        fprintf(fileID, '},\n');%  
    end
end
fprintf(fileID, '\t};\n'); % End Image Array
%%
% Define the filter, also just for debug purposes
% In Application just use the values directly
weights = [0  1 0;
           1 -4 1;
           0  1 0 ];
weights = single(repmat(weights, [1, 1, 1])) ;
kernelSize = size(weights,1);

fprintf(fileID, '\tdouble weights[%d][%d] = {\n', kernelSize, kernelSize);
for x= 1:kernelSize
    fprintf(fileID,'\t\t{');
    for y=1:kernelSize
        if y == kernelSize && x== kernelSize % Omitt the other last comma
            fprintf(fileID, '%f ',weights(x, y));
        else
            fprintf(fileID, '%f, ',weights(x, y));
        end
    end
    if y == kernelSize && x== kernelSize % Omitt the other last comma
        fprintf(fileID, '}\n');
    else
        fprintf(fileID, '},\n');%  
    end
end
fprintf(fileID, '\t};\n'); % End Weights Array

%%
% Create new array for Convolution Output
%TODO

%%
% Actually print out the conv formula
fprintf(fileID, 'double convImg[13][13];\n\n');

for y=2:size(ball,2)-1 
    for x=2:size(ball,1)-1      
        singleConv = calc_conv_once(y, x, 'ball', weights);
        fprintf(fileID, 'convImg[%d][%d] = %s;\n',y-2, x-2, singleConv); % -2: in matlab it starts at 2nd but in output c it should start at 0
    end
end

fprintf(fileID, 'std::cout << convImg[0][0] << std::endl;\n');

fprintf(fileID, '}'); % End main

% Manually apply the convolution filter for testing
calculateConvolution(2, 2, ball, weights)

function outputString = calc_conv_once(indX, indY, imgName, w)
outputString = '';
    for j=-1:1 % Assume that Kernelsize is 3 for now %FIXME
        for i=-1:1 % Assume that Kernelsize is 3 for now %FIXME
            %-1 in index since c starts with zero
            cstring = sprintf('%s[%d][%d] * %d', imgName, (indX+i-1), (indY+j-1), w(i+2,j+2));
            if (j == 1 && i == 1)                
            else
                cstring = sprintf('%s + ', cstring);
            end
            outputString = sprintf('%s%s', outputString, cstring);
        end   
    end
end

function value = calculateConvolution(indX, indY, grayImg, w)
    value = grayImg(indY-1,indX-1)  * w(1,1) ...
      + grayImg(indY,indX-1)        * w(2,1) ...
      + grayImg(indY+1,indX-1)      * w(3,1) ...
      + grayImg(indY-1,indX)        * w(1,2) ...
      + grayImg(indY,indX)          * w(2,2) ...
      + grayImg(indY+1,indX)        * w(3,2) ...
      + grayImg(indY-1,indX+1)      * w(1,3) ...
      + grayImg(indY,indX+1)        * w(2,3) ...
      + grayImg(indY+1,indX+1)      * w(3,3);
end