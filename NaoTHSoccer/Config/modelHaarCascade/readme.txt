lbp1.xml 2016-06-23-experiment-01 -numPos 1000 -numNeg 600 -numStages 20 -maxFalseAlarmRate 0.5
lbp2.xml 2016-06-24-experiment-02 -numPos 1000 -numNeg 600 -numStages 20 -maxFalseAlarmRate 0.5
lbp3.xml 2016-06-23-experiment-01 -numPos 1000 -numNeg 2000 -numStages 16 -maxFalseAlarmRate 0.5
lbp4.xml 2016-06-26-experiment-close-scaled -numPos 1000 -numNeg 2000 -numStages 16 -precalcValBufSize 2024 -precalcIdxBufSize 2024 -featureType LBP -minHitRate 0.995 -maxFalseAlarmRate 0.5
lbp5.xml 2016-06-27-experiment-merge-01-close-scaled -numPos 1000 -numNeg 2000 -numStages 16 -precalcValBufSize 4024 -precalcIdxBufSize 4024 -featureType LBP -minHitRate 0.995 -maxFalseAlarmRate 0.5
lbp6.xml 2016-06-27-experiment-merge-01-close-scaled -numPos 1500 -numNeg 5000 -numStages 16 -precalcValBufSize 16024 -precalcIdxBufSize 16024 -featureType LBP -minHitRate 0.995 -maxFalseAlarmRate 0.5
lbp8.xml -numPos 2500 -numNeg 2000 -numStages 20 -precalcValBufSize 32024 -precalcIdxBufSize 32024 -featureType LBP -minHitRate 0.995 -maxFalseAlarmRate 0.5 -w 12 -h 12 -numThreads 12
lbp9.xml -numPos 2500 -numNeg 4000 -numStages 20 -precalcValBufSize 32024 -precalcIdxBufSize 32024 -featureType LBP -minHitRate 0.995 -maxFalseAlarmRate 0.5 -w 12 -h 12 -numThreads 12
lbp10.xml -numPos 4500 -numNeg 3000 -numStages 20 -precalcValBufSize 32024 -precalcIdxBufSize 32024 -featureType LBP -minHitRate 0.995 -maxFalseAlarmRate 0.5 -w 12 -h 12 -mode ALL -numThreads 12
- lbp11.xml -numPos 7600 -numNeg 4000 -numStages 20 -precalcValBufSize 32024 -precalcIdxBufSize 32024 -featureType LBP -minHitRate 0.995 -maxFalseAlarmRate 0.5 -w 12 -h 12 -mode ALL -numThreads 12

haar1.xml 2016-06-23-experiment-01 -numPos 1000 -numNeg 600 -numStages 20 -maxFalseAlarmRate 0.5
haar2.xml 2016-06-26-experiment-close-scaled -numPos 1000 -numNeg 2000 -numStages 16 -precalcValBufSize 4024 -precalcIdxBufSize 4024 -featureType HAAR -minHitRate 0.995 -maxFalseAlarmRate 0.5
haar3.xml -numPos 2000 -numNeg 1000 -numStages 20 -precalcValBufSize 32024 -precalcIdxBufSize 32024 -featureType HAAR -minHitRate 0.995 -maxFalseAlarmRate 0.5 -w 12 -h 12 -mode ALL

haar6.xml 2016-06-29-experiment-outdoor-merge -numPos 4500 -numNeg 3000 -numStages 20 -precalcValBufSize 32024 -precalcIdxBufSize 32024 -featureType HAAR -minHitRate 0.995 -maxFalseAlarmRate 0.5 -w 12 -h 12 -mode ALL -numThreads 12

haar7.xml 2016-06-29-experiment-outdoor-merge-mega -bg negative.txt -numPos 7500 -numNeg 4000 -numStages 20 -precalcValBufSize 32024 -precalcIdxBufSize 32024 -featureType HAAR -minHitRate 0.995 -maxFalseAlarmRate 0.5 -w 12 -h 12 -mode ALL -numThreads 1

haar9.xml 2016-06-30-experiment-outdoor-merge-better

haar10.xml 2016-06-30-experiment-outdoor-merge-better -numPos 4500 -numNeg 3000 -numStages 23 -precalcValBufSize 32024 -precalcIdxBufSize 32024 -featureType HAAR -minHitRate 0.995 -maxFalseAlarmRate 0.5 -w 12 -h 12 -mode ALL -numThreads 12