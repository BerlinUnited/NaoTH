function [c] = interleaves(a, b)
%INTERLEAVES Summary of this function goes here
%   Detailed explanation goes here

    ar = a(:)';    % make sure ar is a row vector
    br = b(:)';    % make sure br is a row vector
    A = [ar;br];   % concatenate them vertically
    c = A(:)';      % flatten and transpose the result

end

