%include livecd-fedora-8-base-openss.ks

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

# add some OpenSpeedShop bashrc goodies...
echo "cat << EOF" > /root/.bashrc.oss
echo "------------------------------"  >> /root/.bashrc.oss
echo "Welcome to Open|SpeedShop Live"  >> /root/.bashrc.oss
echo "------------------------------"  >> /root/.bashrc.oss
echo " "  >> /root/.bashrc.oss
echo "To run a sequential experiment:"  >> /root/.bashrc.oss
echo "    >> cd sequential/smg2000/test"  >> /root/.bashrc.oss
echo "    >> openss -f smg2000 pcsamp"  >> /root/.bashrc.oss
echo "    >> openss smg2000.pcsamp.openss &"  >> /root/.bashrc.oss
echo " "  >> /root/.bashrc.oss
echo "To run an MPI experiment:"  >> /root/.bashrc.oss
echo "    >> cd mpi/smg2000/test"  >> /root/.bashrc.oss
echo "    >> openss -f \"mpirun -np 2 smg2000 -n 50 50 50\" pcsamp"  >> /root/.bashrc.oss
echo "    >> openss smg2000.pcsamp.openss &"  >> /root/.bashrc.oss
echo " "  >> /root/.bashrc.oss
echo "For more information, please see the included HTML documentation."  >> /root/.bashrc.oss
echo "EOF"  >> /root/.bashrc.oss

echo "if \[ -f /etc/bashrc \]; then" >> /root/.bashrc.oss
echo "        . /etc/bashrc" >> /root/.bashrc.oss
echo "fi" >> /root/.bashrc.oss
echo "export QTDIR=/usr/lib/qt-3.3" >> /root/.bashrc.oss
echo "export OPENSS_PREFIX=/opt/OSS" >> /root/.bashrc.oss
echo "export OPENSS_PLUGIN_PATH=\\\$OPENSS_PREFIX/lib/openspeedshop" \
>> /root/.bashrc.oss
echo "export OPENSS_INSTRUMENTOR=mrnet" >> /root/.bashrc.oss
echo "export OPENSS_MRNET_TOPOLOGY_FILE=/home/openssuser/.openss.top" \
>> /root/.bashrc.oss
echo "export OPENSS_RAWDATA_DIR=/tmp" >> /root/.bashrc.oss
echo "export OPENSS_MPI_OPENMPI=/opt/openmpi" >> /root/.bashrc.oss
echo "export DYNINSTAPI_RT_LIB=\\\$OPENSS_PREFIX/lib/libdyninstAPI_RT.so.1" \
>> /root/.bashrc.oss
echo "export PATH=\\\$OPENSS_PREFIX/bin:/opt/openmpi/bin:\\\$PATH" >> \
/root/.bashrc.oss
echo "export LD_LIBRARY_PATH=\\\$OPENSS_PREFIX/lib:/opt/openmpi/lib:\\\$LD_LIBRARY_PATH" >> /root/.bashrc.oss
echo "export XPLAT_RSHCOMMAND=ssh" >> /root/.bashrc.oss
echo "export XPLAT_RSH=ssh" >> /root/.bashrc.oss
echo "export MRNET_RSH=ssh" >> /root/.bashrc.oss
chmod 644 /root/.bashrc.oss
cp /root/.bashrc.oss /home/openssuser/.bashrc
chown openssuser:openssuser /home/openssuser/.bashrc
rm -f /root/.bashrc.oss

# add a MRNet topology file
echo "localhost.localdomain:0 => localhost.localdomain:1 ;" \
> /root/.openss.top
cp /root/.openss.top /home/openssuser/
chown openssuser:openssuser /home/openssuser/.openss.top
rm -f /root/.openss.top

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

