function evaluate(convnet, image)
    
    layers = convnet.Layers;
    activation = image;
    
    for i = 1:numel(layers)
        
        layer = layers(i);
        
        switch class(layer)
            case 'nnet.cnn.layer.ImageInputLayer'
                activation = calc_ImageInputLayer(layer, activation);
            case 'nnet.cnn.layer.Convolution2DLayer'
                activation = calc_convolution2D(layer, activation);
            case 'nnet.cnn.layer.ReLULayer'
                activation = cals_relu(layer, activation);
            case 'nnet.cnn.layer.MaxPooling2DLayer'
                activation = cals_MaxPolling2D(layer, activation);
            case 'nnet.cnn.layer.FullyConnectedLayer'
                activation = calc_fullyConnected(layer, activation);
            case 'nnet.cnn.layer.SoftmaxLayer'
                activation = calc_softmax(layer, activation);
            case 'nnet.cnn.layer.ClassificationOutputLayer'
                activation= calc_classificationOutput(layer, activation);
            case 'nnet.cnn.layer.BatchNormalizationLayer'
                activation= calc_batchNormalizationLayer(layer, activation);
            otherwise
                fprintf('[unknown layer] %s\n', class(layers(i)));
                continue;
        end
        
        % calculate the layer activations
        activationCN = activations(convnet, image, i);
        
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
    weights = layer.Weights;
    bias = layer.Bias;
    
    in_dim_x = size(input,1);
    in_dim_y = size(input,2);
    
    stride_x  = layer.Stride(1);
    stride_y  = layer.Stride(2);
    padding_x = layer.PaddingSize(1);
    padding_y = layer.PaddingSize(2);

    filter_dim_x = layer.FilterSize(1);
    filter_dim_y = layer.FilterSize(2);
    channels = layer.NumChannels;

    out_dim_x = floor((in_dim_x - filter_dim_x + 2*padding_x)/stride_x + 1);
    out_dim_y = floor((in_dim_y - filter_dim_y + 2*padding_y)/stride_y + 1); 
    out_dim_z = size(weights,4);

    output = zeros(out_dim_x, out_dim_y, out_dim_z);
    for z_out = 1:out_dim_z
        
        x_out = 1;
        for x = -padding_x + 1 : stride_x : in_dim_x + padding_x - filter_dim_x + 1
            y_out = 1;
            for y = -padding_y + 1 : stride_y : in_dim_y + padding_y - filter_dim_y + 1
                
                res = 0;
                for c = 1:channels
                    for f_x = 1:filter_dim_x
                        for f_y = 1:filter_dim_y
                           if (x+f_x-1 < 1 || x+f_x-1 > in_dim_x) || (y+f_y-1 < 1 || y+f_y-1 > in_dim_y) 
                               continue;
                           else
                               res = res + weights(f_x,f_y,c,z_out)*input(x+f_x-1, y+f_y-1, c);
                           end
                        end
                    end
                end % c
                
                output(x_out, y_out,z_out) = res + bias(1,1,z_out);
                y_out = y_out + 1;
            end
            x_out = x_out + 1;
        end
    
        
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
    
    out_dim = layer.OutputSize;
    
    weights  = layer.Weights;
    bias     = layer.Bias;
    
    output = zeros(1, out_dim);
    
    for out = 1:out_dim
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

function output = calc_softmax(~, input)
    exponents = input - max(input);
    expX = exp(exponents);
    output = expX./sum(expX);
end

function output = calc_classificationOutput(~,input)
    output = input;
end

function output = cals_MaxPolling2D(layer, input)

classify()
    in_dim_x = size(input,1);
    in_dim_y = size(input,2);
    in_dim_z = size(input,3);

    stride_x  = layer.Stride(1);
    stride_y  = layer.Stride(2);
    padding_x = layer.Padding(1);
    padding_y = layer.Padding(2);
    pool_dim_x = layer.PoolSize(1);
    pool_dim_y = layer.PoolSize(2);
    
    out_dim_x = floor((in_dim_x - pool_dim_x + 2*padding_x)/stride_x + 1);
    out_dim_y = floor((in_dim_y - pool_dim_y + 2*padding_y)/stride_y + 1);
    
    out_dim_z = in_dim_z;
    
    output = zeros(out_dim_x, out_dim_y, out_dim_z);
    
    for z_out = 1:out_dim_z 
        x_out = 1;
        for x = -padding_x + 1 : stride_x : in_dim_x + padding_x - pool_dim_x + 1
            y_out = 1;
            for y = -padding_y + 1 : stride_y : in_dim_y + padding_y - pool_dim_y + 1
              
                    output(x_out, y_out, z_out) = input(x,y,z_out);
                    
                    for f_x = 1:pool_dim_x
                        for f_y = 1:pool_dim_y
                           if (x+f_x-1 < 1 || x+f_x-1 > in_dim_x) || (y+f_y-1 < 1 || y+f_y-1 > in_dim_y) || (f_x == 1 && f_y == 1)
                               continue;
                           else
                               if(output(x_out, y_out, z_out) < input((x+f_x-1), (y+f_y-1), z_out))
                                    output(x_out, y_out, z_out) = input((x+f_x-1), (y+f_y-1), z_out);
                               end
                               
                           end
                        end % f_y
                    end % f_x
                y_out = y_out + 1;
            end % y
            x_out = x_out + 1;
        end % x
    end % z_out
   
end


function output = calc_batchNormalizationLayer(layer, input)
    e = sqrt(layer.TrainedVariance + layer.Epsilon);
    weights = layer.Scale./e;
    bias = layer.Offset - layer.TrainedMean.*layer.Scale./e;

    %output = input.*weights + bias;
    
    in_dim_x = size(input,1);
    in_dim_y = size(input,2);
    in_dim_z = size(input,3);
    
    output = zeros(in_dim_x, in_dim_y, in_dim_z);
    
    % iterate over input
    for x = 1 : in_dim_x
        for y = 1 : in_dim_y
            for z = 1 : in_dim_z
                output(x,y,z) = input(x,y,z)*weights(z) + bias(z);
            end
        end
    end
    
    %output = (input - layer.TrainedMean)./sqrt(layer.TrainedVariance + layer.Epsilon);
    %output = output.*layer.Scale + layer.Offset;
end
