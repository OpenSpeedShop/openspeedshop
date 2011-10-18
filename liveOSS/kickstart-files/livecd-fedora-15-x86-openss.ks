%include livecd-fedora-15-base-openss.ks

%packages
@graphical-internet
@gnome-desktop
NetworkManager-vpnc
NetworkManager-openvpn

# OpenSpeedShop-specific
python
python-devel
qt3
qt3-devel
libdwarf
libunwind
sqlite
binutils
papi
libmonitor
mrnet
dyninst
openspeedshop
toyprograms
webtutorial

# save some space
#-*.i686
-openvpn
-gnome-bluetooth*
-pulseaudio-module-bluetooth
-bluez*
-wireless-tools*
-empathy
-telepathy-gabble
-telepathy-idle
-telepathy-filesystem
-telepathy-glib
-python-telepathy
-telepathy-butterfly
-telepathy-haze
-telepathy-farsight
-telepathy-salut
-telepathy-mission-control
#-ImageMagick
-samba-client
-mpage
-sox
-hplip
-hpijs
-numactl
-isdn4k-utils
-autofs
# smartcards won't really work on the livecd.
-coolkey
-ccid
-wget

# qlogic firmwares
-ql2100-firmware
-ql2200-firmware
-ql23xx-firmware
-ql2400-firmware

# scanning takes quite a bit of space :/
-xsane
-xsane-gimp
-sane-backends

# dictionaries are big
-aspell-*
-man-pages-*
-scim-tables-*
-wqy-bitmap-fonts
-dejavu-dup
-dejavu-fonts-experimental
-dejavu-fonts
-hunspell-*
-words


# more fun with space saving 
-scim-lang-chinese
scim-chewing
scim-pinyin

# save some space
-ibus-pinyin-open-phrase
-gnome-user-docs
-gimp-help
-evolution-help
-autofs
-nss_db
-vino
#-evolution TODO: need to mod the gnome panel...
-ekiga
-pidgin
-transmission-common
-transmission-gtk
-tomboy
# new deletions
-cjkuni-fonts-common
-cjkuni-ukai-fonts
-cjkuni-uming-fonts
-thunderbird
-eclipse-*
-rhythmbox
-valgrind
-digikam
-ghostscript
-ghostscript-cups
-ghostscript-fonts



%end

%post
#cat >> /etc/rc.d/init.d/fedora-live << EOF
cat >> /etc/rc.d/init.d/livesys << EOF
# disable screensaver locking
gconftool-2 --direct --config-source=xml:readwrite:/etc/gconf/gconf.xml.defaults -s -t bool /apps/gnome-screensaver/lock_enabled false >/dev/null
# set up timed auto-login for after 1 second
sed -i -e 's/\[daemon\]/[daemon]\nTimedLoginEnable=true\nTimedLogin=openssuser\nTimedLoginDelay=1/' /etc/gdm/custom.conf
if [ -e /usr/share/icons/hicolor/96x96/apps/fedora-logo-icon.png ] ; then
    cp /usr/share/icons/hicolor/96x96/apps/fedora-logo-icon.png /home/openssuser/.face
    chown openssuser:openssuser /home/fedora/.face
    # TODO: would be nice to get e-d-s to pick this one up too... but how?
fi

# add some OpenSpeedShop bashrc goodies...
echo "cat << EOF" > /root/.bashrc.oss
echo "------------------------------"  >> /root/.bashrc.oss
echo "Welcome to Open|SpeedShop Live"  >> /root/.bashrc.oss
echo "------------------------------"  >> /root/.bashrc.oss
echo " "  >> /root/.bashrc.oss
echo "To run a sequential experiment:"  >> /root/.bashrc.oss
echo "    >> cd sequential/smg2000/test"  >> /root/.bashrc.oss
echo "    >> osspcsamp \"smg2000 -n 50 50 50\" "  >> /root/.bashrc.oss
echo "    >> openss -f smg2000-pcsamp.openss &"  >> /root/.bashrc.oss
echo " "  >> /root/.bashrc.oss
echo "To run an MPI experiment:"  >> /root/.bashrc.oss
echo "    >> cd mpi/smg2000/test"  >> /root/.bashrc.oss
echo "    >> osspcsamp \"mpirun -np 2 smg2000 -n 50 50 50\" "  >> /root/.bashrc.oss
echo "    >> openss -f smg2000-pcsamp.openss &"  >> /root/.bashrc.oss
echo " "  >> /root/.bashrc.oss
echo "For more information, please see the included HTML documentation."  >> /root/.bashrc.oss
echo "EOF"  >> /root/.bashrc.oss

