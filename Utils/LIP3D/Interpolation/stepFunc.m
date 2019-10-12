function [ Pref ] = stepFunc( x,y,t )
%STEPFUNC Summary of this function goes here
%   Detailed explanation goes here

    idx = 1;
    for i = 1:numel(t)
        while(t(i) >= x(idx))
            idx = idx + 1;
        end
        
        if(idx == 1) 
            Pref(i) = y(idx);
        else
            Pref(i) = y(idx-1);
        end
    end
end

