hold off;
clear all;

parameter.l0 = 1;   % m
parameter.m  = 80;   % kg
parameter.k  = 14000; % N/m
parameter.touchdown_angle = pi*69/180;  % time opt
parameter.maxTime = 100;
parameter.options = odeset( 'OutputFcn', @odeplot,'OutputSel', [1 3], ...
    'Refine',4, 'RelTol', 1e-6, 'AbsTol', 1e-9);

fig = figure(1);
ax = axes;
box on
hold on;

y0 = [0; 1.1; 1; 0]; % avoid x=0 as start if jumping on place... causes instabilities somehow
[tout, yout, teout, yeout, ieout, tsout, sout] = BSLIP(y0,parameter);

plot(teout,yeout(:,1),'ro')
xlabel('time');
ylabel('height');
title('Ball trajectory and the events');
hold off
odeplot([],[],'done');

figure(2);
plot(yout(:,1),yout(:,3));
figure(3);
plot(tsout,sout);