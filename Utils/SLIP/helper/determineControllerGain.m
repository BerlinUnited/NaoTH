function [K] = determineControllerGain(y0,parameter)
%DETERMINEJACOBIANS Summary of this function goes here
%   Detailed explanation goes here

% symmetry matrices: f(x,u) = A f(Ax,Bu)
A = diag([1 1 -1 -1 1]); % state at midstance: x,x',y,y',z (z'=0)
B = diag([1 -1 1]); % theta,phi,k

parameter.enable_assert = true;
parameter.terminal_ms   = true;
parameter.terminal_lh   = false;

J = zeros(5,8);

for i = 1:8
    eps    = zeros(1,8);
    eps(i) = 1e-6;
    J(:,i) = calcDiffQuotient(y0, parameter, eps);
end

Jx = A * J(:,1:5);
Ju = A * J(:,6:8);

Q = 100 * eye(5);
R = eye(3);

[~,~,K] = dare(Jx, Ju, Q, R);

K = -K;
end

function diff = calcDiffQuotient(y0, parameter, eps)
    p = parameter;
    
    y = y0;
    y(1:5) = y(1:5) + eps(1:5)';
    p.touchdown_angle = parameter.touchdown_angle + eps(6:7);
    p.k = parameter.k + eps(8);
    [~, yout, ~, ~, ieout, ~, ~, tdpout] = SLIP3D(y,p);
    df_1 = yout(end, 1:5)'; % x,x',y,y',z 
    df_1([1 3 5]) = df_1([1 3 5]) - tdpout(end,3:5)'; % local to tdp
    
    %assert(any(ieout == 2) & any(ieout == 3)) % assure a midstance and lowest hight event
    
    y = y0;
    y(1:5) = y(1:5) - eps(1:5)';
    p.touchdown_angle = parameter.touchdown_angle - eps(6:7);
    p.k = parameter.k - eps(8);
    [~, yout, ~, ~, ieout, ~, ~, tdpout] = SLIP3D(y,p);
    df_2 = yout(end, 1:5)'; % x,x',y,y',z 
    df_2([1 3 5]) = df_2([1 3 5]) - tdpout(end,3:5)'; % local to tdp
    
    %assert(any(ieout == 2) & any(ieout == 3)) % assure a midstance and lowest hight event
    
    diff = (df_1 - df_2) ./ (2 * eps(eps ~= 0));
end
