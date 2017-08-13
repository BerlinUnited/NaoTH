function quality(convnet, netName, datasetPath)
%     datasetPath = {...
%         '../data/augmented/test1'
%         };

    dataTest = imageDatastore(datasetPath, ...
            'IncludeSubfolders',true,...
            'LabelSource','foldernames', 'ReadFcn',@imgReader);

    disp('labels in dataset:')
    countLabel = dataTest.countEachLabel;    
    disp(countLabel)

    disp('running classification')

    [YTest, ~] = classify(convnet,dataTest);
    TTest = dataTest.Labels;
    % 
    % TestBalls = TTest(TTest == 'ball');
    % TestNoballs = TTest(TTest == 'noball');

    precision = sum(YTest == TTest)/numel(TTest);

    disp({'Precision: ', precision})

    test = zeros(2, numel(TTest));
    pred = zeros(2, numel(YTest));
    for i = 1:numel(TTest)
        if TTest(i) == 'ball'
            test(1, i) = 1;
        else
            test(2, i) = 1;
        end
    end
    for i = 1:numel(YTest)
        if YTest(i) == 'ball'
            pred(1, i) = 1;
        else
            pred(2, i) = 1;
        end

    end

    disp('generating confusion matrix')
    figure
    plotconfusion(test, pred, netName)


end
