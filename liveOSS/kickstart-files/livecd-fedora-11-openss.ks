%include livecd-fedora-11-base-openss.ks

%packages
@graphical-internet
@gnome-desktop
NetworkManager-vpnc
NetworkManager-openvpn

# OpenSpeedShop-specific
openmpi
libdwarf
libunwind
sqlite
papi
libmonitor
dyninst
mrnet
openspeedshop
toyprograms
webtutorial

# dictionaries are big
-aspell-*
-man-pages-*
-scim-tables-*
-wqy-bitmap-fonts
-dejavu-fonts-experimental
-dejavu-fonts

# more fun with space saving 
-scim-lang-chinese
scim-chewing
scim-pinyin

# save some space
-gnome-user-docs
-gimp-help
-evolution-help
-autofs
-nss_db
-vino
#-evolution TODO: need to mod the gnome panel...
-ekiga
-pidgin
-transmission
-tomboy

%end

%post
cat >> /etc/rc.d/init.d/fedora-live << EOF
# disable screensaver locking
gconftool-2 --direct --config-source=xml:readwrite:/etc/gconf/gconf.xml.defaults -s -t bool /apps/gnome-screensaver/lock_enabled false >/dev/null
# set up timed auto-login for after 1 second
sed -i -e 's/\[daemon\]/[daemon]\nTimedLoginEnable=true\nTimedLogin=openssuser\nTimedLoginDelay=1/' /etc/gdm/custom.conf
if [ -e /usr/share/icons/hicolor/96x96/apps/fedora-logo-icon.png ] ; then
    cp /usr/share/icons/hicolor/96x96/apps/fedora-logo-icon.png /home/openssuser/.face
    chown openssuser:openssuser /home/fedora/.face
    # TODO: would be nice to get e-d-s to pick this one up too... but how?
fi

# add some OpenSpeedShop bashrc settings...
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
echo "export QTDIR=/usr/lib64/qt-3.3" >> /root/.bashrc.oss
echo "export OPENSS_PREFIX=/opt/OSS" >> /root/.bashrc.oss
echo "export OPENSS_PLUGIN_PATH=\\\$OPENSS_PREFIX/lib64/openspeedshop" >> /root/.bashrc.oss
echo "export OPENSS_DOC_DIR=\\\$OPENSS_PREFIX/share/doc/packages/OpenSpeedShop" >> /root/.bashrc.oss
echo "export OPENSS_INSTRUMENTOR=mrnet" >> /root/.bashrc.oss
echo "export OPENSS_RAWDATA_DIR=/tmp" >> /root/.bashrc.oss
echo "export OPENSS_MPI_OPENMPI=/opt/openmpi" >> /root/.bashrc.oss
echo "export DYNINSTAPI_RT_LIB=\\\$OPENSS_PREFIX/lib64/libdyninstAPI_RT.so.1" >> /root/.bashrc.oss
echo "export PATH=\\\$OPENSS_PREFIX/bin:/opt/openmpi/bin:\\\$PATH" >> /root/.bashrc.oss
echo "export LD_LIBRARY_PATH=\\\$OPENSS_PREFIX/lib64:/opt/openmpi/lib:\\\$LD_LIBRARY_PATH" >> /root/.bashrc.oss
echo "export XPLAT_RSHCOMMAND=ssh" >> /root/.bashrc.oss
echo "export XPLAT_RSH=ssh" >> /root/.bashrc.oss
echo "export MRNET_RSH=ssh" >> /root/.bashrc.oss
echo "alias lsr=\"ls -lastr\"" >> /root/.bashrc.oss
chmod 644 /root/.bashrc.oss
cp /root/.bashrc.oss /home/openssuser/.bashrc
chown openssuser:openssuser /home/openssuser/.bashrc
rm -f /root/.bashrc.oss

# start some apps...
echo "[Default]" > /root/.session-manual
echo "num_clients=2" >> /root/.session-manual
echo "0,RestartClientHint=3" >> /root/.session-manual
echo "0,Priority=50" >> /root/.session-manual
echo "0,RestartCommand=firefox /opt/doc/index.html" >> /root/.session-manual
echo "1,Program=firefox /opt/doc/index.html" >> /root/.session-manual
echo "1,RestartClientHint=3" >> /root/.session-manual
echo "1,Priority=50" >> /root/.session-manual
echo "1,RestartCommand=gnome-terminal" >> /root/.session-manual
echo "1,Program=gnome-terminal" >> /root/.session-manual
cp /root/.session-manual /home/openssuser/.gnome2/session-manual
chown openssuser:openssuser /home/openssuser/.gnome2/session-manual
rm -f /root/.session-manual

# setup preferences file for new user

mkdir /home/openssuser/.qt
echo "[ManageProcessesPanel]" > /root/.openspeedshoprc
echo "updateDisplayLineEdit=15" >> /root/.openspeedshoprc
echo "updateOnCheckBox=true" >> /root/.openspeedshoprc
echo "" >> /root/.openspeedshoprc
echo "[Source Panel]" >> /root/.openspeedshoprc
echo "leftSideLineEdit0=/home/jeg/OpenSpeedShop/liveOSS/toyprograms-0.0.1" >> /root/.openspeedshoprc
echo "leftSideLineEdit1=/usr/global/tools/openspeedshop/dev/NPB3.3.1/NPB3.3-MPI/" >> /root/.openspeedshoprc
echo "leftSideLineEdit2=" >> /root/.openspeedshoprc
echo "leftSideLineEdit3=" >> /root/.openspeedshoprc
echo "leftSideLineEdit4=" >> /root/.openspeedshoprc
echo "leftSideLineEdit5=" >> /root/.openspeedshoprc
echo "leftSideLineEdit6=" >> /root/.openspeedshoprc
echo "leftSideLineEdit7=" >> /root/.openspeedshoprc
echo "leftSideLineEdit8=" >> /root/.openspeedshoprc
echo "leftSideLineEdit9=" >> /root/.openspeedshoprc
echo "rightSideLineEdit0=/home/openssuser" >> /root/.openspeedshoprc
echo "rightSideLineEdit1=/home/openssuser/mpi/" >> /root/.openspeedshoprc
echo "rightSideLineEdit2=" >> /root/.openspeedshoprc
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
echo "viewFieldSize=16" >> /root/.openspeedshoprc
echo "viewFullPath=false" >> /root/.openspeedshoprc
echo "viewMangledName=false" >> /root/.openspeedshoprc
echo "viewPrecision=4" >> /root/.openspeedshoprc
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
mkdir -p /home/openssuser/{mpi,multi,sequential}
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

chown openssuser:openssuser -R /home/openssuser/mpi
chown openssuser:openssuser -R /home/openssuser/sequential
chown openssuser:openssuser -R /home/openssuser/multi
rm -rf /opt/tempapps

rm -rf /tmp/*

EOF

%end

