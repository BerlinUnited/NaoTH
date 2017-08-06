function Pref = addZMP(Pref, v, n, dn)

t = (1:n)/n;
x = [0,0.2,1];
y = [Pref(end),v*0.8,v];
%spline(x,y,xx);

Pref = [Pref, pchip(x,y,t)];
