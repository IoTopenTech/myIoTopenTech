#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <EEPROM.h>
/*
   This example serves a "hello world" on a WLAN and a SoftAP at the same time.
   The SoftAP allow you to configure WLAN parameters at run time. They are not setup in the sketch but saved on EEPROM.

   Connect your computer or cell phone to wifi network ESP_ap with password 12345678. A popup may appear and it allow you to go to WLAN config. If it does not then navigate to http://192.168.4.1/wifi and config it there.
   Then wait for the module to connect to your wifi and take note of the WLAN IP it got. Then you can disconnect from ESP_ap and return to your regular WLAN.

   Now the ESP8266 is in your network. You can reach it through http://192.168.x.x/ (the IP you took note of) or maybe at http://esp8266.local too.

   This is a captive portal because through the softAP it will redirect any http request to http://192.168.4.1/
*/

/* Set these to your desired softAP credentials. They are not configurable at runtime */
const char *softAP_ssid = "IoTopenTech01";
const char *softAP_password = "12345678";

/* hostname for mDNS. Should work at least on windows. Try http://esp8266.local */
const char *myHostname = "iotopentech";

/* Don't set this wifi credentials. They are configurated at runtime and stored on EEPROM */
char ssid[32] = "";
char password[32] = "";

// DNS server
const byte DNS_PORT = 53;
DNSServer dnsServer;

// Web server
ESP8266WebServer server(80);

/* Soft AP network parameters */
IPAddress apIP(192, 168, 1, 1);
//IPAddress apIP(8, 8, 8, 8);
IPAddress netMsk(255, 255, 255, 0);


/** Should I connect to WLAN asap? */
boolean connect;

/** Last time I tried to connect to WLAN */
unsigned long lastConnectTry = 0;

/** Current WLAN status */
unsigned int status = WL_IDLE_STATUS;
void preinit() {
  // Global WiFi constructors are not called yet
  // (global class instances like WiFi, Serial... are not yet initialized)..
  // No global object methods or C++ exceptions can be called in here!
  //The below is a static class method, which is similar to a function, so it's ok.
  ESP8266WiFiClass::preinitWiFiOff();
}
byte modo = 0; //1. Configuracion   2. Scan AP


void setup() {
 
  pinMode(14, OUTPUT);
  digitalWrite(14, LOW);
  Serial.begin(9600);
  

  //Vamos a tener 2 modos de funcionamiento
  //1. Modo configuración
  //2. Modo scan AP
  /*
  int caracter;
  while (modo == 0) {
    Serial.write("IoT");//Para indicar que está listo
    delay(1000);
    if (Serial.available()) {
      caracter = Serial.read();
      if (caracter == 49) {//el 1 en ascii es DEC 49
        modo = 1;
      } else if (caracter == 50) {
        modo = 2;
      } 
    }
  }
  */
   //En el caso de TTNMAD_SOS siempre usaremos el modo Scan AP
  modo=2;
  if (modo == 1) {
    WiFi.setOutputPower(0);
    //Serial.print("Configuring access point...");
    /* You can remove the password parameter if you want the AP to be open. */
    WiFi.softAPConfig(apIP, apIP, netMsk);
    WiFi.softAP(softAP_ssid, softAP_password);
    delay(500); // Without delay I've seen the IP address blank
    Serial.write("A");//Es solo un indicador interno que utilizo con el monitor serie
    //Serial.print("AP IP address: ");
    //Serial.println(WiFi.softAPIP());

    /* Setup the DNS server redirecting all the domains to the apIP */
    //dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(DNS_PORT, "*", apIP);

    /* Setup web pages: root, wifi config pages, SO captive portal detectors and not found. */
    server.on("/", handleRoot);
    server.on("/conf", handleConf);
    server.on("/save", handleSave);
    server.on("/generate_204", handleRoot);  //Android captive portal. Maybe not needed. Might be handled by notFound handler.
    server.on("/fwlink", handleRoot);  //Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
    server.onNotFound(handleRoot);

    server.begin(); // Web server start
    //Serial.println("HTTP server started");
    //No cargo las credenciales nunca por seguridad; si alguien consigue acceso al esp03, al menos no se lleva gratis las credenciales lorawan del nodo
    //loadCredentials(); // Load WLAN credentials from network
    //connect = strlen(ssid) > 0; // Request WLAN connect if there is a SSID
    //connect = false;
  } else if (modo == 2) {
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
  }
}

void connectWifi() {
  Serial.println("Connecting as wifi client...");
  WiFi.disconnect();
  WiFi.begin(ssid, password);
  int connRes = WiFi.waitForConnectResult();
  Serial.print("connRes: ");
  Serial.println(connRes);
}

void loop() {
  if (modo == 1) {
    // Do work:
    //DNS
    dnsServer.processNextRequest();
    //HTTP
    server.handleClient();
  } else if (modo == 2) {
    int n = WiFi.scanNetworks();
    if (n == 0) {
      Serial.println("*");
    } else {
      //Voy a coger los 2 AP con mejor RSSI
      long rssi_leido;
      int ap1 = -1;
      long rssimax1 = -300;
      int ap2 = -1;
      long rssimax2 = -300;
      for (int i = 0; i < n; ++i) {
        rssi_leido=WiFi.RSSI(i);
        if (rssi_leido > rssimax1) {
          rssimax2 = rssimax1;
          ap2 = ap1;
          rssimax1 = rssi_leido;
          ap1 = i;
          
        } else if (rssi_leido > rssimax2) {
          rssimax2 = rssi_leido;
          ap2 = i;
         
        } 
      }

      Serial.write(WiFi.BSSID(ap1),6);
      Serial.write("#");
      if (ap2 != 0) {
        Serial.write(WiFi.BSSID(ap2),6);
        Serial.write("#");
      }
      
    }
    delay(5000);
  }
}
