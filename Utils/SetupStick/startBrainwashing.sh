#!/bin/bash
rm -rf ./backup
mkdir ./backup
cp -r /home/nao/naoqi/Config ./backup
cp /home/nao/bin/naoth ./backup

naoth stop

rm -rf /home/nao/Config/general
rm -f /home/nao/Config/reachability_grid.dat
rm -rf /home/nao/Config/robot_heads
rm -rf /home/nao/Config/robots
rm -rf /home/nao/Config/scheme
rm -rf /home/nao/Config/scheme.cfg
rm -f /home/nao/bin/naoth

cp -nr ./home/nao/naoqi/Config/* /home/nao/Config/
cp ./home/nao/bin/naoth /home/nao/bin/naoth

mkdir /home/nao/Media
cp ./anDieArbeit.wav /home/nao/Media

naoth start
