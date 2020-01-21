function [tout, yout, teout, yeout, ieout, tsout, sout] = BSLIP(y0, parameter)
%SLIP  simulates a basic SLIP model.
%   This function uses the ODE45 solver to simulate the SLIP model

    l0 = parameter.l0; % m
    m  = parameter.m;  % kg
    k  = parameter.k;  % N/m
    right_tda = parameter.touchdown_angle; % TODO: Binary Search, GS, CMA-ES
    left_tda  = parameter.touchdown_angle;

    options = parameter.options;

    tstart = 0;
    deltaT = 1;

    tout = tstart;
    yout = y0.';
    teout = [];
    yeout = [];
    ieout = [];
    tsout = [];
    sout  = [];
    
    state = 'left_support';    
    left_tdp = [0;0];
    right_tdp = [0;0];

    while(tout(end) < parameter.maxTime)       
        switch state
            case 'left_support'
                tsout = [tsout; tout(end)];
                sout  = [sout; 1];
                event = @(t,y) events_singleSupport(t,y,l0,left_tdp,right_tda);
                options = odeset(options,'Events',event);
                ie = 0;

                while ~any(ie)
                    f = @(t,y) singleSupport(t,y,k,m,l0,left_tdp);
                    [t,y,te,ye,ie] = ode45(f,[tstart tstart + deltaT],y0,options);

                    % Accumulate output.  This could be passed out as output arguments.
                    nt = length(t);
                    tout = [tout; t(2:nt)];
                    yout = [yout; y(2:nt,:)];
                    teout = [teout; te];          % Events at tstart are never reported.
                    yeout = [yeout; ye];
                    ieout = [ieout; ie];

                    y0 = y(nt,:);

                    % A good guess of a valid first timestep is the length of the last valid
                    % timestep, so use it for faster computation.  'refine' is 4 by default.
                    %options = odeset(options,'InitialStep',t(nt)-t(nt-options.Refine),...
                    %    'MaxStep',t(nt)-t(1));

                    tstart = t(nt); 
                end
                
                if any(ie == 1) % fallen
                    break;
                elseif all(ie == 2) % liftoff
                    state = 'flying';
                elseif all(ie == 3) % touchdown
                    state = 'double_support';
                    right_tdp = [y(end,1);y(end,3)] + l0 * [cos(-right_tda);sin(-right_tda)];
                else %liftoff and touchdown
                    state = 'right_support';
                    right_tdp = [y(end,1);y(end,3)] + l0 * [cos(-right_tda);sin(-right_tda)];
                end
                
                last_support_foot_was_left = true;
                
            case 'right_support'
                tsout = [tsout; tout(end)];
                sout  = [sout; 2];
                event = @(t,y) events_singleSupport(t,y,l0,right_tdp,left_tda);
                options = odeset(options,'Events',event);
                ie = 0;

                while ~any(ie)
                    f = @(t,y) singleSupport(t,y,k,m,l0,right_tdp);
                    [t,y,te,ye,ie] = ode45(f,[tstart tstart + deltaT],y0,options);

                    % Accumulate output.  This could be passed out as output arguments.
                    nt = length(t);
                    tout = [tout; t(2:nt)];
                    yout = [yout; y(2:nt,:)];
                    teout = [teout; te];          % Events at tstart are never reported.
                    yeout = [yeout; ye];
                    ieout = [ieout; ie];

                    y0 = y(nt,:);

                    % A good guess of a valid first timestep is the length of the last valid
                    % timestep, so use it for faster computation.  'refine' is 4 by default.
                    %options = odeset(options,'InitialStep',t(nt)-t(nt-options.Refine),...
                    %    'MaxStep',t(nt)-t(1));

                    tstart = t(nt); 
                end
                
                if any(ie == 1) % fallen
                    break;
                elseif all(ie == 2) % liftoff
                    state = 'flying';
                elseif all(ie == 3) % touchdown
                    state = 'double_support';
                    left_tdp = [y(end,1);y(end,3)] + l0 * [cos(-left_tda);sin(-left_tda)];
                else %liftoff and touchdown
                    state = 'left_support';
                    left_tdp = [y(end,1);y(end,3)] + l0 * [cos(-left_tda);sin(-left_tda)];
                end
                
                last_support_foot_was_left = false;
                
            case 'double_support'
                tsout = [tsout; tout(end)];
                sout  = [sout; 3];
                event = @(t,y) events_doubleSupport(t,y,l0,left_tdp,right_tdp);
                options = odeset(options,'Events',event);
                ie = 0;

                while ~any(ie)
                    f = @(t,y) doubleSupport(t,y,k,m,l0,left_tdp,right_tdp);
                    [t,y,te,ye,ie] = ode45(f,[tstart tstart + deltaT],y0,options);

                    % Accumulate output.  This could be passed out as output arguments.
                    nt = length(t);
                    tout = [tout; t(2:nt)];
                    yout = [yout; y(2:nt,:)];
                    teout = [teout; te];          % Events at tstart are never reported.
                    yeout = [yeout; ye];
                    ieout = [ieout; ie];

                    y0 = y(nt,:);

                    % A good guess of a valid first timestep is the length of the last valid
                    % timestep, so use it for faster computation.  'refine' is 4 by default.
                    %options = odeset(options,'InitialStep',t(nt)-t(nt-options.Refine),...
                    %    'MaxStep',t(nt)-t(1));

                    tstart = t(nt); 
                end
               
                if any(ie == 1) % fallen
                    break;
                elseif all(ie == 2) % left liftoff
                    state = 'right_support';
                elseif all(ie == 3) % right liftoff
                    state = 'left_support';
                else % right and left liftoff
                    state = 'flying';
                end
                
            case 'flying'
                tsout = [tsout; tout(end)];
                sout  = [sout; 4];
                if(last_support_foot_was_left)
                    event = @(t,y) events_flying(t,y,l0,right_tda);
                else
                    event = @(t,y) events_flying(t,y,l0,left_tda);
                end
                
                options = odeset(options,'Events',event);
                ie = 0;

                while ~any(ie)
                    [t,y,te,ye,ie] = ode45(@flying,[tstart tstart + deltaT],y0,options);

                    % Accumulate output.  This could be passed out as output arguments.
                    nt = length(t);
                    tout = [tout; t(2:nt)];
                    yout = [yout; y(2:nt,:)];
                    teout = [teout; te];          % Events at tstart are never reported.
                    yeout = [yeout; ye];
                    ieout = [ieout; ie];

                    y0 = y(nt,:);

                    % A good guess of a valid first timestep is the length of the last valid
                    % timestep, so use it for faster computation.  'refine' is 4 by default.
                    %options = odeset(options,'InitialStep',t(nt)-t(nt-options.Refine),...
                    %    'MaxStep',t(nt)-t(1));

                    tstart = t(nt);
                end
                
                if any(ie == 1) % fallen
                    break;
                else % right or left touchdown
                    if(last_support_foot_was_left)
                        state = 'right_support';
                        right_tdp = [y(end,1);y(end,3)] + l0 * [cos(-right_tda);sin(-right_tda)];
                    else
                        state = 'left_support';
                        left_tdp = [y(end,1);y(end,3)] + l0 * [cos(-left_tda);sin(-left_tda)];
                    end
                end
        end
    end
