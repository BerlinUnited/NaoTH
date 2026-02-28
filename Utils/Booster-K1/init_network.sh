# Booster K1
# Configure the network for.

SSID="NAONET"
PWD="a1b0a1b0a1"
NR=41

# wifi
nmcli connection add \
  type wifi \
  ifname wlan0 \
  con-name naonet \
  ssid $SSID \
  wifi-sec.key-mgmt wpa-psk \
  wifi-sec.psk $PWD \
  ipv4.method manual \
  ipv4.addresses 10.0.4.$NR/16 \
  ipv4.gateway 10.0.4.1 \
  ipv4.dns "10.0.4.1 8.8.8.8" \
  ipv6.method disabled
  
  
nmcli connection modify naonet wifi.cloned-mac-address permanent
  
sudo nmcli connection reload

nmcli con down naonet
nmcli con up naonet