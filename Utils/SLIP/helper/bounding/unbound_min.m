function [ unbounded ] = unbound_min( x, min )
%BOUND_MAX Summary of this function goes here
%   Detailed explanation goes here

      x = reshape(x,1,numel(x));
      min = reshape(min,1,numel(min));

      unbounded = min + x.^2;
end

