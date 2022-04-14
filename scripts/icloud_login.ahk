SetWinDelay 1000
SetTitleMatchMode, 2

WinWait, iCloud

ControlClick, Edit1
sleep 1000
ControlSetText, Edit1, aaa@icloud.com

ControlClick, Edit2
sleep 1000
ControlSetText, Edit2, Aa123456

ControlClick, Button1

WinWait Verification Failed
