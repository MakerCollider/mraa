#!/bin/sh

BOARDNAME="/etc/mc_extboard_name"

EDIBOT="mc_newbreakout"
check_edibot()
{
	check6050
	local mpu=$?
	check5883
	local cmp=$?
	check5611
	local baro=$?
	
	if [ $mpu -eq '1'  -a  $cmp -eq '1' -a $baro -eq '1' ] ; then
		echo "Edibot extension board confirmed."
		echo $EDIBOT > $BOARDNAME
	fi

	arr=( 3 7 6 5 4 2 1 0 8 9 10 11 12 13 14 15 )
	echo "test IO are (${arr[@]})"
	echo "turn on all leds."
	for i in ${arr[@]}
	do
		echo "IO led on:" $i
		mraaio $i 0
	done
}

rm -rf $BOARDNAME
check_edibot
