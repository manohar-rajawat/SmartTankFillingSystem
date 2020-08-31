#include <ESP8266WiFi.h> // This library is used for Wifi Setup.
#include <ESP8266mDNS.h> //This library is used for ESP mDNS setup.
#include <WiFiUdp.h> //This library is used for Wifi UDP Traffic update.
#include <ArduinoOTA.h> //This library is used for Arduino Over The Air update.
#include <ESP8266WebServer.h> //This library is used for hosting http web server. 
#include <WebSocketsServer.h> //This library is used to syncronize all the clients in real time without refreshing the web page.
#include <WiFiClient.h> //This library is used for wifi setup.
#include <EEPROM.h> //This library will be used to save the level value on eeprom.

#include "rootPage.h" //This file is in the same directory.

#ifndef STASSID
#define ROUTERID "HomeAutomation"
#define ROUTERPSK "12345670"
#endif

#define TANK_HEIGHT 80 //This is the height of the tank.
#define TANK_EMPTY_THRESHOLD_PERCENTAGE 20 //If Tank is empty more than this percentage then motor will start.
#define TANK_FULL_THRESHOLD_PERCENTAGE 75 //When motor will be filling, Only this much of height will be filled in the tank.
#define TANK_VALUE_LIMIT 1.2 //We will multiply the tank height with this if sensor value is getting more than this we will not start the motor (EG. Tank Open);

#define MOTOR_DRY_TIMEOUT 30 //if water level is not increased withint this period then turn of the motor (Values in Seconds).
#define RELAY 13 //The signal pin to which relay will be connected to

#define ADDR_LEVEL 0 //This is the address of the EEPROM to store the last updated status of the water tank.
#define ADDR_AUTOMATIC 1 //This is the address of the EEPROM to stroe the last updated setting of the automatic mode.

bool isAutomaticModeEnabled = true; //This mode controlls the setting of enable/disable running mode.
int updatedWaterLevel = TANK_HEIGHT;
bool isMotorRunning = false;
bool isAutomaticRunningMode = false; //This will be used to check if motor is started by the automatic mode or by the user(Web Interface).
unsigned long previousMotorUpdateTime = 0; //This will be used to check if the water level has been updated within 30 seconds or not if not then turn off the motor.
int previousTankPercentage = 0;
String Logs = ""; //This will be used to store the logs of the events.
String runningTimeCenterAp = "Not Updated";

bool isUploadModeFlag = false; //Esp Will check it to enter in boot mode.

//This are the values we use to convert millis into day:hour:minutes:seconds.
long m_day = 86400000; // 86400000 milliseconds in a day
long m_hour = 3600000; // 3600000 milliseconds in an hour
long m_minute = 60000; // 60000 milliseconds in a minute
long m_second =  1000; // 1000 milliseconds in a second

//This function is used to store the logs in string, can be retrieved by http://Ip/getLogs.
void storeLogs(bool isError, int value, String message = "Routine Update") {
  if (!isError) {
    Logs += "<div> Time :- " + getTime(millis()) + " Data :- " + String(value) + " Message :- " + message + "</div>";
  } else {
    Logs += "<div> Time :- " + getTime(millis()) + " Error Value :- " + String(value) + " Message :- " + message + "</div>";
  }
}

//This function is used to get the running time of the ESP, can be retrieved by http://Ip/getTime.
String getTime(long timeNow) {
  int Fdays = timeNow / m_day ;                                //number of days
  int Fhours = (timeNow % m_day) / m_hour;                       //the remainder from days division (in milliseconds) divided by hours, this gives the full hours
  int Fminutes = ((timeNow % m_day) % m_hour) / m_minute ;         //and so on...
  int Fseconds = (((timeNow % m_day) % m_hour) % m_minute) / m_second;
  return String(Fdays) + ":" + String(Fhours) + ":" + String(Fminutes) + ":" + String(Fseconds);
}

ESP8266WebServer server(80); // Web server running on port 80.
WebSocketsServer webSocket = WebSocketsServer(81); //Web socket server running on port 81.

void initializeEEPROM() {
  EEPROM.begin(512); //Initializing the eeprom
}

void handleRoot() {
  server.send(200, "text/html", rootPage);
}

