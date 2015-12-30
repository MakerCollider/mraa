#!/bin/sh
echo "Installing Edison extension board check tool.\n"
cp extboardcheck.sh /etc/
cp check5611 /usr/bin
cp check6050 /usr/bin
cp check5883 /usr/bin
cp extboard_detect.service /usr/lib/systemd/user
echo "Enabling Edison extension board check tool start up.\n"
systemctl enable /usr/lib/systemd/user/extboad_detect.service
echo "All done."
echo "Please reboot your Edison to enjoy the new MRAA of Newbreakout."
