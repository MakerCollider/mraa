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
}

rm -rf $BOARDNAME
check_edibot
