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
tfinal = 10;
y0 = [10];
on_ground = false;
refine = 4;
options = odeset('OutputFcn',@odeplot,'OutputSel',1 ,...
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
       
   [t,y] = ode23(@flying,[tstart tfinal],y0,options);

   
   if ~ishold
      hold on
   end
   % Accumulate output.  This could be passed out as output arguments.
   nt = length(t);
   tout = [tout; t(2:nt)];
   yout = [yout; y(2:nt,:)];
 
   ud = fig.UserData;



plot(teout,yeout(:,1),'ro')
xlabel('time');
ylabel('height');
title('Ball trajectory and the events');
hold off
odeplot([],[],'done');

% --------------------------------------------------------------------------

function dydt = flying(t,y) % y(1)= x, y(2)=v_x, y(3)=y, y(4)=v_y
dydt = -1 * y - 100;
end
  
