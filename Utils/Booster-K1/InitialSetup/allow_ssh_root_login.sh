#!/bin/bash
echo "PermitRootLogin yes" >>/etc/ssh/sshd_config
echo "enforcing = 0" >>/etc/security/pwquality.conf
echo 123123 >>tmp
echo 123123 >>tmp
cat tmp | passwd root
systemctl restart ssh
