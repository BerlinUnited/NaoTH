function showLayer(convnet, layer)

    for ch = 1:convnet.Layers(layer).NumChannels
        % Extract the first convolutional layer weights
        w = convnet.Layers(layer).Weights(:,:,ch,:);
        % rescale and resize the weights for better visualization
        w = mat2gray(w);
        w = imresize(w, [100 100]);

        figure('name', 'Layer ' + string(layer) + ',Channel ' + string(ch))
        montage(w)
    end
end