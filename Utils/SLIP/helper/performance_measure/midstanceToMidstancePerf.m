function p = midstanceToMidstancePerf(y0, ieout, yeout, teout, tdpout, yend)
   % performance index from paper for quater step optimization
    initial_state = [y0(1), y0(3), y0(5), y0(2), y0(4)]; % [x,y,z,x',y']
    
    if any(ieout == 1) % fallen
        p = 10;
        return;
    end
    
    if ~any(ieout == 3) % no lowest hight event occured before midstance
        p = 10;
        return;
    end
    
    % find tdp of right foot during midstance
    idx = find(ieout == 2, 1); % will get only one idx at most (terminating event)
    if(isempty(idx))
        final_state = yend([1 3 5 2 4]); % no midstance event occured -> fallen
    else
        idx2 = find((tdpout(:,1) < teout(idx) & (tdpout(:,2) == 0)), 1, 'last');
        if(isempty(idx2))
            final_state = yend([1 3 5 2 4]); % if no tdp with right foot during midstance -> flying phase -> use projected com
        else
            final_state = yeout(idx,[1 3 5 2 4]);
            final_state(1) = final_state(1) - tdpout(idx2,3);
            final_state(2) = final_state(2) - tdpout(idx2,5);
        end
    end
     
    p =  norm(initial_state([1 3 4 5])-final_state([1 3 4 5]))^2 ...
        + (initial_state(2) + final_state(2))^2;
end

