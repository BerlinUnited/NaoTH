function trajectory = generateOneStepBezier(supFootY, startSwingingFootY, targetSwingingFootY, transitionScaling, inFootScalingY, offsetT, offsetY)
    start    = (supFootY + startSwingingFootY)/2;
    target   = (supFootY + targetSwingingFootY)/2;
    
    supFootY = supFootY + offsetY;

    start_to_foot = FourPointBezier2D([0.0 transitionScaling/4 transitionScaling/4 transitionScaling/2; ...
                                       start (start+supFootY)/2 (start+supFootY)/2 supFootY],0:0.001:1);

    in_foot = FourPointBezier2D([transitionScaling/2 transitionScaling/2+transitionScaling/4 1-transitionScaling/2-transitionScaling/4 1-transitionScaling/2; ...
                                 supFootY supFootY+(supFootY-start)/2*inFootScalingY supFootY+(supFootY-target)/2*inFootScalingY supFootY],0:0.001:1);

    %in_foot = FourPointBezier2D([transitionScaling/2 transitionScaling/2+transitionScaling/4 1-transitionScaling/2-transitionScaling/4 1-transitionScaling/2; ...
    %                             supFootY supFootY+(supFootY-start)/2*inFootScalingY supFootY supFootY],0:0.001:1);                             
                             
    foot_to_target = FourPointBezier2D([1-transitionScaling/2 1-transitionScaling/4 1-transitionScaling/4 1; ...
                                        supFootY (target+supFootY)/2 (target+supFootY)/2 target],0:0.001:1);

    %foot_to_target = FourPointBezier2D([1-transitionScaling/2 1-transitionScaling/4 1-transitionScaling/4 1; ...
    %                                    supFootY supFootY supFootY supFootY],0:0.001:1);                                    
                                    
    trajectory = [start_to_foot in_foot foot_to_target];
    
    trajectory(1,:) = trajectory(1,:)*0.25 + offsetT;
end
