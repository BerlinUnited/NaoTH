function [ Pref ] = generateTrajectory( steps,t  )
%GENERATETRAJECTORY Summary of this function goes here
%   Detailed explanation goes here
offset = 0.25;

tra =      generateOneStepBezier(-100,    0,    0, 0.2, 4, offset*0,  30);
tra = [tra generateOneStepBezier(   0, -100, -100, 0.2, 4, offset*1, -30)];
tra = [tra generateOneStepBezier(-100,    0,    0, 0.2, 4, offset*2,  30)];
tra = [tra generateOneStepBezier(   0, -100, -100, 0.2, 4, offset*3, -30)];
tra = [tra generateOneStepBezier(-100,    0,    0, 0.2, 4, offset*4,  30)];
tra = [tra generateOneStepBezier(   0, -100, -100, 0.2, 4, offset*5, -30)];
tra = [tra generateOneStepBezier(-100,    0,    0, 0.2, 4, offset*6,  30)];
tra = [tra generateOneStepBezier( 0,   -100,    0, 0.2, 0.2,offset*7, 0)];
tra = [tra generateOneStepBezier( 0,      0,    0, 0.2,   1,offset*8, 0)];
tra = [tra generateOneStepBezier( 0,      0,    0, 0.2,   1,offset*9, 0)];

idx = 1;
for i = 1:numel(t)
    while(t(i) >= tra(1,idx))
        idx = idx + 1;
    end
    
    if(idx == 1)
        Pref(i) = tra(2,idx);
    else
        Pref(i) = tra(2,idx-1);
    end
end

end

