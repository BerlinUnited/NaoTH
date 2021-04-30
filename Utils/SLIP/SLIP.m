function [tout, yout, teout, yeout, ieout] = SLIP(y0, parameter)
%SLIP  simulates a basic SLIP model.
%   This function uses the ODE45 solver to simulate the SLIP model

    l0 = parameter.l0; % m
    m  = parameter.m;  % kg
    k  = parameter.k;  % N/m
    touchdown_angle = parameter.touchdown_angle; % TODO: Binary Search, GS, CMA-ES

    options = parameter.options;

    tstart = 0;
    deltaT = 1;

    on_ground = false;

    tout = tstart;
    yout = y0.';
    teout = [];
    yeout = [];
    ieout = [];

    while(tout(end) < parameter.maxTime)   
        if(on_ground)        
            touchdown_point = [y(end,1);y(end,3)] + l0 * [cos(-touchdown_angle);sin(-touchdown_angle)];

            event = @(t,y) events_onGround(t,y,l0,touchdown_point);
            options = odeset(options,'Events',event);
            ie = 0;
            
            while ie == 0
                f = @(t,y) onGround(t,y,k,m,l0,touchdown_point);
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
        else
            event = @(t,y) events_flying(t,y,l0,touchdown_angle);
            options = odeset(options,'Events',event);
            ie = 0;

            while ie == 0
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
        end

        if(ie == 1) % fallen
            break;
        end

        on_ground = ~on_ground; % change after break => if fallen last state is known
    end
end

% --------------------------------------------------------------------------

function dydt = flying(t,y) % y(1)= x, y(2)=v_x, y(3)=y, y(4)=v_y
    dydt = [y(2);     0; ...
            y(4); -9.81]; % x,y
end

function dydt = onGround(t,y,k,m,l0,touchdown_point) % y(1)= x, y(2)=v_x, y(3)=y, y(4)=v_y
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
    value = [y(3); y(3) + l0 * sin(-touchdown_angle)]; % fallen down or y of touchdown point = 0
    isterminal = [1; 1];
    direction  = [0;-1];
end

function [value,isterminal,direction] = events_onGround(t,y,l0,touchdown_point)
    %define lift off event
    leg = [y(1); y(3)] - touchdown_point;
    value = [y(3); l0 - norm(leg)]; % fallen down or leg is back in rest length
    isterminal = [1; 1];   % stop the integration because dynamics have to be switchted,
    direction  = [0;-1]; % negative direction
end