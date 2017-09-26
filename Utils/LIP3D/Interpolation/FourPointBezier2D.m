function [ y ] = FourPointBezier2D(polygon, t)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

y = (1-t).*(1-t).*(1-t).*polygon(:,1)+3.*t.*(1-t).*(1-t).*polygon(:,2)+3.*t.*t.*(1-t).*polygon(:,3)+t.*t.*t.*polygon(:,4);
end

