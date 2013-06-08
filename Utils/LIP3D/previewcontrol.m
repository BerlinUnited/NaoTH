function [A, b, c, Ki, Ks,  F] = previewcontrol(z, dt, pt)

g = 9.810; % m/s^2
R = 1;
q = 1e10;

A = [1, 1, dt, dt^2/2 - z/g;
     0, 1, dt, dt^2/2;
     0, 0, 1, dt;
     0, 0, 0, 1];

b = [dt^3/6-z*dt/g;
     dt^3/6;
     dt^2/2;
     dt];

c = [ 1, 0, 0, 0];

Q = c' * q * c;


% P = A'PA-(A'Pb)(R+b'Xb)^-1(b'PA)+Q
%[P,L,G] = dare(A,b,Q,R);
P = dare(A,b,Q,R);

%Y = A'*P*A-(A'*P*b)*(R+b'*P*b)^(-1)*(b'*P*A)+Q;
%norm(P-Y)

Mb = (R+b'*P*b)^-1*b';
K = Mb*P*A;
Ac = A - b*Mb*P*A;
Xl = -Ac'*P*[1; 0; 0; 0];
F = -K(1,1);

N = floor(pt / dt);
for i = 1:N-1
    f = Mb*Xl;
	Xl = Ac'*Xl;
	
    if f > 0
        break;
    end
    F = [F, f];
end

if N ~= length(F)
    fprintf('Warning: %d != %d\n', N, length(F));
end

A = [1, dt, dt^2/2;
     0, 1, dt;
     0, 0, 1];

b = [dt^3/6;
     dt^2/2;
     dt];

c = [ 1, 0, -z/g];

Ki = K(1);

Ks = K(2:4);
