#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Update.h>
#include <Preferences.h>
#include <BLAKE2b.h>
#include <Ed25519.h>
#include <Flag.h>
// Point d'acces WiFi ouvert.
static const char* AP_SSID = "DVID-OTA";

// Signature Ed25519 (minisign) ajoutee a la fin de l'image OTA.
static const size_t SIG_LEN = 64;

// Cle publique minisign du fabricant. Une image n'est acceptee que si sa
// signature a ete produite avec la cle privee correspondante.
static const uint8_t PUBKEY[32] = {
    0x6f, 0xdc, 0xb3, 0x1c, 0xda, 0x2c, 0x9b, 0x96,
    0x71, 0x0f, 0x18, 0x66, 0xa8, 0x3a, 0x67, 0xda,
    0xfb, 0xa6, 0x48, 0xc3, 0x33, 0x34, 0x98, 0x81,
    0x59, 0xcf, 0xd1, 0x59, 0x1a, 0xf5, 0xb8, 0x1f,
};

WebServer server(80);
Preferences prefs;

// Flag obfusque, depose en NVS au demarrage.
const uint8_t idx[] = {1, 6, 8, 17, 26, 33, 36, 42, 44, 51, 52, 60, 66, 70, 74, 82};
String flag = extractFlag(F("aDhhexVvIcsnbacghDtargwuwr{hosizaLwn3iegykackdlltiz3drdmcrju_sgwcbkyjc3dmiyulfllgv}wvuc"), idx, sizeof(idx));

// Etat de la reception OTA.
static BLAKE2b hash;             // empreinte du corps (= tout sauf les 64 derniers octets)
static uint8_t tail[SIG_LEN];    // buffer glissant : les 64 derniers octets = la signature
static size_t  tailLen = 0;
static bool    otaFail = false;

// Ecrit des octets de corps : dans le hash et dans la partition OTA.
static void pushBody(const uint8_t* p, size_t n) {
    hash.update(p, n);
    if (Update.write((uint8_t*)p, n) != n) otaFail = true;
}

// Consomme un morceau recu en gardant toujours les 64 derniers octets en tampon.
static void feed(const uint8_t* data, size_t len) {
    if (otaFail) return;
    size_t total = tailLen + len;
    if (total <= SIG_LEN) {                 // pas encore assez pour ecrire du corps
        memcpy(tail + tailLen, data, len);
        tailLen = total;
        return;
    }
    size_t toFlush  = total - SIG_LEN;      // octets surs d'etre du corps
    size_t fromTail = min(toFlush, tailLen);
    if (fromTail) pushBody(tail, fromTail);
    size_t fromData = toFlush - fromTail;
    if (fromData) pushBody(data, fromData);

    // Reconstruit le tampon = 64 derniers octets de (tampon || data).
    uint8_t keep[SIG_LEN];
    size_t remTail = tailLen - fromTail;
    if (remTail) memcpy(keep, tail + fromTail, remTail);
    memcpy(keep + remTail, data + fromData, len - fromData);
    memcpy(tail, keep, SIG_LEN);
    tailLen = SIG_LEN;
}

// Verifie la signature minisign : Ed25519( blake2b-512(corps) ).
static bool verify() {
    if (otaFail || tailLen != SIG_LEN) return false;   // image trop courte
    uint8_t digest[64];
    hash.finalize(digest, sizeof(digest));
    return Ed25519::verify(tail, PUBKEY, digest, sizeof(digest));
}

// Reception du fichier firmware (POST multipart).
static void onUpload() {
    HTTPUpload& up = server.upload();
    if (up.status == UPLOAD_FILE_START) {
        tailLen = 0;
        otaFail = false;
        hash.reset(64);
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) otaFail = true;
    } else if (up.status == UPLOAD_FILE_WRITE) {
        feed(up.buf, up.currentSize);
    } else if (up.status == UPLOAD_FILE_ABORTED) {
        Update.abort();
    }
}

static const char UPDATE_PAGE[] =
    "<h1>DVID - OTA Update</h1>"
    "<form method='POST' action='/update' enctype='multipart/form-data'>"
    "<input type='file' name='firmware'>"
    "<input type='submit' value='Update'>"
    "</form>";

void setup() {
    Serial.begin(115200);

    // Depose le flag en NVS (lu ensuite par le firmware installe).
    prefs.begin("dvid", false);
    prefs.putString("flag", flag);
    prefs.end();

    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID);
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());

    server.on("/", []() {
        server.send(200, "text/html",
            "<h1>DVID - OTA Update</h1>"
            "<p>Firmware update: <a href=\"/update\">/update</a></p>");
    });
    server.on("/update", HTTP_GET, []() {
        server.send(200, "text/html", UPDATE_PAGE);
    });
    server.on("/update", HTTP_POST, []() {
        if (verify()) {
            Update.end(true);
            server.send(200, "text/html",
                "<h1>Update OK</h1><p>Signature valide, redemarrage...</p>");
            delay(500);
            ESP.restart();
        } else {
            Update.abort();
            server.send(200, "text/html",
                "<h1>Update refuse</h1><p>Signature invalide ou absente.</p>");
        }
    }, onUpload);
    server.begin();
}

void loop() {
    server.handleClient();
}
