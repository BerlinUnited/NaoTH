function [ unbounded ] = unbound_min_max(x,lb,ub)
%BOUNDTDA bounds touchdown angles
%  actually the touchdown_angle is constrainted:
%  0 <= theta <= pi
%  0 <= phi   <= pi
%  so use periodicity of cos to get them back into valid interval
    
    unbounded = (sin(x)+1)./2 .* (ub-lb) + lb;  
    unbounded = max(lb,min(ub,unbounded));
    
    assert(isreal(unbounded));
end