int getTankPercentage() {
  return (int)(((TANK_HEIGHT - updatedWaterLevel) * 100) / TANK_HEIGHT);
}

void initialValue() {
  String isRunning = isMotorRunning ? "true" : "false";
  String isAutomaticEnabled = isAutomaticModeEnabled ? "true" : "false";
  String fillPercentage = String(getTankPercentage());
  String completeString = isRunning + "/" + fillPercentage + "/" + updatedWaterLevel + "/" + isAutomaticEnabled ;
  server.send(200, "text/html", completeString);
}

void broadcastEvent() {
  String isRunning = isMotorRunning ? "true" : "false";
  String isAutomaticEnabled = isAutomaticModeEnabled ? "true" : "false";
  String waterLevel = String(getTankPercentage());
  String command = isRunning + "/" + waterLevel + "/" + updatedWaterLevel + "/" + isAutomaticEnabled ;;
  int n = command.length();
  char char_array[n + 1];
  strcpy(char_array, command.c_str());
  webSocket.broadcastTXT(char_array, sizeof(char_array));
}

void startAutomaticRunningMode() {
  isAutomaticRunningMode = true; //Setting the automatic running mode to true.
  previousMotorUpdateTime = millis(); //Set the previous time as current time.
  previousTankPercentage = getTankPercentage(); //Set the previosu water level as current water level it will help us to track the changes in water level.
  startMotor(); //Now we'll start the motor and will check for continous updates.
  broadcastEvent();
}

void stopAutomaticRunningMode(String message = "Motor Stopped !") {
  isAutomaticRunningMode = false;
  stopMotor();
  storeLogs(false, updatedWaterLevel, message); //Store Motor Starting Log accordingly.]
  broadcastEvent();
}

void checkMotorStatusLoop() {
  if (isAutomaticRunningMode) {
    unsigned long currentTime = millis(); //get the current time value to get the difference between the previous and new one.
    if (currentTime - previousMotorUpdateTime >= MOTOR_DRY_TIMEOUT * 1000) {
      previousMotorUpdateTime = currentTime;
      if (getTankPercentage() >= TANK_FULL_THRESHOLD_PERCENTAGE) { //Tank has been filled to set limit. Now turning off the motor.
        stopAutomaticRunningMode("Tank Has Filled Successfully. Triggered Motor Stopped !");
        return;
      }
      if (getTankPercentage() <= previousTankPercentage) { //If the water level has not increased in the tank then we will stop the motor.
        isAutomaticModeEnabled = false; //Now the auto mode has been disabled and switched to manual.
        saveValueInEEEPROM(ADDR_AUTOMATIC, 0); //It motor was running dry then we will stop the automatic mode until someone turns it on.
        stopAutomaticRunningMode("The Motor Was Running Dry. Triggered Motor Stop!"); //We'll stop the motor immediately with logging the data appropriately.
        storeLogs(false, updatedWaterLevel, "Turning Off Automatic Mode !");
      } else {
        previousTankPercentage = getTankPercentage(); //If the water level has been increased within last 30 seconds then update the previous value to check next time.
      }
    }
  }
}

void startMotor() {
  digitalWrite(RELAY, LOW);
  isMotorRunning = true;
}

void stopMotor() {
  digitalWrite(RELAY, HIGH);
  isMotorRunning = false;
  isAutomaticRunningMode = false; //Overriding the automatic mode;
}

void turnOnMotor() {
  startMotor();
  server.send(200, "text/html", "<h1><Center>Motor Turned On !</Center></h1>");
  broadcastEvent();
  storeLogs(false, updatedWaterLevel, "Motor Strated Manually !");
}

void turnOffMotor() {
  stopMotor();
  server.send(200, "text/html", "<h1><Center>Motor Turned Off !</Center></h1>");
  broadcastEvent();
  storeLogs(false, updatedWaterLevel, "Motor Stopped Manually !");
}

void saveValueInEEEPROM(int addr, int value) {
  EEPROM.write(addr, char(value)); //Saving the values as character rather than integer.
  EEPROM.commit(); //Saving the changes in the EEPROM.
}

