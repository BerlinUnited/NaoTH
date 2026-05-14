These are the (wired) connections that come preinstalled on the K1

/etc/NetworkManager/system-connections/USB\ Ethernet.nmconnection
```
[connection]
id=USB Ethernet
uuid=bca1e280-f425-40ea-965f-85d7cf478ad4
type=ethernet
interface-name=usb_eth0

[ethernet]

[ipv4]
address1=192.168.127.101/24
method=manual

[ipv6]
addr-gen-mode=stable-privacy
method=disabled

[proxy]
```

/etc/NetworkManager/system-connections/Wired\ connection\ 1.nmconnection
```
[connection]
id=Wired connection 1
uuid=c3e72119-d543-3631-ac00-41a50ac70138
type=ethernet
autoconnect-priority=-999
interface-name=enP8p1s0

[ethernet]

[ipv4]
address1=192.168.13.101/24
method=manual

[ipv6]
addr-gen-mode=stable-privacy
method=auto

[proxy]
```

/etc/NetworkManager/system-connections/Wired\ connection\ 2.nmconnection
```
[connection]
id=Wired connection 2
uuid=df2777b1-c583-3ac2-b955-b4ad0c3ea394
type=ethernet
autoconnect-priority=-999
interface-name=enP9p1s0
timestamp=1751286234

[ethernet]

[ipv4]
address1=192.168.10.102/24
method=manual

[ipv6]
addr-gen-mode=stable-privacy
method=auto

[proxy]
```

/etc/NetworkManager/system-connections/Wired\ connection\ 3.nmconnection
```
[connection]
id=Wired connection 3
uuid=8f59c559-4efa-3f38-9dc9-57c008cf7b94
type=ethernet
autoconnect-priority=-999
interface-name=eth0
timestamp=1751461652

[ethernet]

[ipv4]
address1=192.168.127.101/24
method=manual

[ipv6]
addr-gen-mode=stable-privacy
method=auto

[proxy]
```
