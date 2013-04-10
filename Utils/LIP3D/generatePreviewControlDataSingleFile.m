clear
format('long')

% timestep
dt = 0.010; % s ~10ms

% time to preview in the future
previewTime = 1.0; %s

% number of preview steps, i.e., length of F
N = floor(previewTime / dt);

% discretization of hip height
h_min = 0.2; %in m
h_max = 0.3; %in m
dh = 0.001; %in m

% number of steps
Nh = (h_max - h_min) / dh + 1;

% create a new file
paramFileName = sprintf('previewControl%d.prm', round(previewTime*1000));
save(paramFileName, 'N', 'Nh', '-ascii');

for z = h_min:dh:h_max
	[A, b, c, Ki, Ks,  F] = previewcontrol(z, dt, previewTime);
	h = z*1000; % h in mm
	save(paramFileName, 'h', 'Ki', 'Ks', 'F', '-ascii', '-append')
end
