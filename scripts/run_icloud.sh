#!/bin/bash

echo "-----------------------------------------------"
echo ""
echo 'If after 60 seconds you are still not seeing "Anisette Server running at http://0.0.0.0:6969/"'
echo 'Please re-run the docker container as the initialize process are stucked'
echo ""
echo "-----------------------------------------------"

export WINEDEBUG=-all

set -x
set -e

cd /app

wine 'C:\Program Files\Common Files\Apple\Internet Services\iCloud.exe' &

sleep 10

timeout 45s wine "Z:\\ahk\\AutoHotkeyU32.exe" "Z:\\app\\icloud_login.ahk"

sleep 1

export NODE_SKIP_PLATFORM_CHECK=1
#wine "Z:\\app\\server.exe"
(cd /app; node server_linux.js)