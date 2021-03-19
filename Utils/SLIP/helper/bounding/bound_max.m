function [ bounded ] = bound_max( x, max )
%BOUND_MAX Summary of this function goes here
%   Detailed explanation goes here

      x = reshape(x,1,numel(x));
      max = reshape(max,1,numel(max));
      bounded = zeros(1,numel(x));
      idx = x >= max;
      
      % infeasible starting value. use bound.
      bounded(idx) = max(idx);
      idx = ~idx;
      bounded(idx) = sqrt(max(idx) - x(idx));
end

