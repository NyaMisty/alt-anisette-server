FROM nyamisty/docker-wine-dotnet:win32

RUN mkdir -p /tmp/iCloud \
  && wget -O /tmp/iCloud/iCloudSetup.exe http://updates-http.cdn-apple.com/2020/windows/001-39935-20200911-1A70AA56-F448-11EA-8CC0-99D41950005E/iCloudSetup.exe \
  && true

RUN apt-get update && apt-get install p7zip-full

RUN mkdir -p /tmp/ahk && cd /tmp/ahk \
  && wget -O /tmp/ahk/ahk.exe https://github.com/AutoHotkey/AutoHotkey/releases/download/v1.0.48.05/AutoHotkey104805_Install.exe \
  && (mkdir /ahk && cd /ahk && 7z x /tmp/ahk/ahk.exe AutoHotkey.exe AU3_Spy.exe) \
  && chmod +x /ahk/AutoHotkey.exe

ADD scripts/install_icloud.sh /app/
RUN entrypoint true \
  && cd /tmp/iCloud \
  && bash /app/install_icloud.sh \
  && rm -rf /tmp/iCloud

ADD http_wrap/server.exe /app/
ADD anisette_extract/Release/AltWindowsAnisette.exe /app/
ADD scripts/* /app/

EXPOSE 6969

CMD ["xvfb-run", "/app/run_icloud.sh"]
