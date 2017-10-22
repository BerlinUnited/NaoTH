function [ Pref ] = bezierApprox(px,pp,t)
%BEZIERAPPROX Summary of this function goes here
%   Detailed explanation goes here
    % for each time step
    
    idx = 1;
    for i = 1:numel(t)
        while(t(i) >= px(idx))
            idx = idx + 1;
        end
        
        if(idx == 1) 
            Pref(i) = pp(idx);
        else
            Pref(i) = FourPointBezier1D([pp(idx-1) pp(idx-1) pp(idx) pp(idx)], (t(i)-px(idx-1))/(px(idx)-px(idx-1)));
        end
    end
end