echo "if \[ -f /etc/bashrc \]; then" >> /root/.bashrc.oss
echo "        . /etc/bashrc" >> /root/.bashrc.oss
echo "fi" >> /root/.bashrc.oss
echo "export QTDIR=/usr/lib/qt-3.3" >> /root/.bashrc.oss
echo "export OPENSS_PREFIX=/opt/OSS" >> /root/.bashrc.oss
echo "export OPENSS_PLUGIN_PATH=\\\$OPENSS_PREFIX/lib/openspeedshop" >> /root/.bashrc.oss
echo "export OPENSS_DOC_DIR=\\\$OPENSS_PREFIX/share/doc/packages/OpenSpeedShop" >> /root/.bashrc.oss
echo "export OPENSS_INSTRUMENTOR=mrnet" >> /root/.bashrc.oss
echo "export OPENSS_RAWDATA_DIR=/tmp" >> /root/.bashrc.oss
echo "export OPENSS_MPI_IMPLEMENTATION=openmpi" >> /root/.bashrc.oss
echo "export DYNINSTAPI_RT_LIB=\\\$OPENSS_PREFIX/lib/libdyninstAPI_RT.so" >> /root/.bashrc.oss
echo "export PATH=\\\$OPENSS_PREFIX/bin:/usr/lib/openmpi/bin:\\\$PATH" >> /root/.bashrc.oss
echo "export LD_LIBRARY_PATH=\\\$OPENSS_PREFIX/lib:/usr/lib/openmpi/lib:\\\$LD_LIBRARY_PATH" >> /root/.bashrc.oss
echo "export XPLAT_RSHCOMMAND=ssh" >> /root/.bashrc.oss
echo "export XPLAT_RSH=ssh" >> /root/.bashrc.oss
echo "export MRNET_RSH=ssh" >> /root/.bashrc.oss
echo "alias lsr=\"ls -lastr\"" >> /root/.bashrc.oss
chmod 644 /root/.bashrc.oss
cp /root/.bashrc.oss /home/openssuser/.bashrc
chown openssuser:openssuser /home/openssuser/.bashrc
rm -f /root/.bashrc.oss

# start some apps...

mkdir -p /home/openssuser/.config/autostart
chmod 775 /home/openssuser/.config/autostart

echo "[Desktop Entry]" > /root/.firefox.desktop
echo "Type=Application" >> /root/.firefox.desktop
echo "Exec=/usr/bin/firefox /opt/doc/index.html &" >> /root/.firefox.desktop
echo "Hidden=false" >> /root/.firefox.desktop
echo "X-GNOME-Autostart-enabled=true" >> /root/.firefox.desktop
echo "Name[en_US]=/usr/bin/firefox" >> /root/.firefox.desktop
echo "Name=/usr/bin/firefox" >> /root/.firefox.desktop
echo "Comment[en_US]=" >> /root/.firefox.desktop
echo "Comment=" >> /root/.firefox.desktop
chmod 664 /root/.firefox.desktop

echo "[Desktop Entry]" > /root/.gnome-terminal.desktop
echo "Type=Application" >> /root/.gnome-terminal.desktop
echo "Exec=gnome-terminal &" >> /root/.gnome-terminal.desktop
echo "Hidden=false" >> /root/.gnome-terminal.desktop
echo "X-GNOME-Autostart-enabled=true" >> /root/.gnome-terminal.desktop
echo "Name[en_US]=terminal window for demo" >> /root/.gnome-terminal.desktop
echo "Name=terminal window for demo" >> /root/.gnome-terminal.desktop
echo "Comment[en_US]=" >> /root/.gnome-terminal.desktop
echo "Comment=" >> /root/.gnome-terminal.desktop
chmod 664 /root/.gnome-terminal.desktop

# Start up webbrowser and terminal window

 
cp /root/.firefox.desktop /home/openssuser/.config/autostart/firefox.desktop
cp /root/.gnome-terminal.desktop /home/openssuser/.config/autostart/gnome-terminal.desktop
chown openssuser:openssuser -R /home/openssuser/.config
#chown openssuser:openssuser -R /home/openssuser/.config/autostart
chmod 777 /home/openssuser/.config/autostart
chmod 777 /home/openssuser/.config
chmod 777 /home/openssuser/.config/autostart/firefox.desktop
chmod 777 /home/openssuser/.config/autostart/gnome-terminal.desktop
rm -f /root/.firefox.desktop
rm -f /root/.gnome-terminal.desktop


# setup preferences file for new user

