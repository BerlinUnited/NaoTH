clear variables
clc
%% init defaults
addpath('../common')
secret()

try
    load(reference_database_path)
catch
    disp('ERROR: Could not load reference database')
    return
end
%%
% the idea is to crosscorrelate each reference whistle to see which closely
% resemble another one and which ones are different. In the end we want to
% have a minimal set of reference whistles to compare

% get the go18 reference whistles
ref = reference_database.go18;
crosscor_matrix = zeros(length(ref),length(ref));
for i = 1:length(ref)
    for j = 1:length(ref)
        % calculate crosscorrelation
        first = ref(i).rawData;
        second = ref(j).rawData;
        [acor,lag] = xcorr(first,second);
        [maxV,I] = max(abs(acor));
        crosscor_matrix(i,j) = maxV;
        
        if i < j
            crosscor_matrix(i,j) = 0;
        end
        if i == j
            crosscor_matrix(i,j) = 1;
        end
    end
end