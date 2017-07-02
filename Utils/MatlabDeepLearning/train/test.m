function test()

    clear all;

    load cn2.mat
    cn = convnet;

    ball = imread('../data/ball.png');

    evaluate(cn, ball);
end





