 #include <ESP8266WebServer.h> //This library is used for hosting http web server.
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

#define STASSID "HomeAutomation"
#define STAPSK  "********"

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);
bool isUploadModeFlag = false;

void handleRoot() {
  server.send(200, "text/html", "<h1><Center>Hello I'm ESP12 Home Automation Server !</Center></h1>");
}

void setUploadMode() {
  if (server.hasArg("uploadmode")) {
    if (server.arg("uploadmode") == "true") {
      isUploadModeFlag = true;
    }
    else
      isUploadModeFlag = false;
  }
  else {
    isUploadModeFlag = false;
  }
  String isUploadModeFlagString = isUploadModeFlag ? "True" : "False";
  server.send(200,"text/html","<h1><center>Flag Set : \""+isUploadModeFlagString+"\" </center></h1>");
}

void getUploadMode() {
  if (isUploadModeFlag) {
    server.send(200, "text/html", "true");
  }
  else
  {
    server.send(204, "text/html", "false");
  }
}

void restartESP() {
  server.send(200, "text/html", "<h1><Center>ESP Restarted !!!</Center></h1>");
  delay(1000);
  ESP.restart();
}

void notFound() {
  server.send(200, "text/html", "<h1><Center>This page is not there !</Center></h1>");
}

void initializeWifi() {
  WiFi.mode(WIFI_AP);
  IPAddress local_ip(192, 168, 1, 1);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  boolean result = WiFi.softAP(ssid, password);
  if (result == false)
  {
    delay(5000);
    ESP.restart();
  }
}

void initializeServer() {
  server.on("/", handleRoot);
  server.on("/restart", restartESP);
  server.on("/setUploadMode",setUploadMode);
  server.on("/getUploadMode",getUploadMode);
  server.onNotFound(notFound);
  server.begin();
}

void initializeOTA() {
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    //Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    //Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    //Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    //Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      //Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      //Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      //Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      //Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      //Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  //Serial.println("Ready");
  //Serial.print("IP address: ");
  //Serial.println(WiFi.localIP());
}

void setup()
{
  initializeWifi();
  initializeOTA();
  initializeServer();
}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();
}
