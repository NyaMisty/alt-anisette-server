#ErrorStdOut

;	Works regardless if debug argument is passed in.
debug(string) {
	string .= "`n" 
    FileAppend %string%,* ;* goes to stdout
}

debug("[i] Initializing")

SetWinDelay 1000
SetTitleMatchMode, 2


debug("[i] Finding iCloud Window")
WinWait, iCloud

debug("[i] Finding Account Input")
ControlClick, Edit1
sleep 1000
debug("[i] Sending Account Name")
ControlSetText, Edit1, aaa@icloud.com

debug("[i] Finding Password Input")
ControlClick, Edit2
sleep 1000
debug("[i] Sending Password")
ControlSetText, Edit2, Aa123456

While True
{
    WinWait, iCloud
    debug("[i] Clicking login")
    ControlClick, Button1
    debug("[i] Waiting for error dialog")
    WinWait Verification Failed
    ControlGetText text, Static2
    debug("[i] got msg: " . text)
    If InStr(text, "Visit iForgot to reset your account")
    {
        debug("[i] Got locked ret msg, good!")
        Break
    }
    If InStr(text, "There was an error connecting to")
    {
        debug("[i] Got first-time connection error, wait for locked msg!")
        WinClose
        sleep 2500
        Continue
    }
    else
    {
        debug("[e] Unknown message!")
        ExitApp 1
    }
}

debug("[i] iCloud Initialized successfully!")

