clear all;     
    
lower_bound = [pi-pi/18, ... % td_angle_theta
                     0, ... % td_angle_phi
                 12000, ... % k
                   0.8, ... % init_leg_length
                     0]; %, ... % init_leg_theta
                 %  0.5];    % init_x_vel

upper_bound = [   pi, ... % td_angle_theta
              pi/10, ... % td_angle_phi
              16000, ... % k
                  1, ... % init_leg_length
               pi/4]; %, ... % init_leg_theta
                %1.5];    % init_x_vel

opts = optimset('TolFun',   1e-12, ...% default 1e-6
                'MaxFunEvals', 1000 * numel(6)); % default 200 * numel(x0)) ;
    
X = generateStartingPoints();  
idx = randi(size(X,1),1,100);

results = zeros(100,6);
p  = zeros(100,1);

parfor i = 1:100
    state = X(idx(i),:);
    disp(i);
    x0 = bound_min_max(state(1:5), lower_bound, upper_bound);
    
    [r, p(i)] = fminsearch(@(x) SLIP_performance(x, state(6), lower_bound, upper_bound), x0, opts);
    
    results(i,:) = [unbound_min_max(r, lower_bound, upper_bound), X(idx(i),6)];
end

function p = SLIP_performance(x,x_vel, lower_bound, upper_bound)
    x_ub = unbound_min_max(x, lower_bound, upper_bound);
    
    assert(isreal(x_ub));

    parameter.maxTime = 100;
    parameter.options = odeset( ...%'OutputFcn', @odeplot,'OutputSel', [1 3], ...
        'RelTol', 1e-6, ...
        'AbsTol', 1e-9, ...
        'Refine', 1, ...
        'MaxStep', 1e-2);

    parameter.l0 = 1;   % m
    parameter.m  = 80;   % kg
    parameter.k  = x_ub(3); % N/m
    parameter.touchdown_angle = [x_ub(1), x_ub(2)];

    X.init_leg_length = x_ub(4);
    X.init_leg_theta  = x_ub(5);
    X.init_x_vel = x_vel;

    y0 = convertToSLIPState(X,'spherical');
    
    assert(isreal(y0));
    
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
    
    %p = determineQuaterStepPerformance(ieout, teout, yeout, tdpout, yout);
    
    p = midstanceToMidstancePerf(y0, ieout, yeout, teout, tdpout, yout(end,:));
end

function p = determineQuaterStepPerformance(ieout, teout, yeout, tdpout, yout)
   % performance index from paper for quater step optimization
    tdp_A = [0;0];
    
    % find tdp of right foot during midstance
    idx = find(ieout == 2, 1); % will get only one idx at most (terminating event)
    if(isempty(idx))
        tdp_B = yout(end,[1,3]); % no midstance event occured -> fallen
    else
        idx2 = find((tdpout(:,1) < teout(idx) & (tdpout(:,2) == 0)), 1, 'last');
        if(isempty(idx2))
            tdp_B = yout(end,[1,3]); % if no tdp with right foot during midstance -> flying phase -> use projected com
        else
            tdp_B = tdpout(idx2,[3 5]);
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