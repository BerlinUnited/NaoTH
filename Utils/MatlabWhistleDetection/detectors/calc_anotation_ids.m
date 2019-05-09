% function  new_annotations = maxcrosscorr_detector(capture_data
%anot = capture_database.go18_recordings(6).annotations;

function anot = calc_anotation_ids(annotation)
    anot = annotation;
    last1 = 0;
    count1 = 0;
    last2 = 0;
    count2 = 0;
    for i = 1:length(annotation)
        if annotation(i, 1) > 0
            if last1 == 0
                count1 = count1 + 1;
                disp(num2str(i)+ " 1")
            end
            anot(i, 1) = count1;
        end
        if annotation(i, 2) > 0
            if last2 == 0
                count2 = count2 + 1;
                disp(num2str(i)+ " 2")
            end
            anot(i, 2) = count2;
        end
        last1 = annotation(i, 1);
        last2 = annotation(i, 2);
    end
end