echo "[ManageProcessesPanel]" > /home/openssuser/.qt/openspeedshoprc
echo "updateDisplayLineEdit=15" >> /home/openssuser/.qt/openspeedshoprc
echo "updateOnCheckBox=true" >> /home/openssuser/.qt/openspeedshoprc
echo "" >> /home/openssuser/.qt/openspeedshoprc
echo "[Source Panel]" >> /home/openssuser/.qt/openspeedshoprc
echo "leftSideLineEdit0=/home/openssuser/OpenSpeedShop/liveOSS/toyprograms" >> /home/jeg/.qt/openspeedshoprc
echo "leftSideLineEdit1=" >> /home/openssuser/.qt/openspeedshoprc
echo "leftSideLineEdit2=" >> /home/openssuser/.qt/openspeedshoprc
echo "leftSideLineEdit3=" >> /home/openssuser/.qt/openspeedshoprc
echo "leftSideLineEdit4=" >> /home/openssuser/.qt/openspeedshoprc
echo "leftSideLineEdit5=" >> /home/openssuser/.qt/openspeedshoprc
echo "leftSideLineEdit6=" >> /home/openssuser/.qt/openspeedshoprc
echo "leftSideLineEdit7=" >> /home/openssuser/.qt/openspeedshoprc
echo "leftSideLineEdit8=" >> /home/openssuser/.qt/openspeedshoprc
echo "leftSideLineEdit9=" >> /home/openssuser/.qt/openspeedshoprc
echo "rightSideLineEdit0=/home/openssuser" >> /home/openssuser/.qt/openspeedshoprc
echo "rightSideLineEdit1=" >> /home/openssuser/.qt/openspeedshoprc
echo "rightSideLineEdit2=" >> /home/openssuser/.qt/openspeedshoprc
echo "rightSideLineEdit3=" >> /home/openssuser/.qt/openspeedshoprc
echo "rightSideLineEdit4=" >> /home/openssuser/.qt/openspeedshoprc
echo "rightSideLineEdit5=" >> /home/openssuser/.qt/openspeedshoprc
echo "rightSideLineEdit6=" >> /home/openssuser/.qt/openspeedshoprc
echo "rightSideLineEdit7=" >> /home/openssuser/.qt/openspeedshoprc
echo "rightSideLineEdit8=" >> /home/openssuser/.qt/openspeedshoprc
echo "rightSideLineEdit9=" >> /home/openssuser/.qt/openspeedshoprc
echo "showLineNumbersCheckBox=true" >> /home/openssuser/.qt/openspeedshoprc
echo "showStatisticsCheckBox=false" >> /home/openssuser/.qt/openspeedshoprc
echo "" >> /home/openssuser/.qt/openspeedshoprc
echo "[Stats Panel]" >> /home/openssuser/.qt/openspeedshoprc
echo "advancedToolbarCheckBox=false" >> /home/openssuser/.qt/openspeedshoprc
echo "chartTypeComboBox=2" >> /home/openssuser/.qt/openspeedshoprc
echo "focusSourcePanelCheckBox=false" >> /home/openssuser/.qt/openspeedshoprc
echo "levelsToOpenLineEdit=-1" >> /home/openssuser/.qt/openspeedshoprc
echo "showMetadataCheckBox=true" >> /home/openssuser/.qt/openspeedshoprc
echo "showSkylineCheckBox=false" >> /home/openssuser/.qt/openspeedshoprc
echo "showSkylineLineEdit=25" >> /home/openssuser/.qt/openspeedshoprc
echo "showTextByLocationCheckBox=false" >> /home/openssuser/.qt/openspeedshoprc
echo "showTextByPercentCheckBox=true" >> /home/openssuser/.qt/openspeedshoprc
echo "showTextByValueCheckBox=false" >> /home/openssuser/.qt/openspeedshoprc
echo "showTextInChartCheckBox=true" >> /home/openssuser/.qt/openspeedshoprc
echo "showToolbarCheckBox=true" >> /home/openssuser/.qt/openspeedshoprc
echo "showTopNChartLineEdit=5" >> /home/openssuser/.qt/openspeedshoprc
echo "showTopNLineEdit=100" >> /home/openssuser/.qt/openspeedshoprc
echo "sortDecendingCheckBox=true" >> /home/openssuser/.qt/openspeedshoprc
echo "" >> /home/openssuser/.qt/openspeedshoprc
echo "[general]" >> /home/openssuser/.qt/openspeedshoprc
echo "allowPythonCommands=true" >> /home/openssuser/.qt/openspeedshoprc
echo "askAboutChangingArgs=true" >> /home/openssuser/.qt/openspeedshoprc
echo "askAboutSavingTheDatabase=true" >> /home/openssuser/.qt/openspeedshoprc
echo "globalFontFamily=Bitstream Charter" >> /home/openssuser/.qt/openspeedshoprc
echo "globalFontItalic=false" >> /home/openssuser/.qt/openspeedshoprc
echo "globalFontPointSize=10" >> /home/openssuser/.qt/openspeedshoprc
echo "globalFontWeight=75" >> /home/openssuser/.qt/openspeedshoprc
echo "globalRemoteShell=/usr/bin/rsh" >> /home/openssuser/.qt/openspeedshoprc
echo "helpLevelDefault=2" >> /home/openssuser/.qt/openspeedshoprc
echo "historyDefault=24" >> /home/openssuser/.qt/openspeedshoprc
echo "historyLimit=100" >> /home/openssuser/.qt/openspeedshoprc
echo "instrumentorIsOffline=true" >> /home/openssuser/.qt/openspeedshoprc
echo "lessRestrictiveComparisons=true" >> /home/openssuser/.qt/openspeedshoprc
echo "maxAsyncCommands=20" >> /home/openssuser/.qt/openspeedshoprc
echo "onRerunSaveCopyOfExperimentDatabase=true" >> /home/openssuser/.qt/openspeedshoprc
echo "saveExperimentDatabase=true" >> /home/openssuser/.qt/openspeedshoprc
echo "showGraphics=false" >> /home/openssuser/.qt/openspeedshoprc
echo "showSplashScreen=true" >> /home/openssuser/.qt/openspeedshoprc
echo "viewFieldSize=20" >> /home/openssuser/.qt/openspeedshoprc
echo "viewFullPath=false" >> /home/openssuser/.qt/openspeedshoprc
echo "viewMangledName=false" >> /home/openssuser/.qt/openspeedshoprc
echo "viewPrecision=6" >> /home/openssuser/.qt/openspeedshoprc


