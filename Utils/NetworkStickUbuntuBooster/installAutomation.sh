# turn off auto-mounting of usb sticks
# gsettings set org.gnome.desktop.media-handling automount false gsettings set org.gnome.desktop.media-handling automount-open false

sudo mv 99-new-network-berlin-united.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules

sudo mv getNewNetworkOfUSBStick@.service /etc/systemd/system/
sudo systemctl daemon-reload

sudo mv getNewNetworkOfUSBStick.sh /usr/local/bin/