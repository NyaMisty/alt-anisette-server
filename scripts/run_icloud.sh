#!/bin/bash

set -x
set -e

cd /app

wine 'C:\Program Files\Common Files\Apple\Internet Services\iCloud.exe' &

sleep 10

wine "Z:\\ahk\\AutoHotkey.exe" "Z:\\app\\icloud_login.ahk"

sleep 1

export NODE_SKIP_PLATFORM_CHECK=1
wine "Z:\\app\\server.exe"