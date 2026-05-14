#!/bin/bash
pkill qemu 2>/dev/null
pkill websockify 2>/dev/null
sleep 1
qemu-system-x86_64 -cdrom crunchos.iso -boot d -m 512 -vga std -vnc :0,password=off &
sleep 2
websockify --web=/usr/share/novnc/ 6080 localhost:5900 &
echo "CrunchOS started! Open port 6080 in Ports tab then go to vnc_lite.html"
