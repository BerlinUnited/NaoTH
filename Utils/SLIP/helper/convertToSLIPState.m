function y0 = convertToSLIPState(X, type)

switch type
    case 'spherical'
        init_pos = X.init_leg_length * [ sin(X.init_leg_theta) * cos(-pi/2);
                                         sin(X.init_leg_theta) * sin(-pi/2);
                                         cos(X.init_leg_theta)]; 
                                     
        % avoid x=0 as start if jumping on place... causes somehow instabilities             
        y0 = [init_pos(1); X.init_x_vel; init_pos(2); 0; init_pos(3); 0];  
                 
    case 'cartesian'
        % avoid x=0 as start if jumping on place... causes somehow instabilities             
        y0 = [0; X.init_x_vel; X.init_y; 0; X.init_z; 0]; 
end

