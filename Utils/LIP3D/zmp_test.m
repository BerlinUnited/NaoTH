clear
close all

pathCell = regexp(path, pathsep, 'split');
if ispc  % Windows is not case-sensitive
  onPath = any(strcmpi(strcat(pwd,'\Interpolation'), pathCell));
else
  onPath = any(strcmp(strcat(pwd,'/Interpolation'), pathCell));
end

if ~onPath
    addpath('./Interpolation');
end

% system model
z = 0.260; % m
dt = 0.010; % s
previewTime = 0.5; % s
R = 1;
q = 1e10;
[A, b, c, Ki, Ks, F] = previewcontrol(z, dt, previewTime,R,q);

% test
X = [0; 0; 0];
stepTime = 0.25; % s
StepSamples = stepTime / dt;
doubleSupportSamples = 0;
singleSupportSamples = StepSamples - doubleSupportSamples;
N = previewTime / dt;

%ZMP = [0, 0, -0.5, 0.1, -0.1, 0.1, 0, 0, 0, 0];
ZMP = [-100, 0.0, -100, 0.0, -100, 0.0, -100, 0, 0, 0];

pp  = [];
px  = [];
px2 = [];

tt = 0;
offset = stepTime*0.49;
for i = 1:length(ZMP)
    v = ZMP(i);
    pp  = [pp, v, v];
    px  = [px, tt+offset, tt+stepTime-offset];
    px2 = [px2, tt, tt+stepTime];
    tt = tt + stepTime;
end

%xZMP = (0:(length(ZMP)-1))*dt;
t = (0:(length(ZMP)-1)*singleSupportSamples)/singleSupportSamples*stepTime;

Pref = zeros(4,numel(t));

Pref(1,:) = pchip(px,pp,t);
Pref(2,:) = bezierApprox(px,pp,t); % use one dimensional bezier curves to approx discret zmp trajectory

sigma = 2;
sz = 30;    % length of gaussFilter vector
x = linspace(-sz / 2, sz / 2, sz);
gaussFilter = exp(-x .^ 2 / (2 * sigma ^ 2));
gaussFilter = gaussFilter / sum (gaussFilter); % normalize
Pref(3,:) = conv (stepFunc(px2,pp,t), gaussFilter, 'same');

Pref(4,:) = stepFunc(px2,pp,t);

offset = 0.25;

Pref(5,:) = generateTrajectory(5, t);

%for i = 1:length(ZMP)
    %Pref = addPref(Pref, ZMP(i), singleSupportSamples, doubleSupportSamples);
    %Pref = addZMP(Pref, ZMP(i), singleSupportSamples, doubleSupportSamples);
%end

% figure(5)
% plot(x,gaussFilter)

figure(1)
plot(t, Pref(1,:),'DisplayName','pchip')
hold on
plot(t, Pref(2,:),'x','DisplayName','bezier')
plot(t, Pref(3,:),'DisplayName','Gaussian filter')
plot(t, Pref(4,:),'DisplayName','step function')
plot(t, Pref(5,:),'DisplayName','bezier steps')
plot(px, pp,'DisplayName','input')
legend('show')
hold off

t = 0;
%J = 0;
k = [4,5];
l = 1;

figure(3)
for j = k
    U   = [];
    x   = [];
    dx  = [];
    ddx = [];
    zmp = [];
    err = (- Ks*X - F*Pref(j,1:N)') / Ki;

    for i = 1:length(Pref(j,:))-N
        P = Pref(j,i:i+N-1)';

        u = -Ki*err - Ks*X - F*P;

        X = A*X + b*u;
        y = c*X;

        U = [U, u];
        x = [x, X(1)];
        dx = [dx, X(2)];
        ddx = [ddx, X(3)];
        zmp = [zmp, y(1,1)];

        err = err + zmp(length(zmp)) - P(1,1);
        %J = J + q*(P(1,1) - zmp(length(zmp)))^2 + R*(U(length(U))-U(length(U)-1))^2;
        t = t + dt;
    end

    % without starting end stopping step
    totalU(l) = sum(U(25:end-25).^2);
    varU(l)   = var(U(25:end-25));
    
    subplot(numel(k),1,l);
    l = l+1;
    plot(x)
    hold on
    plot(dx,'g')
    plot(zmp,'r')
    plot(Pref(j,:),'k--')
    hold off
end

% figure(2)
% plot(F)
display('Summe quadrierter Steuerwerte')
display(totalU)
display('Varianz des Rucks (com)')
display(varU)
    
% save result
format('long')
A = A';
save('previewcontrol.prm','A', 'b', 'c', 'Ki', 'Ks',  'F', '-ascii');

