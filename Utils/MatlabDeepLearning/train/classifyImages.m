function classifyImages(convnet, srcDir, tgtDir, useImgReader)
    % tgtDir = '..\data\16x16_bw\2016-06-29-patches-play_preclassified\';
    % srcDir = '..\data\16x16_bw\2016-06-29-patches-play\noball';
    countLabel = size(convnet.Layers(end,1).ClassNames);

    for i = 1:countLabel
        mkdir (tgtDir, convnet.Layers(end,1).ClassNames{i});
    end

    if useImgReader
        digitData = imageDatastore(srcDir, ...
            'IncludeSubfolders',true,'LabelSource','foldernames', 'ReadFcn',@imgReader);
    else
        digitData = imageDatastore(srcDir, ...
            'IncludeSubfolders',true,'LabelSource','foldernames');
    end

    YTest = classify(convnet,digitData);
    files = digitData.Files; % this is much faster in the for loop
    for i = 1:length(YTest)
        fileOld = files{i};
        pathOld = strsplit(fileOld,{'/','\'});
        fileName = pathOld(end);
        fileNew = tgtDir + string(YTest(i)) + '\' + fileName;
        copyfile(char(fileOld), char(fileNew));
    end

end
