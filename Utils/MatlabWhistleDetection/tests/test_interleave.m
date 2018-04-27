clear variables;
clc;
addpath(genpath('../MatDatabase'))
%% Test my custom interleave function

a = [1; 3; 5;];
b = [2 4 6];

c = interleave(a, b);

disp(c)