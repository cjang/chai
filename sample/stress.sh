#!/bin/sh

cp /dev/null $1.tmp

date > $1.tmp

CNT=0

while [ $CNT -lt 50 ]
do

    echo -n .
    ./$1 >> $1.tmp 2>&1
    echo >> $1.tmp
    CNT=`expr $CNT + 1`

    RES=`expr $CNT % 10`
    if [ $RES -eq 0 ]
    then
        echo $CNT
    fi

done

date >> $1.tmp

grep 'Segmentation' $1.tmp
