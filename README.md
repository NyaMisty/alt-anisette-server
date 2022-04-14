# alt-anisette-server

Anisette Data as Service, like Sideloadly's, based on AltServer-Windows!

## What's Anisette Data

An unknown data generated by Apple softwares, currently mainly used by AltServer to login Apple Developer

## How to use

To start it:

```
docker pull nyamisty/alt_anisette_server
docker run -d --rm -p 6969:6969 -it nyamisty/alt_anisette_server
```

After seeing "Server running on ..." message, you can do `curl 127.0.0.1:6969` to see the Anisette data returned

## Credits

- Sideloadly: for idea of AnisetteData-as-Service

- AltServer-Windows: for code to extract Anisette Data from Windows iTunes
