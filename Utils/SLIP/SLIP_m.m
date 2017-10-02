%function SLIP_m
%BALLODE  Run a demo of a bouncing ball.
%   This is an example of repeated event location, where the initial
%   conditions are changed after each terminal event.  This demo computes ten
%   bounces with calls to ODE23.  The speed of the ball is attenuated by 0.9
%   after each bounce. The trajectory is plotted using the output function
%   ODEPLOT.
%
%   See also ODE23, ODE45, ODESET, ODEPLOT, FUNCTION_HANDLE.

%   Mark W. Reichelt and Lawrence F. Shampine, 1/3/95
%   Copyright 1984-2014 The MathWorks, Inc.
clear all;

l0 = 1;
m = 1;
k = 100;
touchdown_angle = pi/2;

tstart = 0;
tfinal = 30;
y0 = [0; 0; 2; 0];
on_ground = false;
refine = 4;
options = odeset('Events',@events,'OutputFcn',@odeplot,'OutputSel',[1 3],...
   'Refine',refine);

fig = figure;
ax = axes;
%ax.XLim = [0 5];
%ax.YLim = [0 3];
box on
hold on;

tout = tstart;
yout = y0.';
teout = [];
yeout = [];
ieout = [];
for i = 1:10
   % Solve until the first terminal event.
   if(on_ground)
        touchdown_point = [y(end,1);y(end,3)] + l0 * [cos(-touchdown_angle);sin(-touchdown_angle)];
        
        event = @(t,y) events_onGround(t,y,l0,touchdown_point);
        options = odeset(options,'Events',event);
        
        f = @(t,y) onGround(t,y,k,m,l0,touchdown_point);
        [t,y,te,ye,ie] = ode23(f,[tstart tfinal],y0,options);
        
        if(ie == 1) % fallen
            break;
        end
   else
        event = @(t,y) events_flying(t,y,l0,touchdown_angle);
        options = odeset(options,'Events',event);
       
        [t,y,te,ye,ie] = ode23(@flying,[tstart tfinal],y0,options);
   end
   
   if ~ishold
      hold on
   end
   % Accumulate output.  This could be passed out as output arguments.
   nt = length(t);
   tout = [tout; t(2:nt)];
   yout = [yout; y(2:nt,:)];
   teout = [teout; te];          % Events at tstart are never reported.
   yeout = [yeout; ye];
   ieout = [ieout; ie];
   
   ud = fig.UserData;
   if ud.stop
      break;
   end
  
   y0 = y(nt,:);
   
   on_ground = ~on_ground;
   
   % A good guess of a valid first timestep is the length of the last valid
   % timestep, so use it for faster computation.  'refine' is 4 by default.
   options = odeset(options,'InitialStep',t(nt)-t(nt-refine),...
      'MaxStep',t(nt)-t(1));
   
   tstart = t(nt);
end

plot(teout,yeout(:,1),'ro')
xlabel('time');
ylabel('height');
title('Ball trajectory and the events');
hold off
odeplot([],[],'done');

% --------------------------------------------------------------------------

function dydt = flying(t,y) % y(1)= x, y(2)=v_x, y(3)=y, y(4)=v_y
dydt = [y(2); 0; ...
        y(4); -9.81]; % x,y
end

function dydt = onGround(t,y,k,m,l0,touchdown_point) % y(1)= x, y(2)=v_x, y(3)=y, y(4)=v_y
b = k/m; % some constants
%l_0; some constant
leg = [y(1);y(3)] - touchdown_point;
leg_length = norm(leg);
g=[0,-9.81];
dydt = [y(2); b*(l0/leg_length-1)*leg(1) + g(1);...
        y(4); b*(l0/leg_length-1)*leg(2) + g(2)];
end    
% --------------------------------------------------------------------------

function [value,isterminal,direction] = events_flying(t,y,l0,touchdown_angle)
%define touchdown event
value = y(3) + l0 * sin(-touchdown_angle); % y of touchdown point = 0
isterminal =  1;
direction  = -1;
end

function [value,isterminal,direction] = events_onGround(t,y,l0,touchdown_point)
%define lift off event
leg = [y(1);y(3)] - touchdown_point;
value = [y(3); l0 - norm(leg)]; % leg is back in rest length
isterminal =  [1;1];   % stop the integration because dynamics have to be switchted,
direction  = [0;-1]; % negative direction
end
