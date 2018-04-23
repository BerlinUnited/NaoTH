function [fs, annot] = readAudacityProjectFile(filePath, data, showFigure)
    fs = 0;
    annot = [];
    
    filePathAup = erase(filePath,'.raw');
    filePathAup = strcat(filePathAup, '.aup');
    
    if exist(filePathAup, 'file') == 2
        aup = fileread(filePathAup);

        expr = 'projname=".*?"';
        projname = regexp(aup, expr, 'match');
        projname = erase(projname{1},'projname=');
        projname = erase(projname,'_data');
        projname = erase(projname,'"');

        expr = 'rate=".*?"';
        rate = regexp(aup, expr, 'match');
        rate = erase(rate{1},'rate=');
        fs = str2num(erase(rate,'"'));

%         fileID = fopen (filePath, 'r');
%             data = fread(fileID, 'int16', 'ieee-le');
%             fs = rate;
%         fclose(fileID);
 
        expr = '<label .*?/>';
        labels = regexp(aup, expr, 'match')';

        if ~isempty(labels)
            labels = erase(labels,'<label ');
            labels = erase(labels,'t=');
            labels = erase(labels,'t1=');
            labels = erase(labels,'title=');
            labels = erase(labels,'/>');
            labels = replace(labels,'" "', '<|>');
            labels = erase(labels,'"');
            labels = split(labels,'<|>',2);    
            labelsSize = size(labels, 1);

            dataSize = length(data);

            annot = zeros(dataSize,2);    
            for i = 1:labelsSize
                a = int64(str2double(labels{i, 1}) * fs)+1;
                b = int64(str2double(labels{i, 2}) * fs)+1;
                if strcmpi(labels{i, 3}, 'whistle') || isempty(labels{i, 3})
                    annot(a:b, 1) = 1;
                else
                    annot(a:b, 2) = 1;
                end
            end

            if showFigure
                whistle = zeros(dataSize, 1);
                other = zeros(dataSize, 1);
                no = zeros(dataSize, 1);


                for i = 1:dataSize
                    if annot(i, 1) == 1
                       whistle(i) = data(i); 
                    elseif annot(i, 2) == 1
                       other(i) = data(i);
                    else
                       no(i) = data(i);
                    end

                end
                figure('name', strcat(projname, '.raw'))
                subplot(2,1,1)
                hold on
                plot(data)
                plot(30000 * annot(:, 2), 'c-')
                plot(30000 * annot(:, 1), 'r-')
                hold off

                subplot(2,1,2)
                hold on
                plot(other, 'c-')
                plot(whistle, 'r-')
                plot(no)
                hold off
            end
        end
        
    end

end