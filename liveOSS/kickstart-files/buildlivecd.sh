#!/bin/sh

livecd_label=OpenSpeedShop-2.0.1-x86_64-Live
livecd_kickstart_file=livecd-fedora-15-x86_64-openss.ks

#You must run livecd-creator as root...
if [ `whoami` != "root" ]; then
    echo "remember: you must be root to run livecd-creator..."
fi

# su -c "livecd-creator -d -v --config=$livecd_kickstart_file \
su -c "livecd-creator --config=$livecd_kickstart_file --fslabel=$livecd_label" root
