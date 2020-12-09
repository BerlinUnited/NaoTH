clear

% system model
z = 0.260; % m
dt = 0.010; % s
previewTime = 0.5; % s
[A, b, c, Ki, Ks, F] = previewcontrol(z, dt, previewTime);

% test
X = [0; 0; 0];
stepTime = 0.25; % s
StepSamples = stepTime / dt;
doubleSupportSamples = 10;
singleSupportSamples = StepSamples - doubleSupportSamples;
N = previewTime / dt;

%ZMP = [0, 0, -0.5, 0.1, -0.1, 0.1, 0, 0, 0, 0];
ZMP = [0, 0.05, -0.05, 0.05, -0.05, 0.05, -0.05, 0, 0, 0];

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

% try to imitate NaoDevils zmp trajectory
steps = 2;
phasen =[];
PrefNaoDevils = [];
for i = 1 : steps 
    if(mod(i,2)) % add left step
        % go to left
        for j = 0 : singleSupportSamples-1
            PrefNaoDevils = [PrefNaoDevils generateZMPNaoDevils(1,j/singleSupportSamples)];
            phasen = [phasen 1];
        end
        
        % stay left
        for j = 0 : doubleSupportSamples-1
            PrefNaoDevils = [PrefNaoDevils generateZMPNaoDevils(2,j/doubleSupportSamples)];
            phasen = [phasen 2];            
        end
    else
        
        % go right
        for j = 0 : singleSupportSamples-1
            PrefNaoDevils = [PrefNaoDevils generateZMPNaoDevils(3,j/singleSupportSamples)];
            phasen = [phasen 3];
        end
        
        % stay right
        for j = 0 : doubleSupportSamples-1
            PrefNaoDevils = [PrefNaoDevils generateZMPNaoDevils(4,j/doubleSupportSamples)];
            phasen = [phasen 4];
        end
    end
end

figure(4)
t = (0:(length(PrefNaoDevils)-1))*dt;
plot(t,PrefNaoDevils,'.',t,phasen/4*max(PrefNaoDevils))

%xZMP = (0:(length(ZMP)-1))*dt;
t = (0:(length(ZMP)-1)*singleSupportSamples)/singleSupportSamples*stepTime;

Pref = zeros(4,numel(t));

%for i = 1:length(ZMP)
    %Pref = addPref(Pref, ZMP(i), singleSupportSamples, doubleSupportSamples);
    %Pref = addZMP(Pref, ZMP(i), singleSupportSamples, doubleSupportSamples);
%end



t = 0;
%J = 0;

U   = [];
x   = [];
dx  = [];
ddx = [];
zmp = [];
err = (- Ks*X - F*Pref(1:N)') / Ki;

for i = 1:length(Pref)-N
    P = Pref(i:i+N-1)';
    
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

    
figure(3)
plot(x)
hold on
plot(dx,'g')
%plot(ddx,'g--')
plot(zmp,'r')
plot(Pref,'k--')
hold off

% figure(2)
% plot(F)

% save result
format('long')
A = A';
save('previewcontrol.prm','A', 'b', 'c', 'Ki', 'Ks',  'F', '-ascii');