mkdir /home/openssuser/.qt
echo "[ManageProcessesPanel]" > /root/.openspeedshoprc
echo "updateDisplayLineEdit=15" >> /root/.openspeedshoprc
echo "updateOnCheckBox=true" >> /root/.openspeedshoprc
echo "" >> /root/.openspeedshoprc
echo "[Source Panel]" >> /root/.openspeedshoprc
echo "leftSideLineEdit0=/home/jeg/OpenSpeedShop/liveOSS/toyprograms-0.0.1" >> /root/.openspeedshoprc
echo "leftSideLineEdit1=/usr/global/tools/openspeedshop/dev/NPB3.3.1/NPB3.3-MPI/" >> /root/.openspeedshoprc
echo "leftSideLineEdit2=/home/jeg/Download/NPB3.2-MZ/NPB3.2-MZ-MPI/BT-MZ" >> /root/.openspeedshoprc
echo "leftSideLineEdit3=" >> /root/.openspeedshoprc
echo "leftSideLineEdit4=" >> /root/.openspeedshoprc
echo "leftSideLineEdit5=" >> /root/.openspeedshoprc
echo "leftSideLineEdit6=" >> /root/.openspeedshoprc
echo "leftSideLineEdit7=" >> /root/.openspeedshoprc
echo "leftSideLineEdit8=" >> /root/.openspeedshoprc
echo "leftSideLineEdit9=" >> /root/.openspeedshoprc
echo "rightSideLineEdit0=/home/openssuser" >> /root/.openspeedshoprc
echo "rightSideLineEdit1=/home/openssuser/mpi/" >> /root/.openspeedshoprc
echo "rightSideLineEdit2=/home/openssuser/hybrid" >> /root/.openspeedshoprc
echo "rightSideLineEdit3=" >> /root/.openspeedshoprc
echo "rightSideLineEdit4=" >> /root/.openspeedshoprc
echo "rightSideLineEdit5=" >> /root/.openspeedshoprc
echo "rightSideLineEdit6=" >> /root/.openspeedshoprc
echo "rightSideLineEdit7=" >> /root/.openspeedshoprc
echo "rightSideLineEdit8=" >> /root/.openspeedshoprc
echo "rightSideLineEdit9=" >> /root/.openspeedshoprc
echo "showLineNumbersCheckBox=true" >> /root/.openspeedshoprc
echo "showStatisticsCheckBox=false" >> /root/.openspeedshoprc
echo "" >> /root/.openspeedshoprc
echo "[Stats Panel]" >> /root/.openspeedshoprc
echo "advancedToolbarCheckBox=false" >> /root/.openspeedshoprc
echo "chartTypeComboBox=2" >> /root/.openspeedshoprc
echo "focusSourcePanelCheckBox=false" >> /root/.openspeedshoprc
echo "levelsToOpenLineEdit=-1" >> /root/.openspeedshoprc
echo "showMetadataCheckBox=true" >> /root/.openspeedshoprc
echo "showSkylineCheckBox=false" >> /root/.openspeedshoprc
echo "showSkylineLineEdit=25" >> /root/.openspeedshoprc
echo "showTextByLocationCheckBox=false" >> /root/.openspeedshoprc
echo "showTextByPercentCheckBox=true" >> /root/.openspeedshoprc
echo "showTextByValueCheckBox=false" >> /root/.openspeedshoprc
echo "showTextInChartCheckBox=true" >> /root/.openspeedshoprc
echo "showToolbarCheckBox=true" >> /root/.openspeedshoprc
echo "showTopNChartLineEdit=5" >> /root/.openspeedshoprc
echo "showTopNLineEdit=100" >> /root/.openspeedshoprc
echo "showTopNTraceLineEdit=1000000" >> /root/.openspeedshoprc
echo "sortDecendingCheckBox=true" >> /root/.openspeedshoprc
echo "" >> /root/.openspeedshoprc
echo "[general]" >> /root/.openspeedshoprc
echo "allowPythonCommands=true" >> /root/.openspeedshoprc
echo "askAboutChangingArgs=true" >> /root/.openspeedshoprc
echo "askAboutSavingTheDatabase=true" >> /root/.openspeedshoprc
echo "globalFontFamily=Bitstream Charter" >> /root/.openspeedshoprc
echo "globalFontItalic=false" >> /root/.openspeedshoprc
echo "globalFontPointSize=10" >> /root/.openspeedshoprc
echo "globalFontWeight=75" >> /root/.openspeedshoprc
echo "globalRemoteShell=/usr/bin/rsh" >> /root/.openspeedshoprc
echo "helpLevelDefault=2" >> /root/.openspeedshoprc
echo "historyDefault=24" >> /root/.openspeedshoprc
echo "historyLimit=100" >> /root/.openspeedshoprc
echo "instrumentorIsOffline=true" >> /root/.openspeedshoprc
echo "lessRestrictiveComparisons=true" >> /root/.openspeedshoprc
echo "maxAsyncCommands=20" >> /root/.openspeedshoprc
echo "onRerunSaveCopyOfExperimentDatabase=true" >> /root/.openspeedshoprc
echo "saveExperimentDatabase=true" >> /root/.openspeedshoprc
echo "showGraphics=false" >> /root/.openspeedshoprc
echo "showSplashScreen=true" >> /root/.openspeedshoprc
echo "viewBlankInPlaceOfZero=false"  >> /root/.openspeedshoprc
echo "viewDateTimePrecision=3" >> /root/.openspeedshoprc
echo "viewFieldSize=16" >> /root/.openspeedshoprc
echo "viewFieldSizeIsDynamic=true" >> /root/.openspeedshoprc
echo "viewFullPath=false" >> /root/.openspeedshoprc
echo "viewMangledName=false" >> /root/.openspeedshoprc
echo "viewPrecision=4" >> /root/.openspeedshoprc
echo "" >> /root/.openspeedshoprc
echo "[geometry]" >> /root/.openspeedshoprc
echo "height=667" >> /root/.openspeedshoprc
echo "width=1246" >> /root/.openspeedshoprc
echo "x=335" >> /root/.openspeedshoprc
echo "y=108" >> /root/.openspeedshoprc
echo "" >> /root/.openspeedshoprc
echo "[managecollectors]" >> /root/.openspeedshoprc
echo "splitter/size_0=602" >> /root/.openspeedshoprc
echo "splitter/size_1=602" >> /root/.openspeedshoprc
mkdir /root/.qt
chmod 644 /root/.openspeedshoprc
cp /root/.openspeedshoprc /root/.qt/openspeedshoprc
cp -r /root/.qt /home/openssuser/.
chown openssuser:openssuser -R /home/openssuser/.qt
rm /root/.openspeedshoprc


