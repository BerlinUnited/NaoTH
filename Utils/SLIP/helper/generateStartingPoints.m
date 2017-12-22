function [ X ] = generateStartingPoints()
%GENERATESTARTINGPOINTS Summary of this function goes here
%   Detailed explanation goes here

[X0, X1, X2, X3, X4, X5] = ndgrid(linspace(pi-pi/18, pi, 10), ... td_angle_theta
                                  linspace(0, pi/10, 10), ... % td_angle_phi
                                  linspace(12000, 16000, 10), ... % k
                                  linspace(0.8, 1, 10), ... % init_leg_length
                                  linspace(0, pi/4, 10), ... % init_leg_theta
                                  linspace(0.5, 1.5, 10));    % init_x_vel

X = [X0(:), X1(:), X2(:), X3(:), X4(:), X5(:)]; 
end