int getValuefromEEPROM(int addr) {
  return int(EEPROM.read(addr)); //Converted that char value in int.
}

void updateLastValue() {
  int lastKnownLevel = getValuefromEEPROM(ADDR_LEVEL); //This fetches the last updated/stored water level value from EEPROM.
  if (lastKnownLevel < 0 || lastKnownLevel > TANK_HEIGHT) {
    lastKnownLevel = TANK_HEIGHT;
  }
  updatedWaterLevel = lastKnownLevel; //Update the last known level to current running programme level.
  isAutomaticModeEnabled = getValuefromEEPROM(ADDR_AUTOMATIC) == 1 ? true : false ; //This fethces the last enable/disable mode value from EEPROM.
}

void updateAutomaticMode() {
  if (server.hasArg("automaticMode")) {
    String argValue = server.arg("automaticMode");
    if (argValue == "true") {
      isAutomaticModeEnabled = true;
      saveValueInEEEPROM(ADDR_AUTOMATIC, 1); //Updating the automatic mode value in EEPROM.
      storeLogs(false, updatedWaterLevel, "Mode Changed To Automatic");
    } else {
      isAutomaticModeEnabled = false;
      storeLogs(false, updatedWaterLevel, "Mode Changed To Manual");
      saveValueInEEEPROM(ADDR_AUTOMATIC, 0); //Updating the automatic mode value in EEPROM.
    }
    String intToStringValue = isAutomaticModeEnabled ? "true" : "false";
    server.send(200, "text/html", intToStringValue);
    broadcastEvent();
  } else {
    server.send(400, "text/html", "Argument Missing !");
  }
}

bool isNumber(String value) {
  for (int i = 0; i < value.length(); i++) {
    if (isDigit(value.charAt(i))) {
    } else return false;
  }
  return true;
}

void updateWaterLevel() {
  int previousValue = updatedWaterLevel;
  if (server.method() == HTTP_POST) {
    if (server.hasArg("tankemptyheight")) {
      String argValue = server.arg("tankemptyheight");
      if (isNumber(argValue)) {
        int receivedValue = argValue.toInt();
        if (receivedValue < 0 || receivedValue >= TANK_HEIGHT * TANK_VALUE_LIMIT) { //This block of code will check for false values and return appropriate status.
          server.send(400, "text/html", "<h1><Center>The Water Level Is Out Of Range !</center></h1>");
          storeLogs(true, receivedValue, "The Water Level Is Out Of Range");
        } else { //This block of code will check for correct values and will return success status(200).
          updatedWaterLevel = receivedValue;
          if (!isAutomaticModeEnabled) { //If the automatic mode is disabled then we'll only update the values will not turn the motor on.
            storeLogs(false, updatedWaterLevel, "Updating Only... Automatic Mode Disabled !"); //Store Normal Update Log.
            server.send(200, "text/html", "<h1><Center>Water Level Updated. Automatic Mode Disabled !</Center></h1>");
          } else {
            if (getTankPercentage() > TANK_EMPTY_THRESHOLD_PERCENTAGE) {
              if (!isAutomaticRunningMode) {
                storeLogs(false, updatedWaterLevel); //Store Normal Update Log.
                server.send(200, "text/html", "<h1><Center>Water Level Updated !</Center></h1>");
              } else {
                if (getTankPercentage() >= TANK_FULL_THRESHOLD_PERCENTAGE) {
                  stopAutomaticRunningMode("Tank Has Filled Successfully. Triggered Motor Stopped !"); //Motor has filled successfully. Now stopping the motor.
                  server.send(200, "text/html", "<h1><Center>Water Level Updated Tank Has Filled Successfully!</Center></h1>");
                } else {
                  storeLogs(false, updatedWaterLevel, "Update During Automatic Mode"); //Store Normal Update Log.
                  server.send(202, "text/html", "<h1><Center>Water Level Updated During Motor Running Mode Please Continue Sending Updates ... !</Center></h1>");
                }
              }
            } else {
              if (!isAutomaticRunningMode) {
                startAutomaticRunningMode(); //This will start the motor and initiate the variables accordingly.
                storeLogs(false, updatedWaterLevel, "Tank Level Found Less Than Defined. Triggered Motor Started !"); //Store Motor Starting Log
                server.send(201, "text/html", "<h1><Center>Water Level Updated And Motor Started !</Center></h1>");
              } else {
                server.send(202, "text/html", "<h1><Center>Water Level Updated During Motor Running Mode Please Continue Sending Updates ... !</Center></h1>");
              }
            }
          }
          if (previousValue != updatedWaterLevel) {
            previousValue = updatedWaterLevel;
            saveValueInEEEPROM(ADDR_LEVEL, updatedWaterLevel); //Stores the last updated value in EEPROM.
            broadcastEvent();
          }
        }
      } else {
        server.send(400, "text/html", "<h1><Center>Updated Value Should be Integer Only !</Center></h1>");
      }
    }
    else {
      server.send(400, "text/html", "<h1><Center>Updated Argument Value Missing !</Center></h1>");
    }
  }
  else {
    server.send(404, "text/html", "<h1><Center>Only Post Method Allowed !</Center></h1>");
  }
}

