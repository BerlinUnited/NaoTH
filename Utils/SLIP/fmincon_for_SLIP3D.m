clear all;

lower_bound= [pi-pi/18, ... % td_angle_theta
                     0, ... % td_angle_phi
                 12000, ... % k
                   0.8, ... % init_leg_length
                     0, ... % init_leg_theta
                   0.5];    % init_x_vel

upper_bound= [   pi, ... % td_angle_theta
              pi/10, ... % td_angle_phi
              16000, ... % k
                  1, ... % init_leg_length
               pi/4, ... % init_leg_theta
                1.5];    % init_x_vel
            
x0 = [     2.992, ...
           0.207, ...
           13347, ...
         0.99869, ...
      0.00075871, ...
          1.0166];
    
opts = optimoptions(@fmincon,'Algorithm', 'active-set',    ... default interior-point
                             'OptimalityTolerance', 1e-12, ... default 1e-6
                             'FunctionTolerance',   1e-12); % default 1e-6) ;
      
results = fmincon(@SLIP_performance,x0,[],[],[],[],lower_bound,upper_bound,...
                        @nonlcon,opts);

function p = SLIP_performance(x)
    parameter.maxTime = 100;
    parameter.options = odeset( ... %'OutputFcn', @odeplot,'OutputSel', [1 3],
        'Refine',4, 'RelTol', 1e-6, 'AbsTol', 1e-9);

    parameter.l0 = 1;   % m
    parameter.m  = 80;   % kg
    parameter.k  = x(3); % N/m
    parameter.touchdown_angle = [x(1), x(2)];

    X.init_leg_length =x(4);
    X.init_leg_theta = x(5);
    X.init_x_vel = x(6);

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
    
    %p = determineQuaterStepPerformance(ieout, teout, yeout, tdpout, yout);
    
    p = determineStepPerformance(y0, ieout, yeout, teout, tdpout, yout);
end

function [c,ceq] = nonlcon(x)
    c   = [0,0,0,0,0,0];
    ceq = [0,0,0,0,0,0];
end

function p = determineQuaterStepPerformance(ieout, teout, yeout, tdpout, yout)
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
            tdp_B = tdpout(idx,[3 5]);
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

function p = determineStepPerformance(y0, ieout, yeout, teout, tdpout, yout)
   % performance index from paper for quater step optimization
    initial_state = [y0(1), y0(3), y0(5), y0(2), y0(4)];
    
    % find tdp of right foot during midstance
    idx = find(ieout == 2, 1); % will get only one idx at most (terminating event)
    if(isempty(idx))
        final_state = yout(end,[1 3 5 2 4]); % no midstance event occured -> fallen
    else
        idx2 = find((tdpout(:,1) < teout(idx) & (tdpout(:,2) == 0)), 1, 'last');
        if(isempty(idx2))
            final_state = yout(end,[1 3 5 2 4]); % if no tdp with right foot during midstance -> flying phase -> use projected com
        else
            final_state = yeout(idx,[1 3 5 2 4]);
            final_state(1) = final_state(1) - tdpout(idx,3);
            final_state(2) = final_state(2) - tdpout(idx,5);
        end
    end
     
    p =  norm(initial_state([1 3 4 5])-final_state([1 3 4 5]))^2 ...
        + (initial_state(2) + final_state(2))^2;
        
end