hold off;
clear all;

parameter.l0 = 1;   % m
parameter.m  = 1;   % kg
parameter.k  = 100; % N/m
%parameter.touchdown_angle = 1.3419; % hop opt, multiple executions
%parameter.touchdown_angle = 1.509; % dist opt
%parameter.touchdown_angle = pi/180 * 77.8; % manuel % TODO: Binary Search, GS, CMA-ES
parameter.touchdown_angle = 1.358;  % time opt
%parameter.touchdown_angle = 1.2888; % y traveled distance
parameter.maxTime = 100;
parameter.options = odeset( 'OutputFcn', @odeplot,'OutputSel', [1 3], ...
    'Refine',4, 'RelTol', 1e-6, 'AbsTol', 1e-9);

fig = figure(1);
ax = axes;
box on
hold on;

y0 = [0; 1; 1; 0]; % avoid x=0 as start if jumping on place... causes somehow instabilities
[tout, yout, teout, yeout, ieout] = SLIP(y0,parameter);

plot(teout,yeout(:,1),'ro')
xlabel('time');
ylabel('height');
title('Ball trajectory and the events');
hold off
odeplot([],[],'done');

figure(2);
plot(yout(:,1),yout(:,3));