void restartESP() {
  server.send(200, "text/html", "<h1><Center>ESP Restarted !!!</Center></h1>");
  delay(1000);
  ESP.restart();
}

void getRunningTime() {
  long currentMillis = millis();
  String currentTime = getTime(currentMillis);
  server.send(200, "text/html", "<h1><Center>" + currentTime + "</Center></h1>");
}

void runningTimeUpdateCenter() {
  if (server.method() == HTTP_POST) {
    if (server.hasArg("runningTime")) {
      runningTimeCenterAp = server.arg("runningTime");
      server.send(200,"text/html","Ping Received");
    } else {
      server.send(400,"text/html","Time Argument Missing");
    }
  } else {
    server.send(200, "text/html", "<h1><Center>" + runningTimeCenterAp + "</Center></h1>");
  }
}

void getLogs() {
  server.send(200, "text/html", Logs);
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
  server.send(200, "text/html", "<h1><center>Flag Set : \"" + isUploadModeFlagString + "\" </center></h1>");
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

void notFound() {
  server.send(404, "text/html", "<h1><Center>This page is not there !</Center></h1>");
}

void initializeWebSocket() {
  webSocket.begin();
}

void initializeRelay() {
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, HIGH);
}

void initializeWifi() {
  WiFi.mode(WIFI_OFF); //Turn the wifi off first to reduce ambuiguity.
  delay(1000);
  WiFi.mode(WIFI_STA); //Now put the esp again in station mode.
  const char* routerssid = ROUTERID;
  const char* routerpassword = ROUTERPSK;

  IPAddress local_IP(192, 168, 1, 150);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress primaryDNS(8, 8, 8, 8);
  IPAddress secondaryDNS(8, 8, 4, 4);

  WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
  WiFi.begin(routerssid, routerpassword);
  int RetryCount = 0;
  while (WiFi.status() != WL_CONNECTED && RetryCount < 10) {
    delay(500);
    RetryCount++;
  }
}

void initializeServer() {
  server.on("/", handleRoot);
  server.on("/motorOn", turnOnMotor);
  server.on("/motorOff", turnOffMotor);
  server.on("/updateAutomaticMode", updateAutomaticMode);
  server.on("/updateLevel", updateWaterLevel);
  server.on("/restart", restartESP);
  server.on("/getTime", getRunningTime);
  server.on("/initialValue", initialValue);
  server.on("/getLogs", getLogs);
  server.on("/setUploadMode", setUploadMode);
  server.on("/getUploadMode", getUploadMode);
  server.on("/RunningTimeCenterAp", runningTimeUpdateCenter);
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
  initializeEEPROM();
  updateLastValue();
  initializeRelay();
  initializeWifi(); //Wifi initialization.
  initializeServer(); //Server initialization.
  initializeWebSocket(); //Web socket server.
  initializeOTA(); //ESP OTA UPdate.
}

void loop() {
  webSocket.loop(); //This will be used to provide realtime update to the connected devices without needing them refreshing the web page.
  ArduinoOTA.handle(); //This will help the administrator to upload the code through server.
  server.handleClient(); //This will check for incoming connections and then server the content over http.
  checkMotorStatusLoop(); //This will check the motor status if motor is running or not.
}
