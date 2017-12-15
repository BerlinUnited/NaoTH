td_angle_theta = optimizableVariable('td_angle_theta',[pi-pi/18 pi]);
td_angle_phi = optimizableVariable('td_angle_phi',[0 pi/10]);
k = optimizableVariable('k',[12000 16000]);

% define starting midstance
% TODO: a valid midstance is a local maxima of z...
init_leg_length = optimizableVariable('init_leg_length',[0.8, 1]);
init_leg_theta  = optimizableVariable('init_leg_theta',[0, pi/4]);
init_x_vel = optimizableVariable('init_x_vel',[0.5, 1.5]);

results = bayesopt(@(x)SLIP_performance(x), ...
                    [td_angle_theta, td_angle_phi, k, ...
                     init_leg_length, init_leg_theta, ...
                     init_x_vel], ...
                    'NumSeedPoints', 10, ... default 4
                    'MaxObjectiveEvaluations', 200, ... default 30
                    'MaxTime', Inf, ...
                    'IsObjectiveDeterministic', true)% ...
                    % 'GPActiveSetSize', 600); % default 300 update needed?


function p = SLIP_performance(X)
    parameter.maxTime = 100;
    parameter.options = odeset( ... %'OutputFcn', @odeplot,'OutputSel', [1 3],
                                'Refine',4, 'RelTol', 1e-6, 'AbsTol', 1e-9);
                            
    parameter.l0 = 1;   % m
    parameter.m  = 80;   % kg
    parameter.k  = X.k; % N/m
    parameter.touchdown_angle = [X.td_angle_theta, X.td_angle_phi]; %pi/180 * 77.8; % TODO: Binary Search, GS, CMA-ES
    parameter.maxTime = 100;
    parameter.options = odeset( ... %'OutputFcn', @odeplot,'OutputSel', [1 3],
    'Refine',4, 'RelTol', 1e-6, 'AbsTol', 1e-9);

    y0 = convertToSLIPState(X,'spherical');
    
    parameter.enable_assert = true;
    parameter.terminal_ms = true;
    parameter.terminal_lh = false;
    [tout, yout, teout, yeout, ieout, tsout, sout, tdpout] = SLIP3D(y0,parameter);
    
    % bayesopt minimizes a loss
    % final position
    % p = -yout(end,1); 
    
    % number of hops ~ number of events
    % p = (-numel(ieout)-1)/2;
    
    % scale distance by hops
    % p = -yout(end,1) * numel(ieout);
    
    % traveled distance in x
    % p = - sum(abs(diff(yout(1:end,1))));
    
    % traveled distance in y
    % p = - sum(abs(diff(yout(3:end,1))));
    
    % time
    % p = -tout(end);
    
    % number of state changes
    % p = -numel(sout);
    
    % as much double support phases, as far on x without going to left or right as possible
    %p = sum(sout == 3) * -(yout(end,1) - yout(end,3)^2);
    
    % performance index from paper for quater step optimization
    tdp_A = [0;0];
    
    % find tdp of right foot during midstance
    idx = find(ieout == 2, 1); % will get only one idx at most (terminating event)
    if(isempty(idx))
        tdp_B = yout(end,[1,3]); % no midstance event occured -> fallen
    else
        idx = find((tdpout(:,1) < teout(idx) & (tdpout(:,2) == 0)), 1, 'last');
        if(isempty(idx))
            tdp_B = yout(end,[1,3]); % if no tdp with right foot during midstance -> flying phase -> use projected com
        else
            tdp_B = tdpout(idx,3:4);
        end
    end
      
    % find x,y projection during lowest hight event
    idx = find(ieout == 3, 1); % happen at most once... has to be followed by a midstance or fallen event
    if(isempty(idx))
        c_lh = [0;0];
    else
        c_lh = yeout(idx,[1,3]);
    end
    
    p = norm(0.5*(tdp_A + tdp_B) - c_lh)^2;
end