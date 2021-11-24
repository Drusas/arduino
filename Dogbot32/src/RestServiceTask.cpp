#include "ArduinoJson.h"
#include "RestServiceTask.h"
#include "QuadrupedFsm.h"
#include "WebServer.h"
#include "WiFi.h"

const char *SSID = "TP-209";
const char *PWD = "BuddyWireless!";

static const uint16_t JSON_BUF_SIZE = 500;
static StaticJsonDocument<JSON_BUF_SIZE> jsonDocument;
static char buffer[JSON_BUF_SIZE];
static WebServer server;
Quadruped *spot;
State *spotState;

void setupRouting();
void connectToWiFi();

RestServiceTask::RestServiceTask() {
    spot = nullptr;
    spotState = nullptr;
}

void RestServiceTask::configure(int interval, Quadruped *q, State *s) {
    setEnabled(false);
    updateInterval = interval;
    spot = q;
    spotState = s;
}

boolean getConfigured() {
    return (spot != nullptr);
}

void RestServiceTask::start() {
    if (spot == nullptr) {
        return;
    }

    connectToWiFi();
    setupRouting();
}

void RestServiceTask::performUpdate() {
    if ((spot == nullptr) || (spotState == nullptr)) {
        return;
    }

    server.handleClient();
}

void connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(SSID);
  
  WiFi.begin(SSID, PWD);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
 
  Serial.print("Connected. IP: ");
  Serial.println(WiFi.localIP());
}

void startJson(char *tag, const char *value) {  
  jsonDocument.clear();  
  jsonDocument[tag] = value;
}

void endJson() {
  serializeJson(jsonDocument, buffer);
}

void addJsonArray(char *tag, float x, float y, float z) {
  JsonArray data = jsonDocument.createNestedArray(tag);
  data.add(x);
  data.add(y);
  data.add(z);
}

void setCrossOrigin(){
    server.sendHeader(F("Access-Control-Max-Age"), F("600"));
    server.sendHeader(F("Access-Control-Allow-Methods"), F("PUT,POST,GET,OPTIONS"));
    server.sendHeader(F("Access-Control-Allow-Headers"), F("*"));
}

std::string getModeOfOperationStr() {
    std::string mode = "Unknown";
    if (getConfigured()) {
        mode = QuadrupedFsm::getCurrentState();
    }
    return mode;
}

void getModeOfOperation() {
  Serial.println("Get mode of operation");
  startJson("modeOfOperation", getModeOfOperationStr().c_str());
  endJson();
  server.send(200, "application/json", buffer);
}

void postModeOfOperation() {
  Serial.println("Post mode of operation");
  if (server.hasArg("plain") == false) {
    //handle error here
  }
  String body = server.arg("plain");
  deserializeJson(jsonDocument, body);
  Serial.println(body.c_str());

  const char *m = jsonDocument["value"];
//   mode = m;
  Serial.print("modeOfOperation: ");
  Serial.println(m);
}

void sendCrossOriginHeader(){
    Serial.println(F("sendCORSHeader"));
    setCrossOrigin();
    server.send(204);
}

void getLegCoordinates() {
  Serial.println("Get leg coordinates");
  startJson("modeOfOperation", getModeOfOperationStr().c_str());
  //updateLegCoordinates();
//   addJsonArray("RF", rfLeg[0], rfLeg[1], rfLeg[2]);
//   addJsonArray("LF", lfLeg[0], lfLeg[1], lfLeg[2]);
//   addJsonArray("RR", rrLeg[0], rrLeg[1], rrLeg[2]);
//   addJsonArray("LR", lrLeg[0], lrLeg[1], lrLeg[2]);
  endJson();
  server.send(200, "application/json", buffer);
}

void getLegJointAngles() {
  Serial.println("Get leg joint angles");
  startJson("modeOfOperation", getModeOfOperationStr().c_str());
  addJsonArray("RF", 90.1, 90.1, 135.1);
  addJsonArray("LF", 90.2, 90.2, 135.2);
  addJsonArray("RR", 90.3, 90.3, 135.3);
  addJsonArray("LR", 90.4, 90.4, 135.4);
  endJson();
  Serial.println(buffer);
  server.send(200, "application/json", buffer);
}

void postLegCoordinates() {
  Serial.println("Post leg coordinates");
  if (server.hasArg("plain") == false) {
    //handle error here
  }
  String body = server.arg("plain");
  deserializeJson(jsonDocument, body);
  Serial.println(body.c_str());

//   legs[jsonDocument["leg"]][0] = jsonDocument["x"];
//   legs[jsonDocument["leg"]][1] = jsonDocument["y"];
//   legs[jsonDocument["leg"]][2] = jsonDocument["z"];
}

void postLegJointAngles() {
  Serial.println("Post leg joint angles");
  if (server.hasArg("plain") == false) {
    //handle error here
  }
  String body = server.arg("plain");
  deserializeJson(jsonDocument, body);
  Serial.println(body.c_str());
}



void setupRouting() {
server.on("/modeOfOperation", HTTP_GET, getModeOfOperation);
server.on("/modeOfOperation", HTTP_POST, postModeOfOperation);
server.on("/modeOfOperation", HTTP_OPTIONS, sendCrossOriginHeader);

server.on("/legCoordinates", HTTP_GET, getLegCoordinates);
server.on("/legCoordinates", HTTP_POST, postLegCoordinates);
server.on("/legCoordinates", HTTP_OPTIONS, sendCrossOriginHeader);

server.on("/legJointAngles", HTTP_GET, getLegJointAngles);
server.on("/legJointAngles", HTTP_POST, postLegJointAngles);
server.on("/legJointAngles", HTTP_OPTIONS, sendCrossOriginHeader);
  	 	 
  server.enableCORS(true);	 	 
  server.begin();
}