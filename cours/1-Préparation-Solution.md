Ce training utilise l'ESP32-C6 de la carte DVID (Module wifi).

Pour le flasher, vous devez connecter votre dongle UART comme ceci :
![Branchement](/files/bb78da22-b97d-4714-93ed-43c99202bf66/afd419e8-f22f-42c6-b0d6-27fa3aa06f11/ac8456be-b186-4547-84cc-4931e14df46c.png)

Installez `esptool` :

```bash
pip install esptool
```

Branchez la carte en USB puis flashez :

Pour **Linux** :
```bash
esptool --chip esp32c6 --port /dev/ttyUSB0 --baud 921600 write-flash 0x10000 legit.bin
```

Pour **Windows** :
```powershell
esptool --chip esp32c6 --port COM3 --baud 921600 write-flash 0x10000 .\legit.bin
```

Vous devriez obtenir ceci :
```powershell
esptool v5.4.0
Connected to ESP32-C6 on COM3:
Chip type:          ESP32-C6 (QFN40) (revision v0.2)
Features:           Wi-Fi 6, BT 5 (LE), IEEE802.15.4, Single Core + LP Core, 160MHz, Unknown Embedded Flash
Crystal frequency:  40MHz
MAC:                98:a3:16:ff:fe:a8:ed:bc
BASE MAC:           98:a3:16:a8:ed:bc
MAC_EXT:            ff:fe

Stub flasher running.
Changing baud rate to 921600...
Changed.

Configuring flash size...
Flash will be erased from 0x00010000 to 0x0011dfff...
Wrote 1105920 bytes (674129 compressed) at 0x00010000 in 11.5 seconds (766.5 kbit/s).
Hash of data verified.

Hard resetting via RTS pin...
```

Une fois le flash terminé, réinitialisez la carte. Le nom du réseau WiFi `DVID-OTA` et l'adresse `http://192.168.4.1/`.