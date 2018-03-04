clear all
% test for adding a raw file to the whistledatabase
try
    load('database.mat')
catch
    disp('no previous mat file was created')
    database = struct;
end
%whistle categories: 
    %reference_whistle_rc17
    %reference_whistle_go17
    %bhuman
    %capture_rc17


database = addWhistle(database, '../data/andyDark_8kHz_1channels.raw', 1, 8000, 'bhuman');
database = addWhistle(database, '../data/defaultWhistle_8kHz_1channels.raw', 1, 8000, 'bhuman');
database = addWhistle(database, '../data/hefeiWhistle_8kHz_1channels.raw', 1, 8000, 'bhuman');
database = addWhistle(database, '../data/lamEindhoven_8kHz_1channels.raw', 1, 8000, 'bhuman');
database = addWhistle(database, '../data/patrickWhistle_8kHz_1channels.raw', 1, 8000, 'bhuman');
database = addWhistle(database, '../data/pimmelWhistleOutdoor_8kHz_1channels.raw', 1, 8000, 'bhuman');
database = addWhistle(database, '../data/silverWhistleOutdoor_8kHz_1channels.raw', 1, 8000, 'bhuman');
disp('Saving Whistle database');
save('database.mat','database')