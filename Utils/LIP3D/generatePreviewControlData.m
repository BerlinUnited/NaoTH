clear

format('long')

dt = 0.010; % s
topdir = sprintf('%d', round(dt*1000));
mkdir(topdir)

for previewTime = 0.1:0.1:1.0 %s
  previewTimeDir = strcat(topdir, '/', sprintf('%d', round(previewTime*1000)));
  mkdir(previewTimeDir)
  for z = 0.2:0.001:0.3
    [A, b, c, Ki, Ks,  F] = previewcontrol(z, dt, previewTime);
    
    filename = strcat(previewTimeDir, '/', sprintf('%d.prm', round(z*1000)));
    lenF = length(F);
	A = A';
    save(filename,'A', 'b', 'c', 'Ki', 'Ks', 'lenF', 'F', '-ascii');
    fprintf('%s saved!\n', filename);
  end
end
