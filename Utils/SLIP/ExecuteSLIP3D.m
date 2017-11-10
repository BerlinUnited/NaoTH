hold off;
clear all;

parameter.l0 = 1;   % m
parameter.m  = 80;   % kg
parameter.k  = 14000; % N/m
parameter.maxTime = 100;
parameter.options = odeset( ...%'OutputFcn', @odeplot,'OutputSel', [1 3], ...
    'Refine',4, 'RelTol', 1e-6, 'AbsTol', 1e-9);

fig = figure(1);
ax = axes;
box on
hold on;

%y0 = [0; 1.2; 0; 0; -parameter.l0*cos(parameter.touchdown_angle(1)); 0]; % avoid x=0 as start if jumping on place... causes instabilities somehow
%y0 = [0; 1.1; 0; 0; 1; 0]; % avoid x=0 as start if jumping on place... causes instabilities somehow

% opt tout                           
% td_angle_theta  =  2.3469;
% td_angle_phi    = -0.11703;
% init_leg_length =  0.67807;
% init_leg_theta  = -0.61583;
% init_leg_phi    = -0.40995;
% init_x_vel      =  0.28869;
% init_y_vel      =  0.019281;
% init_z_vel      =  1.8528;    

% opt final state 
% td_angle_theta  =  2.2593;
% td_angle_phi    = -1.2734;
% init_leg_length =  0.54033;
% init_leg_theta  = -0.16329;
% init_leg_phi    = -1.257;
% init_x_vel      =  1.9829;
% init_y_vel      =  0.1106;
% init_z_vel      =  1.641;  

% opt final number of states                                                               
% td_angle_theta  =  2.4564;
% td_angle_phi    = -0.54891;
% init_leg_length =  0.90997;
% init_leg_theta  = -0.31314;
% init_leg_phi    = -0.12494;
% init_x_vel      =  1.959;
% init_y_vel      =  1.085;
% init_z_vel      =  0.28402; 

% some best found number of states times (final x - abs(y))
td_angle_theta  =  2.9675;
td_angle_phi    =  0.249;
init_leg_length =  0.99917;
init_leg_theta  =  0.011187;
init_x_vel      =  0.81274;
                                                        
parameter.touchdown_angle = [td_angle_theta, td_angle_phi];  % [theta, phi]

init_pos = init_leg_length * [sin(init_leg_theta) * cos(-pi/2);
                              sin(init_leg_theta) * sin(-pi/2);
                              cos(init_leg_theta)]; 
                     
y0 = [init_pos(1); init_x_vel; init_pos(2); 0; init_pos(3); 0];  % avoid x=0 as start if jumping on place... causes somehow instabilities             

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