function [ bounded ] = bound_min( x, min )
%BOUND_MAX Summary of this function goes here
%   Detailed explanation goes here

      x = reshape(x,1,numel(x));
      min = reshape(min,1,numel(min));
      bounded = zeros(1,numel(x));
      idx = x <= min;
      
      % infeasible starting value. use bound.
      bounded(idx) = min(idx);
      idx = ~idx;
      bounded(idx) = sqrt(x(idx) - min(idx));
end

