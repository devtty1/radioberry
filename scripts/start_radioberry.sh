#!/bin/bash
GPIO_IN=18
GPIO_OUT=4
IN_EXIST=0
OUT_EXIST=0
LAST_IN_VAL=0

[ -d /sys/class/gpio/gpio$GPIO_IN ] && IN_EXIST=1
[ -d /sys/class/gpio/gpio$GPIO_OUT ] && OUT_EXIST=1

[ $IN_EXIST -eq 0 ] && echo $GPIO_IN > /sys/class/gpio/export
if [ $OUT_EXIST -eq 0 ]; then
	echo $GPIO_OUT > /sys/class/gpio/export
	echo out > /sys/class/gpio/gpio$GPIO_OUT/direction
fi

while [ 1 ]; do
	IN_VAL=`cat /sys/class/gpio/gpio$GPIO_IN/value`
	# high flank detected.
	if [ $IN_VAL -gt $LAST_IN_VAL ]; then
		echo "+++++++++++++++++ start at `date` +++++++++++++++++" >> /var/log/radioberry.log
		# turn back light on
		echo 1 > /sys/class/gpio/gpio$GPIO_OUT/value
		radioberry >> /var/log/radioberry.log &
	fi

	# low flank detected.
	if [ $IN_VAL -lt $LAST_IN_VAL ]; then
		echo 0 > /sys/class/gpio/gpio$GPIO_OUT/value
		# turn back light off
		killall -s SIGINT radioberry
		echo "+++++++++++++++++ stop at `date` +++++++++++++++++" >> /var/log/radioberry.log
	fi

	LAST_IN_VAL=$IN_VAL

	sleepenh 0.4 > /dev/null
done

