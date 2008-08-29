%include livecd-fedora-8-base-openss.ks

%packages
@graphical-internet
@gnome-desktop
NetworkManager-vpnc
NetworkManager-openvpn

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

EOF

%end
