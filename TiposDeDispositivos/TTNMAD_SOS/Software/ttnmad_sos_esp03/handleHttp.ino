/** Handle root or redirect to captive portal */
void handleRoot() {
  if (captivePortal()) { // If caprive portal redirect instead of displaying the page.
    return;
  }
  server.send(200, "text/html", "<html><head></head><body><h1>IoTopenTech</h1><p>Haga clic <a href='/conf'>para configurar su nodo</a>.</p></body></html>");
  /*
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server.sendContent(
    "<html><head></head><body>"
    "<h1>IoTopenTech</h1>"
    );

    server.sendContent(
    "<p>Haga clic <a href='/conf'>para configurar su nodo</a>.</p>"
    "</body></html>"
    );
  */
  server.client().stop(); // Stop is needed because we sent no content length
}

/** Redirect to captive portal if we got a request for another domain. Return true in that case so the page handler do not try to handle the request again. */
boolean captivePortal() {
  if (!isIp(server.hostHeader()) && server.hostHeader() != (String(myHostname) + ".local")) {
    //Serial.print("Request redirected to captive portal");
    server.sendHeader("Location", String("http://") + toStringIp(server.client().localIP()), true);
    server.send(302, "text/plain", "");   // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server.client().stop(); // Stop is needed because we sent no content length
    return true;
  }
  return false;
}

