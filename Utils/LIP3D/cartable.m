clear

% system model
z = 0.260; % m
dt = 0.010; % s
previewTime = 0.5; % s
[A, b, c, Ki, Ks,  F] = previewcontrol(z, dt, previewTime);

% test
X = [0; 0; 0];
Pref = X(1, 1);
stepTime = 0.25; % s
StepSamples = stepTime / dt;
doubleSupportSamples = 0;
singleSupportSamples = StepSamples - doubleSupportSamples;
N = previewTime / dt;

ZMP = [0, 0, -0.5, 0.1, -0.1, 0.1, 0, 0, 0, 0];

for i = 1:length(ZMP)
    Pref = addPref(Pref, ZMP(i), singleSupportSamples, doubleSupportSamples);
end

U = [];
x = [];
dx = [];
ddx = [];
zmp = [];
err = (- Ks*X - F*Pref(1:N)') / Ki;

t = 0;
J = 0;
for i = 1:length(Pref)-N
    P = Pref(i:i+N-1)';

    u = -Ki*err - Ks*X - F*P;

    X = A*X + b*u;
    y = c*X;

    U = [U, u];
    x = [x, X(1,1)];
    dx = [dx, X(2,1)];
    ddx = [ddx, X(3,1)];
    zmp = [zmp, y(1,1)];
    err = err + zmp(length(zmp)) - P(1,1);
    %J = J + q*(P(1,1) - zmp(length(zmp)))^2 + R*(U(length(U))-U(length(U)-1))^2;
    t += dt;
end

figure
plot(F)

figure
hold on
plot(x)
plot(dx,'g')
%plot(ddx,'g--')
plot(zmp,'r')
plot(Pref,'k--')
    
% save result
format('long')
A = A';
save('previewcontrol.prm','A', 'b', 'c', 'Ki', 'Ks',  'F', '-ascii');
