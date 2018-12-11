groupadd -r quanser
groupmems -g quanser -a msvas
cp ~/init.sh /etc/init.d
chmod +x /etc/init.d/init.sh
update-rc.d init.sh defaults
reboot
