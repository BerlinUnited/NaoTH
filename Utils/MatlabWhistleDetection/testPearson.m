x = [1 2 3 4 5 6 7 8 9 10];
y = x*2;

R = corrcoef(x,y)
pearson_corr = R(2,1)