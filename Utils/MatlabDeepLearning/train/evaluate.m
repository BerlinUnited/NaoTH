function evaluate(convnet, image)
    
    layers = convnet.Layers;
    activation = image;
    
    for i = 1:numel(layers)
        
        % calculate the layer activations
        activationCN = activations(convnet, image, i);
        layer = layers(i);
        
        switch class(layer)
            case 'nnet.cnn.layer.ImageInputLayer'
                activation = calc_ImageInputLayer(layer, activation);
            case 'nnet.cnn.layer.Convolution2DLayer'
                activation = calc_convolution2D(layer, activation);
            case 'nnet.cnn.layer.ReLULayer'
                activation = cals_relu(layer, activation);
            case 'nnet.cnn.layer.MaxPooling2DLayer'
                fprintf('[%s] missing.\n',class(layer));
                continue;
            case 'nnet.cnn.layer.FullyConnectedLayer'
                activation = calc_fullyConnected(layer, activation);
            case 'nnet.cnn.layer.SoftmaxLayer'
                activation = calc_softmax(layer, activation);
            case 'nnet.cnn.layer.ClassificationOutputLayer'
                fprintf('[%s] missing.\n',class(layer));
                continue;
            otherwise
                fprintf('[unknown layer] %s', class(layers(i)));
                continue;
        end
        
        % compare the potputs
        output = reshape(activation, 1, numel(activation));
        if length(output) ~= length(activationCN)
            fprintf('[%s] wrong output dimension.\n',class(layer));
        else
            diff = max(max(abs(output - activationCN)));
            fprintf('[%s] error: %f\n',class(layer), diff);
        end
    end
end


function output = calc_ImageInputLayer(layer, input)
    averageImage = struct(layer).PrivateLayer.AverageImage;
    output = single(input) - averageImage;
end

function output = calc_convolution2D(layer, input)
    in_dim_x = size(input,1);
    in_dim_y = size(input,2);
    in_dim_z = size(input,3);
    
    weights = layer.Weights;
    bias = layer.Bias;

    stride_x  = layer.Stride(1);
    stride_y  = layer.Stride(2);
    padding_x = layer.Padding(1);
    padding_y = layer.Padding(2);

    filter_dim_x = layer.FilterSize(1);
    filter_dim_y = layer.FilterSize(2);

    out_dim_x = floor((in_dim_x - filter_dim_x + 2*padding_x)/stride_x + 1);
    out_dim_y = floor((in_dim_y - filter_dim_y + 2*padding_y)/stride_y + 1); 
    out_dim_z = size(weights,4);

    output = zeros(out_dim_x, out_dim_y, out_dim_z);
    for i = 1:out_dim_z
        
        res = zeros(out_dim_x, out_dim_y);
        b = bias(1,1,i);
        
        x_out = 1;
        for x = -padding_x + 1 : stride_x : in_dim_x + padding_x - filter_dim_x + 1
            y_out = 1;
            for y = -padding_y + 1 : stride_y : in_dim_y + padding_y - filter_dim_y + 1

                for z = 1:in_dim_z
                    for f_x = 1:filter_dim_x
                        for f_y = 1:filter_dim_y
                           if (x+f_x-1 < 1 || x+f_x-1 > in_dim_x) || (y+f_y-1 < 1 || y+f_y-1 > in_dim_y) 
                               continue;
                           else
                               res(x_out, y_out) = res(x_out, y_out) + weights(f_x,f_y,z,i)*input(x+f_x-1, y+f_y-1, z);
                           end
                        end
                    end
                end % z
                
                res(x_out, y_out) = res(x_out, y_out) + b;
                y_out = y_out + 1;
            end
            x_out = x_out + 1;
        end
        
        output(:,:,i) = res;
    end
end

function output = cals_relu(~, input)
    output = input;
    output(output < 0) = 0;
end

function output = calc_fullyConnected(layer, input)
    
    in_dim_x = size(input,1);
    in_dim_y = size(input,2);
    in_dim_z = size(input,3);
    
    out_rows = layer.OutputSize;
    
    weights  = layer.Weights;
    bias     = layer.Bias;
    
    output = zeros(1, out_rows);
    
    for out = 1:out_rows

        % iterate over input
        for z = 1 : in_dim_z
            for x = 1 : in_dim_x
                for y = 1 : in_dim_y
                    idx = (z-1)*(in_dim_y*in_dim_x) + (x-1)*in_dim_y + y;
                    output(out) = output(out) + weights(out,idx)*input(y,x,z);
                end
            end
        end
        output(out) = output(out) + bias(out);
    end
    
end

function output = calc_softmax(layer, input)
    exponents = input - max(input);
    expX = exp(exponents);
    output = expX./sum(expX);
end