echo "localhost.localdomain ssh-rsa AAAAB3NzaC1yc2EAAAABIwAAAQEApWX5IL5f9c0tmBbmuj3/+Wx/2s8Nv0eo04gom+46DWNL9OQqiLtzHbqzn6M9WA0500kUsS5ZYxPXAcAo6LtWBpJ8eqVIbgYbwVRf5K2Iprm1irK6gYzBh8+iTPdqyLj4bjVzecxuNDNyD0YJTwB+q84J/linTHKmqdPC4Pzl+bTa+Je9oan9uORe1k5BKKkm8sem6/V39EwlRePiJMuVtZPCwBtLspMgCA+9R2KuvyisnS5vDjwnwcZQaHzCTsi57isk/P//4x1RWX+6KEngjjpp5igjnN1DBaabfIhLTc09z9tDVPcBir7mg0fJ09kckA4BExip1fzVLqmM1NoPgw==" > /root/.known_hosts
mkdir /root/.ssh
chmod 644 /root/.known_hosts
cp /root/.known_hosts /root/.ssh/known_hosts
cp -r /root/.ssh /home/openssuser/.
chown openssuser:openssuser -R /home/openssuser/.ssh
rm /root/.known_hosts


#move apps to /home/openssuser
#mkdir -p /home/openssuser/{mpi,openmp,sequential}
mkdir -p /home/openssuser/{mpi,multi,sequential,hybrid}
mv /opt/tempapps/mpi/smg2000 /home/openssuser/mpi/.
mv /opt/tempapps/mpi/nbody /home/openssuser/mpi/.
mv /opt/tempapps/mpi/LU /home/openssuser/mpi/.
mv /opt/tempapps/sequential/smg2000 /home/openssuser/sequential/.
#mv /opt/tempapps/openmp/smg2000 /home/openssuser/openmp/

mv /opt/tempapps/sequential/forever /home/openssuser/sequential/.
mv /opt/tempapps/sequential/threads /home/openssuser/sequential/.
mv /opt/tempapps/sequential/mutatee /home/openssuser/sequential/.
mv /opt/tempapps/multi/openmp_stress /home/openssuser/multi/.
mv /opt/tempapps/sequential/matmul /home/openssuser/sequential/.
mv /opt/tempapps/hybrid /home/openssuser/.

chown openssuser:openssuser -R /home/openssuser/mpi
chown openssuser:openssuser -R /home/openssuser/sequential
chown openssuser:openssuser -R /home/openssuser/multi
chown openssuser:openssuser -R /home/openssuser/hybrid
rm -rf /opt/tempapps

rm -rf /tmp/*

EOF

%end

