td_angle_theta = optimizableVariable('td_angle_theta',[pi-pi/18 pi]);
td_angle_phi = optimizableVariable('td_angle_phi',[0 pi/10]);

% define starting midstance
init_leg_length = optimizableVariable('init_leg_length',[0.8, 1]);
init_leg_theta  = optimizableVariable('init_leg_theta',[0, pi/4]);
init_x_vel = optimizableVariable('init_x_vel',[0.5, 1.5]);

results = bayesopt(@(x)SLIP_performance(x), ...
                    [td_angle_theta, td_angle_phi, ...
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
    parameter.k  = 14000; % N/m
    parameter.touchdown_angle = [X.td_angle_theta, X.td_angle_phi]; %pi/180 * 77.8; % TODO: Binary Search, GS, CMA-ES
    parameter.maxTime = 100;
    parameter.options = odeset( ... %'OutputFcn', @odeplot,'OutputSel', [1 3],
    'Refine',4, 'RelTol', 1e-6, 'AbsTol', 1e-9);

    init_pos = X.init_leg_length * [ sin(X.init_leg_theta) * cos(-pi/2);
                                     sin(X.init_leg_theta) * sin(-pi/2);
                                     cos(X.init_leg_theta)]; 
                     
    y0 = [init_pos(1); X.init_x_vel; init_pos(2); 0; init_pos(3); 0];  % avoid x=0 as start if jumping on place... causes somehow instabilities             
                 
    parameter.enable_assert = true;
    parameter.terminal_ms = false;
    parameter.terminal_lh = false;
    [tout, yout, teout, yeout, ieout, tsout, sout] = SLIP3D(y0,parameter);
    
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
    p = sum(sout == 3) * -(yout(end,1) - yout(end,3)^2);
end