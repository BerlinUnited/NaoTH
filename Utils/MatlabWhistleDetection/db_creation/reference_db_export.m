clear all
clc
%% Example of how to get capture data and apply a detector
load('../data/reference_database.mat')

%% Get the reference categories
ref_bhuman = reference_database.bhuman;
ref_kln17= reference_database.kln17;
ref_rc17= reference_database.rc17;

for i=1:length(ref_bhuman)
    %TODO export the recordings as wav in the correct folder structure
    %TODO export the spectrum as dat in the correct folder structure
    
    %for spectrum export the exportSpectrum.m script can be used
end

for i=1:length(ref_kln17)
    %TODO export the recordings as wav in the correct folder structure
    %TODO export the spectrum as dat in the correct folder structure
    
    %for spectrum export the exportSpectrum.m script can be used
end

for i=1:length(ref_rc17)
    %TODO export the recordings as wav in the correct folder structure
    %TODO export the spectrum as dat in the correct folder structure
    
    %for spectrum export the exportSpectrum.m script can be used
end