end

% --------------------------------------------------------------------------

function dydt = flying(t,y) % y(1)= x, y(2)=v_x, y(3)=y, y(4)=v_y
    dydt = [y(2);     0; ...
            y(4); -9.81]; % x,y
end

function dydt = doubleSupport(t,y,k,m,l0,left_tdp,right_tdp) % y(1)= x, y(2)=v_x, y(3)=y, y(4)=v_y
    b = k/m; % some constants
    left_leg = [y(1); y(3)] - left_tdp;
    left_ll = norm(left_leg);
    right_leg = [y(1); y(3)] - right_tdp;
    right_ll = norm(right_leg);
    g = [0, -9.81];
    dydt = [y(2); b*(l0/left_ll-1)*left_leg(1) + b*(l0/right_ll-1)*right_leg(1) + g(1); ...
            y(4); b*(l0/left_ll-1)*left_leg(2) + b*(l0/right_ll-1)*right_leg(2) + g(2)];
end

function dydt = singleSupport(t,y,k,m,l0,touchdown_point) % y(1)= x, y(2)=v_x, y(3)=y, y(4)=v_y
    b = k/m; % some constants
    leg = [y(1); y(3)] - touchdown_point;
    leg_length = norm(leg);
    g = [0, -9.81];
    dydt = [y(2); b*(l0/leg_length-1)*leg(1) + g(1); ...
            y(4); b*(l0/leg_length-1)*leg(2) + g(2)];
end

% --------------------------------------------------------------------------

function [value,isterminal,direction] = events_flying(t,y,l0,touchdown_angle)
    %define touchdown event
    value = [y(3); y(3) + l0 * sin(-touchdown_angle)]; % [fallen down; y of touchdown point = 0]
    isterminal = [1; 1];
    direction  = [0;-1];
end

function [value,isterminal,direction] = events_doubleSupport(t,y,l0,left_tdp,right_tdp)
    %define liftoff events
    left_leg  = [y(1); y(3)] - left_tdp;
    right_leg = [y(1); y(3)] - right_tdp;  
    value = [y(3); l0 - norm(left_leg); l0 - norm(right_leg)]; % [fallen down; left liftoff; right liftoff)
    isterminal = [1;  1;  1]; % stop the integration because dynamics have to be switchted,
    direction  = [0; -1; -1]; % negative direction
end

function [value,isterminal,direction] = events_singleSupport(t,y,l0,tdp,tda)
    leg = [y(1); y(3)] - tdp; 
    value = [y(3); l0 - norm(leg); y(3) + l0 * sin(-tda)]; % [fallen down; liftoff; touchdown]
    isterminal = [1;  1;  1]; % stop the integration because dynamics have to be switchted,
    direction  = [0; -1; -1]; % negative direction
end

