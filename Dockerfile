FROM nyamisty/docker-wine-dotnet:win32

RUN mkdir -p /tmp/iCloud \
  && wget -O /tmp/iCloud/iCloudSetup.exe http://updates-http.cdn-apple.com/2020/windows/001-39935-20200911-1A70AA56-F448-11EA-8CC0-99D41950005E/iCloudSetup.exe \
  && true

ADD scripts/install_icloud.sh /app/
RUN entrypoint true \
  && cd /tmp/iCloud \
  && bash /app/install_icloud.sh \
  && rm -rf /tmp/iCloud

RUN apt-get update && apt-get install -y p7zip-full vim nodejs

RUN mkdir -p /tmp/ahk && cd /tmp/ahk \
  && wget -O /tmp/ahk/ahk.exe https://github.com/Lexikos/AutoHotkey_L/releases/download/v1.1.33.10/AutoHotkey_1.1.33.10_setup.exe \
  && (mkdir /ahk && cd /ahk && 7z x /tmp/ahk/ahk.exe AutoHotkeyA32.exe AutoHotkeyU32.exe AutoHotkeyU64.exe WindowSpy.ahk) \
  && chmod +x /ahk/AutoHotkey*.exe

ADD http_wrap/server* /app/
ADD anisette_extract/Release/AltWindowsAnisette.exe /app/
ADD scripts/* /app/

EXPOSE 6969

CMD ["/app/run_icloud.sh"]
