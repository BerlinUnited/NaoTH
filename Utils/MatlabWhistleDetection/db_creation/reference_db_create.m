clear variables
clc
% adds a raw file to the whistle reference database
% TODO does not check if file is already inserted
try
    load('../data/reference_database.mat')
    disp('INFO: using a previously created database')
catch
    disp('INFO: no previous mat file was found')
    reference_database = struct;
end

% TODO the paths to the raw reference files should be collected differently

% bhuman reference whistles
reference_database = addWhistle(reference_database, '../data/bhuman_references/andyDark_8kHz_1channels.raw', 1, 8000, 'bhuman');
reference_database = addWhistle(reference_database, '../data/bhuman_references/defaultWhistle_8kHz_1channels.raw', 1, 8000, 'bhuman');
reference_database = addWhistle(reference_database, '../data/bhuman_references/hefeiWhistle_8kHz_1channels.raw', 1, 8000, 'bhuman');
reference_database = addWhistle(reference_database, '../data/bhuman_references/lamEindhoven_8kHz_1channels.raw', 1, 8000, 'bhuman');
reference_database = addWhistle(reference_database, '../data/bhuman_references/patrickWhistle_8kHz_1channels.raw', 1, 8000, 'bhuman');
reference_database = addWhistle(reference_database, '../data/bhuman_references/pimmelWhistleOutdoor_8kHz_1channels.raw', 1, 8000, 'bhuman');
reference_database = addWhistle(reference_database, '../data/bhuman_references/silverWhistleOutdoor_8kHz_1channels.raw', 1, 8000, 'bhuman');

% kln17 reference whistles
reference_database = addWhistle(reference_database, '../data/kln17_references/andyDark_8kHz_1channels.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/black_loud_begin.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/black_loud_end.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/black_loud_middle.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/black_low_begin.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/black_low_end.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/black_low_middle.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/black_medium_begin.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/black_medium_end.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/black_medium_middle.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/defaultWhistle_8kHz_1channels.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/GO17-Devils_begin.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/GO17-Devils_end.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/GO17-Devils_middle.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/grey_loud_begin.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/grey_loud_end.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/grey_loud_middle.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/grey_low_begin.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/grey_low_end.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/grey_low_middle.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/grey_medium_begin.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/grey_medium_end.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/grey_medium_middle.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/hefeiWhistle_8kHz_1channels.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/lamEindhoven_8kHz_1channels.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/patrickWhistle_8kHz_1channels.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/pimmelWhistleOutdoor_8kHz_1channels.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/ref_yellow.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/silverWhistleOutdoor_8kHz_1channels.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/yellow_loud_begin.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/yellow_loud_end.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/yellow_loud_middle.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/yellow_low_begin.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/yellow_low_end.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/yellow_low_middle.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/yellow_medium_begin.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/yellow_medium_end.raw', 1, 8000, 'kln17');
reference_database = addWhistle(reference_database, '../data/kln17_references/yellow_medium_middle.raw', 1, 8000, 'kln17');

% rc17 reference whistles
reference_database = addWhistle(reference_database, '../data/rc17_references/andyDark_8kHz_1channels.raw', 1, 8000, 'rc17');
reference_database = addWhistle(reference_database, '../data/rc17_references/black_medium_begin.raw', 1, 8000, 'rc17');
reference_database = addWhistle(reference_database, '../data/rc17_references/black_medium_end.raw', 1, 8000, 'rc17');
reference_database = addWhistle(reference_database, '../data/rc17_references/yellow_medium_begin.raw', 1, 8000, 'rc17');
reference_database = addWhistle(reference_database, '../data/rc17_references/yellow_medium_end.raw', 1, 8000, 'rc17');

disp('Saving Whistle database');
save('reference_database.mat','reference_database')