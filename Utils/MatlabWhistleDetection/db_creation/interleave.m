function [c] = interleave(a, b)
    ar = a(:)';    % make sure ar is a row vector
    br = b(:)';    % make sure br is a row vector
    A = [ar;br];   % concatenate them vertically
    c = A(:)';      % flatten and transpose the result
end

