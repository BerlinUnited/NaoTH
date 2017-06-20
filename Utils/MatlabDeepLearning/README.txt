createConvolutionCode.m generates a cpp file which calculates a convolution for ball.png with a predefined filter

For Training:
use the 01_set_16x16.tar file from
/vol/repl261-vol4/naoth/logs/BallDetection/2016-ball-detection

It contains labeled 16x16 patches sorted according to the logfile they are from.

copy the contents of this archiv to data - for testing run trainTest01.m file 


TODO
- describe where trainingsdata can be found
- explain how training can be done in matlab

- which layer still need implementing

- Issues with convolution layer export
	-> introduce matlabOffset as variable
	-> implement stride as parameter which defines the dimensions of the output matrix
	