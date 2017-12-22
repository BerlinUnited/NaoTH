function [ bounded ] = bound_min_max(x,min,max)
%BOUNDTDA bounds touchdown angles
%  actually the touchdown_angle is constrainted:
%  0 <= theta <= pi
%  0 <= phi   <= pi
%  so use periodicity of cos to get them back into valid interval
    
    
     x   = reshape(x,1,numel(x));
     max = reshape(max,1,numel(max));
     min = reshape(min,1,numel(min));
     bounded = zeros(1,numel(x));
     
     % infeasible starting value. use bound.
     idx = x >= max;
     bounded(idx) = max(idx);
     
     idx = x <= min;
     bounded(idx) = min(idx);
     
     bounded = 2 .* (x-min) ./ (max-min) - 1; % map to [-1,1]
     % shift by 2*pi to avoid problems at zero in fminsearch
     % otherwise, the initial simplex is vanishingly small
     bounded = 2*pi+asin(bounded);
end

