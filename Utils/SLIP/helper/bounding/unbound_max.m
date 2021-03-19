function [ unbounded ] = unbound_max( x, max )
%BOUND_MAX Summary of this function goes here
%   Detailed explanation goes here

      x = reshape(x,1,numel(x));
      max = reshape(max,1,numel(max));

      unbounded = max - x.^2;
end

