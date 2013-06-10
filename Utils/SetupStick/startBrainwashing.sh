#!/bin/bash
rm -rf ./backup
mkdir ./backup
cp -r /home/nao/naoqi/Config ./backup
cp /home/nao/bin/naoth ./backup

naoth stop

rm -rf /home/nao/Config/*
rm -f /home/nao/bin/naoth

cp -r ./home/nao/naoqi/Config/* /home/nao/Config/
cp ./home/nao/bin/naoth /home/nao/bin/naoth

mkdir /home/nao/Media
cp ./anDieArbeit.wav /home/nao/Media

naoth start