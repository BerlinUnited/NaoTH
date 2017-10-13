td_angle = optimizableVariable('touchdown_angle',[pi/4 pi/2]);

results = bayesopt(@(x)SLIP_performance(x), td_angle, ...
                    'NumSeedPoints',4, ... default 4
                    'MaxObjectiveEvaluations', 60, ... default 30
                    'MaxTime', Inf, ...
                    'IsObjectiveDeterministic', true);


function p = SLIP_performance(X)
    parameter.l0 = 1;   % m
    parameter.m  = 1;   % kg
    parameter.k  = 100; % N/m
    parameter.touchdown_angle = X.touchdown_angle; %pi/180 * 77.8; % TODO: Binary Search, GS, CMA-ES
    parameter.maxTime = 100;
    parameter.options = odeset( ... %'OutputFcn', @odeplot,'OutputSel', [1 3],
                                'Refine',4, 'RelTol', 1e-6, 'AbsTol', 1e-9);
                            
    y0 = [0; 1; 1; 0]; % avoid x=0 as start if jumping on place... causes somehow instabilities

    [tout, yout, ~, ~, ieout] = SLIP(y0,parameter);
    
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
     p = -tout(end);
end