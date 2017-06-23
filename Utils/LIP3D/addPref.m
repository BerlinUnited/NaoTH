function Pref = addPref(Pref, v, n, dn)
if dn > 0
    s = Pref(length(Pref));
    ds = (v - s) / dn;
    for i = 1:dn
        s = s + ds;
        Pref = [Pref, s];
    end
end

for i = 1:n
    Pref = [Pref, v];
end
