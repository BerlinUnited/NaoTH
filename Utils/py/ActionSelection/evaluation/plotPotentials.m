clear all;
load('potentials.mat');

% Plot matrix as colorcoded squares
%imagesc(potentials);

%surf(potentials)

contour(potentials)

% try function fitting
% x = int32(1:size(potentials,2));
% y = int32(1:size(potentials,1));
% z = potentials;
% [xo,yo,zo] = prepareSurfaceData(x,y,z); 
% sf = fit([xo, yo],zo,'poly23')
% plot(sf,[xo,yo],zo)