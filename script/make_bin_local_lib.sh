#!/bin/sh
bin_make=`pwd`
cd ..
mkdir bin -p
cp team/src/* bin/ -r
cd bin
rm auto.sh *.o *.cpp *.h chain_action goalie move_def move_off neck roles setplay -r
rm train*
rm Makefile*
rm debug*
rm start.sh.in start-offline.sh
rm pimentao_verde_trainer
#sed -i 's/use_sync_mode : on/#/g' player.conf
mkdir lib -p
cp ${bin_make}/../pimentao_verde-soccer-simulation-lib/build/rcsc/librcsc.so* lib/ 2>/dev/null || cp $HOME/local/pimentao_verde_lib/lib/librcsc* lib/
cp ../script/start .
cp ../script/kill .
cd ..
mv bin Pimentao_Verde -f
tar -czvf Pimentao_Verde.tar.gz Pimentao_Verde
rm Pimentao_Verde -rf
