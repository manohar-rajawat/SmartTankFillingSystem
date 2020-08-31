#include <ESP8266WiFi.h> // This library is used for Wifi Setup.
#include <ESP8266mDNS.h> //This library is used for ESP mDNS setup.
#include <WiFiUdp.h> //This library is used for Wifi UDP Traffic update.
#include <ArduinoOTA.h> //This library is used for Arduino Over The Air update.
#include <ESP8266WebServer.h> //This library is used for hosting http web server. 
#include <WiFiClient.h> //This library is used for wifi setup.
#include <ESP8266HTTPClient.h> //This library is used to make http request to the server.

//AP Configurations
#define ROUTERID "HomeAutomation"
#define ROUTERPSK "12345670"

//Pin Definitions
#define ULTRASONIC 5
#define TRIGGER 13
#define ECHO 12
#define WATER_LEVEL_AVERAGE 10

//Deep Sleep Times (In minutes)
#define DEEP_SLEEP_TIMER 65

#define MOTOR_RUNNING_MODE_UPDATE_DELAY 5 //This is the value in seconds.

String commandMode = "Running";

ESP8266WebServer server(80); // Web server running on port 80.

void goToSleep(int sleepTime) {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  putPinInLowMode();
  delay(10); //Let the pins be low.
  ESP.deepSleep(sleepTime * 60 * 1000000UL);
}

void getlevel() {
  int tankEmptyHeight = waterLevel();
  server.send(200, "text/html", "<center><h1>" + String(tankEmptyHeight) + "</h1></center>");
}

int calculateDistance() {
  long duration; //variable to calculate the duration of the sound wave
  int distance; //variable to calculate the distance to the object.
  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER, LOW);
  duration = pulseIn(ECHO, HIGH); //pulseIn(pin,value,timeout)
  distance = duration * 0.0344 / 2; //Speed of light is .0344 cm(centimeter)/us(Microseconds)
  return distance;
}

int waterLevel()
{
  int currentWaterLevel = 0;
  for (int i = 0; i < WATER_LEVEL_AVERAGE; i++)
  {
    int currentDistance = calculateDistance();
    if (i != 0)
      currentWaterLevel += currentDistance;
    delay(50);
  }
  currentWaterLevel /= (WATER_LEVEL_AVERAGE - 1) ;
  return currentWaterLevel;
}

int sendUpdate(int tankEmptyHeight) {
  HTTPClient http;
  String postData;                                                                                          //Post Data Variable
  postData = "tankemptyheight=" + String(tankEmptyHeight);
  http.begin("http://192.168.1.150/updateLevel");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");                                      //Specify content-type header
  int httpCode = http.POST(postData);                                                                       //Send the request
  http.end();
  return httpCode;
}

/*
  void sendTimelyUpdates() {
  unsigned long previousTime = millis();
  while (1) {
    unsigned long currentTime = millis();
    if (currentTime - previousTime >= 7000) {
      previousTime = currentTime;
      int statusCode = sendUpdate(waterLevel());
    }
    ESP.wdtFeed();
  }
  }
*/

void sendTimelyUpdates() {
  int statusCode = 0;
  if (WiFi.status() == WL_CONNECTED) {
    int tankEmptyHeight = waterLevel();
    statusCode = sendUpdate(tankEmptyHeight);
  }
  if (statusCode == 201) { //If status code is 201 then motor has started and machine have to send status each 5 seconds.
    sendContinousUpdates();
  } else { //If status code is not 201 then go to sleep code no matter what was the result.
    goToSleep(DEEP_SLEEP_TIMER);
  }
}


void sendContinousUpdates() {
  int errorCount = 0;
  unsigned long previousTime = millis();
  while (true) {
    unsigned long currentTime = millis();
    if (errorCount >= 3) {
      //There is some issue in the connection or while updating the values so putting esp in deep sleep mode.
      goToSleep(DEEP_SLEEP_TIMER);
    }
    if (currentTime - previousTime >= MOTOR_RUNNING_MODE_UPDATE_DELAY * 1000) {
      previousTime = currentTime;
      if (WiFi.status() == WL_CONNECTED) {
        int tankEmptyHeight = waterLevel();
        int statusCode = sendUpdate(tankEmptyHeight);
        if (statusCode == 202) {
          continue; // Updating to the server properly
        } else if (statusCode == 200) {
          goToSleep(DEEP_SLEEP_TIMER); //Water Tank Filled Successfully. Go to deep sleep mode.
        } else {
          errorCount++;
        }
      } else {
        errorCount++;
      }
    }
    ESP.wdtFeed();
  }
}

void getCommandMode() {
  HTTPClient http;
  http.begin("http://192.168.1.150/getUploadMode");
  int httpCode = http.GET();
  if (httpCode == 200) {
    commandMode = "Boot";
  }
  http.end();
}

void putPinInLowMode() {
  digitalWrite(TRIGGER, LOW);
  digitalWrite(ULTRASONIC, LOW);
}

void handleRoot() {
  server.send(200, "text/html", "<h1><Center>Hi I'm The ESP12 Tank Controller ! Mode :- " + commandMode  + "</Center></h1>");
}

void restartESP() {
  server.send(200, "text/html", "<h1><Center>ESP Restarted !!!</Center></h1>");
  delay(1000);
  ESP.restart();
}

void notFound() {
  server.send(200, "text/html", "<h1><Center>This page is not there !</Center></h1>");
}

void initializePin() {
  pinMode(TRIGGER, OUTPUT); //Setting pin mode
  pinMode(ECHO, INPUT);
  pinMode(ULTRASONIC, OUTPUT);
  digitalWrite(ULTRASONIC, HIGH);
}

bool initializeWifi() {
  WiFi.mode(WIFI_OFF);
  const char* routerssid = ROUTERID;
  const char* routerpassword = ROUTERPSK;

  IPAddress local_IP(192, 168, 1, 140);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress primaryDNS(8, 8, 8, 8);
  IPAddress secondaryDNS(8, 8, 4, 4);

  WiFi.mode(WIFI_STA);
  WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
  WiFi.begin(routerssid, routerpassword);
  int RetryCount = 0;
  while (WiFi.status() != WL_CONNECTED && RetryCount < 20) {
    delay(500);
    RetryCount++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }
  else {
    return false;
  }
}

void initializeServer() {
  server.on("/", handleRoot);
  server.on("/restart", restartESP);
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

void setup() {
  //Serial.begin(115200);
  //Serial.println("Booting");
  initializePin();
  if ( initializeWifi()) //Wifi initialization.
  {
    getCommandMode();
  } else {
    goToSleep(DEEP_SLEEP_TIMER);
  }
  if (commandMode != "Running") {
    initializeServer(); //Server initialization.
    initializeOTA(); //ESP OTA UPdate.
  } else {
    sendTimelyUpdates();
  }
}

void loop() {
  if (commandMode != "Running") {
    ArduinoOTA.handle();
    server.handleClient();
  }
}
