#!/bin/sh
echo "Installing Edison extension board check tool.\n"
cp extboardcheck.sh /etc/
cp check5611 /usr/bin
cp check6050 /usr/bin
cp check5883 /usr/bin
cp mraaio /usr/bin

systemctl disable extboard_detect
cp extboard_detect.service /etc/systemd/system/
echo "Enabling Edison extension board check tool start up.\n"
systemctl enable extboard_detect
systemctl restart extboard_detect
echo "All done."
