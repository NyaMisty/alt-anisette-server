set -x -e
echo "Extracting iCloud..." && wine iCloudSetup.exe /extract
echo "Installing Application Support..." && wine MsiExec.exe /i AppleApplicationSupport.msi /qn

bash -c "while true; do wine taskkill /f /im iCloudServices.exe; sleep 5; done" &

echo "Installing iCloud..." && wine MsiExec.exe /i iCloud.msi REBOOT=ReallySuppress /qn || echo "msiexec return: $?"

echo "Cleanup..." && kill %1
