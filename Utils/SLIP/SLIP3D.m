function [tout, yout, teout, yeout, ieout, tsout, sout, tdpout] = SLIP3D(y0, parameter)
%SLIP  simulates a basic SLIP model.
%   This function uses the ODE45 solver to simulate the SLIP model
%   Attention: angles are interpreted as polar coordinates tda=[theta, phi]

    l0 = parameter.l0; % m
    m  = parameter.m;  % kg
    k  = parameter.k;  % N/m
    right_tda = parameter.touchdown_angle; % TODO: Binary Search, GS, CMA-ES
    right_tda(2) = -right_tda(2);
    left_tda  = parameter.touchdown_angle;
    
    options = parameter.options;

    deltaT = 1;

    tout = 0;
    yout = y0.';
    teout = [];
    yeout = [];
    ieout = [];
    tsout = [];
    sout  = [];
    tdpout = [];
    
    state = 'left_support';    
    left_tdp  = [0;0;0];
    right_tdp = [y0(1); y0(3); y0(5)] ...
              + l0 * [sin(right_tda(1)) * cos(-pi/2); sin(right_tda(1)) * sin(-pi/2); cos(right_tda(1))];

    while(tout(end) < parameter.maxTime)     
        switch state
            case 'left_support'  
                tdpout = [tdpout; [tout(end), 1, left_tdp']];
                sout  = [sout; 1];
                event = @(t,y) events_singleSupport(t,y,l0,left_tdp,right_tda, parameter.terminal_ms, parameter.terminal_lh);
                f = @(t,y) singleSupport(t,y,k,m,l0,left_tdp);
            case 'right_support'               
                tdpout = [tdpout; [tout(end), 0, right_tdp']];
                sout  = [sout; 3];
                event = @(t,y) events_singleSupport(t,y,l0,right_tdp,left_tda, parameter.terminal_ms, parameter.terminal_lh);
                f = @(t,y) singleSupport(t,y,k,m,l0,right_tdp);
            case 'double_support'
                sout  = [sout; 2];
                event = @(t,y) events_doubleSupport(t,y,l0,left_tdp,right_tdp, parameter.terminal_ms, parameter.terminal_lh);
                f = @(t,y) doubleSupport(t,y,k,m,l0,left_tdp,right_tdp);
            case 'flying'
                sout  = [sout; 4];
                if(last_support_foot_was_left)
                    tda = right_tda;
                else
                    tda = left_tda;
                end
                event = @(t,y) events_flying(t,y,l0,tda, parameter.terminal_ms, parameter.terminal_lh);
                f = @(t,y) flying(t,y);
        end
                
        tsout = [tsout; tout(end)];
        options = odeset(options,'Events',event);
        ie = 0;
        
        while ~any(ie)
            % system is  time-invariant
            [t,y,te,ye,ie] = ode45(f,[0 deltaT],yout(end,:),options);
            
            % filter events: events happening in the first step are
            % reported but they are
            % 1) never terminal
            % 2) maybe an artifact of the last state(?) -> might be
            %    handled better
            ie = ie(abs(te) > 10e-16);
            ye = ye(abs(te) > 10e-16,:);
            te = te(abs(te) > 10e-16);
            
            % Accumulate output.  This could be passed out as output arguments.
            tout = [tout; tout(end) + t(2:end)];
            yout = [yout; y(2:end,:)];
            teout = [teout; te];          % Events at tstart are never reported.
            yeout = [yeout; ye];
            ieout = [ieout; ie];

        end
        
        if any(ie == 1) % fallen
            break;
        end
        
        if(parameter.terminal_ms && any(ie == 2)) % midstance
            break;
        else
            ie = ie(ie ~= 2); % remove if not terminal
        end
        
        if(parameter.terminal_lh && any(ie == 3)) % lowest hight
            break;
        else   
            ie = ie(ie ~= 3); % remove if not terminal
        end
        
        switch state
            case 'left_support'
                if all(ie == 4) % liftoff
                    state = 'flying';
                elseif all(ie == 5) % touchdown
                    state = 'double_support';
                    right_tdp = [y(end,1); y(end,3); y(end,5)] + l0 * [sin(right_tda(1)) * cos(right_tda(2)); sin(right_tda(1)) * sin(right_tda(2)); cos(right_tda(1))];
                else %liftoff and touchdown
                    
                    if (parameter.enable_assert)
                        save('error.mat', y0, parameter);
                        assert(false,'liftoff and touchdown 2at the same time", logical error?');
                    end
                    
                    break;
                    
                    %state = 'right_support';
                    %right_tdp = [y(end,1); y(end,3); y(end,5)] + l0 * [sin(right_tda(1)) * cos(right_tda(2)); sin(right_tda(1)) * sin(right_tda(2)); cos(right_tda(1))];
                    
                    %if(right_tdp(3) < 0) % the touch down event is an artefact from the first integration step, e.g. after a flying phase
                    %    state = 'flying';
                    %end
                end
                
                last_support_foot_was_left = true;
                
            case 'right_support'
                if all(ie == 4) % liftoff
                    state = 'flying';
                elseif all(ie == 5) % touchdown
                    state = 'double_support';
                    left_tdp = [y(end,1); y(end,3); y(end,5)] + l0 * [sin(left_tda(1)) * cos(left_tda(2)); sin(left_tda(1)) * sin(left_tda(2)); cos(left_tda(1))];
                else %liftoff and touchdown
                    
                    if (parameter.enable_assert)
                        save('error.mat', y0, parameter);
                        assert(false,'liftoff and touchdown 2at the same time", logical error?');
                    end
                    
                    break;
                    %state = 'left_support';
                    %left_tdp = [y(end,1); y(end,3); y(end,5)] + l0 * [sin(left_tda(1)) * cos(left_tda(2)); sin(left_tda(1)) * sin(left_tda(2)); cos(left_tda(1))];
                    
                    %if(left_tdp(3) < 0) % the touch down event is an artefact from the first integration step, e.g. after a flying phase
                    %    state = 'flying';
                    %end
                end
                
                last_support_foot_was_left = false;
                
            case 'double_support'
                if all(ie == 4) % left liftoff
                    state = 'right_support';
                elseif all(ie == 5) % right liftoff
                    state = 'left_support';
                else % right and left liftoff
                    state = 'flying';
                end
                
            case 'flying'
                if(last_support_foot_was_left)
                    state = 'right_support';
                    right_tdp = [y(end,1); y(end,3); y(end,5)] + l0 * [sin(right_tda(1)) * cos(right_tda(2)); sin(right_tda(1)) * sin(right_tda(2)); cos(right_tda(1))];
                else
                    state = 'left_support';
                    left_tdp  = [y(end,1); y(end,3); y(end,5)] + l0 * [sin(left_tda(1)) * cos(left_tda(2)); sin(left_tda(1)) * sin(left_tda(2)); cos(left_tda(1))];
                end
        end
    end
end


% --------------------------------------------------------------------------

function dydt = flying(t,y) % y(1)= x, y(2)=v_x, y(3)=y, y(4)=v_y
    dydt = [y(2);     0; ...
            y(4);     0; ...
            y(6); -9.81]; % x,y
end

function dydt = doubleSupport(t,y,k,m,l0,left_tdp,right_tdp) % y(1)= x, y(2)=v_x, y(3)=y, y(4)=v_y
    b = k/m; % some constants
    left_leg  = [y(1); y(3); y(5)] - left_tdp;
    left_ll = norm(left_leg);
    right_leg = [y(1); y(3); y(5)] - right_tdp;
    right_ll = norm(right_leg);
    g = [0, 0, -9.81];
    dydt = [y(2); b*(l0/left_ll-1)*left_leg(1) + b*(l0/right_ll-1)*right_leg(1) + g(1); ...
            y(4); b*(l0/left_ll-1)*left_leg(2) + b*(l0/right_ll-1)*right_leg(2) + g(2);
            y(6); b*(l0/left_ll-1)*left_leg(3) + b*(l0/right_ll-1)*right_leg(3) + g(3)];
end

function dydt = singleSupport(t,y,k,m,l0,touchdown_point) % y(1)= x, y(2)=v_x, y(3)=y, y(4)=v_y
    b = k/m; % some constants
    leg = [y(1); y(3); y(5)] - touchdown_point;
    leg_length = norm(leg);
    g = [0, 0, -9.81];
    dydt = [y(2); b*(l0/leg_length-1)*leg(1) + g(1); ...
            y(4); b*(l0/leg_length-1)*leg(2) + g(2); ...
            y(6); b*(l0/leg_length-1)*leg(3) + g(3)];
end

% --------------------------------------------------------------------------

function [value,isterminal,direction] = events_flying(t,y,l0,touchdown_angle,terminal_ms, terminal_lh)
    %define touchdown event
    value = [y(5); y(6); y(6); y(5) + l0 * cos(touchdown_angle(1))]; % [fallen down; midstance; lowest hight; z of touchdown point = 0]
    isterminal = [1; terminal_ms; terminal_lh;  1];
    direction  = [0;          -1;           1; -1];
end

function [value,isterminal,direction] = events_doubleSupport(t,y,l0,left_tdp,right_tdp,terminal_ms,terminal_lh)
    %define liftoff events
    left_leg  = [y(1); y(3); y(5)] - left_tdp;
    right_leg = [y(1); y(3); y(5)] - right_tdp;  
    value = [y(5); y(6); y(6); l0 - norm(left_leg); l0 - norm(right_leg)]; % [fallen down; midstance; lowest hight; left liftoff; right liftoff]
    isterminal = [1; terminal_ms; terminal_lh;  1;  1]; % stop the integration because dynamics have to be switchted,
    direction  = [0;          -1;           1; -1; -1]; % negative direction
end

function [value,isterminal,direction] = events_singleSupport(t,y,l0,tdp,tda,terminal_ms,terminal_lh)
    leg = [y(1); y(3); y(5)] - tdp; 
    value = [y(5); y(6); y(6); l0 - norm(leg); y(5) + l0 * cos(tda(1))]; % [fallen down; midstance; lowest hight; liftoff; touchdown]
    isterminal = [1; terminal_ms; terminal_lh;  1;  1]; % stop the integration because dynamics have to be switchted,
    direction  = [0;          -1;           1; -1; -1]; % negative direction
end