echo "localhost.localdomain ssh-rsa AAAAB3NzaC1yc2EAAAABIwAAAQEApWX5IL5f9c0tmBbmuj3/+Wx/2s8Nv0eo04gom+46DWNL9OQqiLtzHbqzn6M9WA0500kUsS5ZYxPXAcAo6LtWBpJ8eqVIbgYbwVRf5K2Iprm1irK6gYzBh8+iTPdqyLj4bjVzecxuNDNyD0YJTwB+q84J/linTHKmqdPC4Pzl+bTa+Je9oan9uORe1k5BKKkm8sem6/V39EwlRePiJMuVtZPCwBtLspMgCA+9R2KuvyisnS5vDjwnwcZQaHzCTsi57isk/P//4x1RWX+6KEngjjpp5igjnN1DBaabfIhLTc09z9tDVPcBir7mg0fJ09kckA4BExip1fzVLqmM1NoPgw==" > /home/openssuser/.ssh/known_hosts


#move apps to /home/openssuser
#mkdir -p /home/openssuser/{mpi,openmp,sequential}
mkdir -p /home/openssuser/{mpi,sequential}
mv /opt/tempapps/mpi/smg2000 /home/openssuser/mpi/
mv /opt/tempapps/mpi/nbody /home/openssuser/mpi/
mv /opt/tempapps/sequential/smg2000 /home/openssuser/sequential/
#mv /opt/tempapps/openmp/smg2000 /home/openssuser/openmp/

mv /opt/tempapps/sequential/forever* /home/openssuser/sequential/
mv /opt/tempapps/sequential/threads* /home/openssuser/sequential/
mv /opt/tempapps/sequential/mutatee* /home/openssuser/sequential/
mv /opt/tempapps/sequential/matmul* /home/openssuser/sequential/

chown openssuser:openssuser -R /home/openssuser/mpi
chown openssuser:openssuser -R /home/openssuser/sequential
rm -rf /opt/tempapps

rm -rf /tmp/*

EOF

%end

