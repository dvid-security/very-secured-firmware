Téléchargez les firmwares du training :

- [legit.bin](/files/e77234ee-ae1e-485f-9910-ef0ea9b2e196/03f49d1f-7608-4c94-98f8-c6634c215ad6/bd65c256-0f38-42ec-9299-705bdb186d6e.bin) : le firmware légitime, à flasher sur votre carte.
- [evil.bin](/files/e77234ee-ae1e-485f-9910-ef0ea9b2e196/03f49d1f-7608-4c94-98f8-c6634c215ad6/0ca23653-904a-4885-bd7e-643f071fa069.bin) : le firmware malveillant, que vous pousserez via l'OTA.

Flashez le firmware `legit.bin` sur votre carte DVID (ESP32C6).

Une fois flashée, la carte démarre et crée un réseau WiFi ouvert nommé
`DVID-OTA`. Elle expose une page d'accueil à l'adresse
`http://192.168.4.1`.

Dans ce training, l'appareil expose une mise à jour OTA (Over The Air) qui
**vérifie la signature** du firmware : seule une image signée par le fabricant
est acceptée avant d'être installée. Les sources du firmware sont publiées sur
le dépôt du fabricant :
[github.com/dvid-security/very-secured-firmware](https://github.com/dvid-security/very-secured-firmware).

Votre objectif : pousser malgré tout votre propre firmware (`evil.bin`) et
récupérer un secret stocké dans l'appareil.

Vous êtes prêt à commencer la formation ?