/** Wifi config page handler */
void handleConf() {
  /*
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  */
  server.send(200, "text/html", "<html><head><script type='text/javascript'>function gestionar_campos(valor){if (valor=='OTAA'){var campos_off = document.getElementsByClassName('ABP');var campos_on = document.getElementsByClassName('OTAA');}"
              "else{var campos_off = document.getElementsByClassName('OTAA');var campos_on = document.getElementsByClassName('ABP');}"
              "for (var i = 0; i < campos_off.length; i++) { campos_off[i].disabled = true;campos_off[i].style.display='none';}"
              "for (var i = 0; i < campos_on.length; i++) { campos_on[i].disabled = false;campos_on[i].style.display='inline';}}"
              "</script></head><body>"
              "<h1>IoTopenTech</h1>"
              "<h2>Configuraci&oacute;n del nodo</h2>"
              "\r\n<br /><form method='POST' action='save'>"
              "<input type='radio' name='abp_otaa' id='radio_otaa' value='OTAA' checked onchange='gestionar_campos(value)'/>OTAA<br/>"
              "<input type='radio' name='abp_otaa' id='radio_abp' value='ABP' onchange='gestionar_campos(value)'/>ABP"
              "<br class='OTAA'/><input class='OTAA' type='text' placeholder='DEVEUI' name='deveui' required minlength='16' maxlength='16' size='16'/>"
              "<br class='OTAA'/><input class='OTAA' type='text' placeholder='APPEUI' name='appeui' required minlength='16' maxlength='16' size='16'/>"
              "<br class='OTAA'/><input class='OTAA' type='text' placeholder='APPKEY' name='appkey' required minlength='32' maxlength='32' size='32'/>"
              "<br class='ABP' style='display: none;'/><input class='ABP' type='text' placeholder='DEVADDR' name='devaddr'  disabled style='display: none;' required minlength='8' maxlength='8' size='8'/>"
              "<br class='ABP' style='display: none;'/><input class='ABP' type='text' placeholder='NWKSKEY' name='nwkskey'  disabled style='display: none;' required minlength='32' maxlength='32' size='32'/>"
              "<br class='ABP' style='display: none;'/><input class='ABP' type='text' placeholder='APPSKEY' name='appskey'  disabled style='display: none;' required minlength='32' maxlength='32' size='32'/>"
              "<br/><input type='submit' value='ENVIAR'/></form>"
              "</body></html>"

             ); // Empty content inhibits Content-length header so we have to close the socket ourselves.

  /*server.sendContent(
    "<html><head><script type='text/javascript'>function gestionar_campos(valor){if (valor=='OTAA'){var campos_off = document.getElementsByClassName('ABP');var campos_on = document.getElementsByClassName('OTAA');}"
    "else{var campos_off = document.getElementsByClassName('OTAA');var campos_on = document.getElementsByClassName('ABP');}"
    "for (var i = 0; i < campos_off.length; i++) { campos_off[i].disabled = true;campos_off[i].style.display='none';}"
    "for (var i = 0; i < campos_on.length; i++) { campos_on[i].disabled = false;campos_on[i].style.display='inline';}}"
    "</script></head><body>"
    "<h1>IoTopenTech</h1>"
    "<h2>Configuraci&oacute;n del nodo</h2>"
    );
  */
  /*
    if (server.client().localIP() == apIP) {
    server.sendContent(String("<p>You are connected through the soft AP: ") + softAP_ssid + "</p>");
    } else {
    server.sendContent(String("<p>You are connected through the wifi network: ") + ssid + "</p>");
    }

    server.sendContent(
    "\r\n<br />"
    "<table><tr><th align='left'>SoftAP config</th></tr>"
    );
    server.sendContent(String() + "<tr><td>SSID " + String(softAP_ssid) + "</td></tr>");
    server.sendContent(String() + "<tr><td>IP " + toStringIp(WiFi.softAPIP()) + "</td></tr>");
    server.sendContent(
    "</table>"
    "\r\n<br />"
    "<table><tr><th align='left'>WLAN config</th></tr>"
    );
    server.sendContent(String() + "<tr><td>SSID " + String(ssid) + "</td></tr>");
    server.sendContent(String() + "<tr><td>IP " + toStringIp(WiFi.localIP()) + "</td></tr>");
    server.sendContent(
    "</table>"
    "\r\n<br />"
    "<table><tr><th align='left'>WLAN list (refresh if any missing)</th></tr>"
    );
    Serial.println("scan start");
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n > 0) {
    for (int i = 0; i < n; i++) {
      server.sendContent(String() + "\r\n<tr><td>SSID " + WiFi.SSID(i) + String((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : " *") + " (" + WiFi.RSSI(i) + ")</td></tr>");
    }
    } else {
    server.sendContent(String() + "<tr><td>No WLAN found</td></tr>");
    }

    server.sendContent(
    "\r\n<br /><form method='POST' action='save'>"
    "<input type='radio' name='abp_otaa' id='radio_otaa' value='OTAA' checked onchange='gestionar_campos(value)'/>OTAA<br/>"
    "<input type='radio' name='abp_otaa' id='radio_abp' value='ABP' onchange='gestionar_campos(value)'/>ABP<br/>"
    "<input class='OTAA' type='text' placeholder='APPEUI' name='appeui' required/>"
    "<br class='OTAA'/><input class='OTAA' type='text' placeholder='DEVEUI' name='deveui' required/>"
    "<br class='OTAA'/><input class='OTAA' type='text' placeholder='APPKEY' name='appkey' required/>"
    "<input class='ABP' type='text' placeholder='DEVADDR' name='devaddr'  disabled style='display: none;' required/>"
    "<br class='ABP' style='display: none;'/><input class='ABP' type='text' placeholder='NWKSKEY' name='nwkskey'  disabled style='display: none;' required/>"
    "<br class='ABP' style='display: none;'/><input class='ABP' type='text' placeholder='APPSKEY' name='appskey'  disabled style='display: none;' required/>"
    "<br/><input type='submit' value='ENVIAR'/></form>"
    //"<p>You may want to <a href='/'>return to the home page</a>.</p>"
    "</body></html>"
    );
  */
  server.client().stop(); // Stop is needed because we sent no content length
}
char nibble2c(char c) {
  if ((c >= '0') && (c <= '9'))
    return c - '0' ;
  if ((c >= 'A') && (c <= 'F'))
    return c + 10 - 'A' ;
  if ((c >= 'a') && (c <= 'a'))
    return c + 10 - 'a' ;
  return -1 ;
}
/** Handle the WLAN save form and redirect to WLAN config page again */
void handleSave() {
  //Serial.println("conf save");
  if (server.arg("abp_otaa") == "OTAA") {
    Serial.print(server.arg("abp_otaa"));
    delay(2000);//Para superar el timeout del readString del atmega328
    for (int i = 0; i < 8; i++) {
      Serial.write(16 * (nibble2c(server.arg("deveui")[i * 2])) + nibble2c(server.arg("deveui")[i * 2 + 1]));
    }
    //Serial.print(server.arg("deveui"));
    delay(2000);
    for (int i = 0; i < 8; i++) {
      Serial.write(16 * (nibble2c(server.arg("appeui")[i * 2])) + nibble2c(server.arg("appeui")[i * 2 + 1]));
    }
    //Serial.print(server.arg("appeui"));
    delay(2000);
    for (int i = 0; i < 16; i++) {
      Serial.write(16 * (nibble2c(server.arg("appkey")[i * 2])) + nibble2c(server.arg("appkey")[i * 2 + 1]));
    }
    //Serial.print(server.arg("appkey"));
    delay(2000);
  } else {
    Serial.print(server.arg("abp_otaa"));
    delay(2000);//Para superar el timeout del readString del atmega328
    for (int i = 0; i < 4; i++) {
      Serial.write(16 * (nibble2c(server.arg("devaddr")[i * 2])) + nibble2c(server.arg("devaddr")[i * 2 + 1]));
    }
    //Serial.print(server.arg("devaddr"));
    delay(2000);
    for (int i = 0; i < 16; i++) {
      Serial.write(16 * (nibble2c(server.arg("nwkskey")[i * 2])) + nibble2c(server.arg("nwkskey")[i * 2 + 1]));
    }
    //Serial.print(server.arg("nwkskey"));
    delay(2000);
    for (int i = 0; i < 16; i++) {
      Serial.write(16 * (nibble2c(server.arg("appskey")[i * 2])) + nibble2c(server.arg("appskey")[i * 2 + 1]));
    }
    //Serial.print(server.arg("appskey"));
    delay(2000);
  }
  unsigned long time = millis();
  while (millis() - time < 5000 && !Serial.available()) {
    //Esperamos como mucho 5 segundos
  }
  delay (100);
  if (!Serial.available() || Serial.readString() != "B") {
    server.send(200, "text/html", "<html><head></head><body><h1>IoTopenTech</h1><p>ERROR al guardar la configuraci&oacute;n.</p></body></html>");
  } else {
    server.send(200, "text/html", "<html><head></head><body><h1>IoTopenTech</h1><p>Configuraci&oacute;n almacenada correctamente.</p></body></html>");
  }
  /*
    server.arg("n").toCharArray(ssid, sizeof(ssid) - 1);
    server.arg("p").toCharArray(password, sizeof(password) - 1);
    server.sendHeader("Location", "wifi", true);
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");
    server.send(302, "text/plain", "");    // Empty content inhibits Content-length header so we have to close the socket ourselves.
  */
  server.send(200, "text/html", "<html><head></head><body><h1>IoTopenTech</h1><p>Configuraci&oacute; almacenada.</p></body></html>");
  server.client().stop(); // Stop is needed because we sent no content length
  digitalWrite(14, LOW);
  //saveCredentials();
  //connect = strlen(ssid) > 0; // Request WLAN connect with new credentials if there is a SSID
}

void handleNotFound() {
  if (captivePortal()) { // If caprive portal redirect instead of displaying the error page.
    return;
  }
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(404, "text/plain", message);
}
