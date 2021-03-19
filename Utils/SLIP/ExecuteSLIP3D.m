hold off;
clear all;

parameter.l0 = 1;   % m
parameter.m  = 80;   % kg
parameter.maxTime = 100;

parameter.options = odeset( ...%'OutputFcn', @odeplot,'OutputSel', [1 3], ...
    'RelTol', 1e-6, ...
    'AbsTol', 1e-9, ...
    'Refine', 1, ...
    'MaxStep', 1e-2);

fig = figure(1);
ax = axes;
box on
hold on;

parameter.A = [1 0 0 0 0;0 1 0 0 0;0 0 -1 0 0;0 0 0 -1 0;0 0 0 0 1];
parameter.B = [1 0 0;0 -1 0;0 0 1];
parameter.K = [-0.654890233934113 -0.642836212808432 1.46507289075606e-07 1.44720561048440e-07 0.267316287317275;-1.10657752276589e-06 -5.64008412954113e-05 -4.10806021567492 -3.40151422440362 -0.000823412401939584;3.64169830823154e-06 -4.52183343289762e-05 8.16438359388338e-11 6.25291743129772e-11 -0.000723199441127653];

td_angle_theta    =  2.97162702610267;
td_angle_phi      =  2.18681539959303e-07;
parameter.k       =  15011.6575775283;
X.init_leg_length =  0.995891964424831;
X.init_leg_theta  =  2.04009876352827e-08;
X.init_x_vel      =  0.611111111111111;
                                                        
parameter.touchdown_angle = [td_angle_theta, td_angle_phi];  % [theta, phi]

y0 = convertToSLIPState(X,'spherical');

parameter.enable_assert = true;
parameter.terminal_ms = false;
parameter.terminal_lh = false;
[tout, yout, teout, yeout, ieout, tsout, sout, tdpout] = SLIP3D(y0,parameter);

% plot(teout,yeout(:,1),'ro')
% xlabel('time');
% ylabel('height');
% hold off
% odeplot([],[],'done');

left_footprints  = tdpout(tdpout(:,2)==1,:);
right_footprints = tdpout(tdpout(:,2)==0,:);

figure(2);
plot(yout(:,1),yout(:,3));
hold on
plot(left_footprints(:,3),left_footprints(:,4),'bx');
plot(right_footprints(:,3),right_footprints(:,4),'rx');
hold off
xlabel('x');
ylabel('y');

figure(3);
plot(yout(:,1),yout(:,5));
hold on
plot(left_footprints(:,3),left_footprints(:,5),'bx');
plot(right_footprints(:,3),right_footprints(:,5),'rx');
hold off
xlabel('x');
ylabel('z');

figure(4);
stem(tsout,sout);
xlabel('time');
ylabel('state');

figure(5);
plot3(yout(:,1), yout(:,3), yout(:,5));
hold on
plot3(left_footprints(:,3),left_footprints(:,4),left_footprints(:,5),'bx');
plot3(right_footprints(:,3),right_footprints(:,4),right_footprints(:,5),'rx');
hold off
xlabel('x');
ylabel('y');
zlabel('z');