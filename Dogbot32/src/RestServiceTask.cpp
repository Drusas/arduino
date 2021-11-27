#include "ArduinoJson.h"
#include <iostream>
#include "RestServiceTask.h"
#include "SpotFacade.h"
#include "QuadrupedFsm.h"
#include "WebServer.h"
#include "WiFi.h"

#include <map>

const char *SSID = "TP-209";
const char *PWD = "BuddyWireless!";

static const uint16_t JSON_BUF_SIZE = 500;
static StaticJsonDocument<JSON_BUF_SIZE> jsonDocument;
static char buffer[JSON_BUF_SIZE];
static WebServer server;
Quadruped *RestServiceTask::spot;
State *RestServiceTask::spotState;
SpotFacade *RestServiceTask::spotFacade;

std::map<std::string, SpotFunction> RestServiceTask::commandMap;

void setupRouting();
void connectToWiFi();

void RestServiceTask::enable() {
    RestServiceTask::spotFacade->setEnabled(true);
}

void RestServiceTask::disable() {
    RestServiceTask::spotFacade->setEnabled(false);
}

RestServiceTask::RestServiceTask() {
    RestServiceTask::spotFacade = nullptr;
    RestServiceTask::spot = nullptr;
    RestServiceTask::spotState = nullptr;
}

void RestServiceTask::configure(int interval, SpotFacade *sf, Quadruped *q, State *s) {
    setEnabled(false);
    updateInterval = interval;
    RestServiceTask::spotFacade = sf;
    RestServiceTask::spot = q;
    RestServiceTask::spotState = s;
    commandMap["enable"] = &RestServiceTask::enable;
    commandMap["disable"] = &RestServiceTask::disable;
}

void RestServiceTask::executeCommand(std::string command) {
    if (RestServiceTask::spot == nullptr) {
        return;
    }

    transform(command.begin(), command.end(), command.begin(), ::tolower);
    auto iter = commandMap.find(command);
    if (iter == commandMap.end())
    {
        std::cout << "command not found" << command << std::endl;
    }
    else {
        (*iter->second)();
    }
}

boolean RestServiceTask::getConfigured() {
    return (RestServiceTask::spot != nullptr);
}

void RestServiceTask::start() {
    if (RestServiceTask::spot == nullptr) {
        return;
    }

    connectToWiFi();
    setupRouting();
}

void RestServiceTask::performUpdate() {
    if ((RestServiceTask::spot == nullptr) || (RestServiceTask::spotState == nullptr)) {
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
    if (RestServiceTask::getConfigured()) {
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

    std::string str = m;
    
    RestServiceTask::executeCommand(str);
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
    // Serial.println(buffer);
    endJson();
    server.send(200, "application/json", buffer);
}

void RestServiceTask::getLegJointAngles() {
    Serial.println("Get leg joint angles");
    startJson("modeOfOperation", getModeOfOperationStr().c_str());
    RestServiceTask::spotFacade->updateLegAngles();
    addJsonArray("RF", RestServiceTask::spot->LegPositions[0][0], RestServiceTask::spot->LegPositions[1][0], RestServiceTask::spot->LegPositions[2][0]);
    addJsonArray("LF", RestServiceTask::spot->LegPositions[0][1], RestServiceTask::spot->LegPositions[1][1], RestServiceTask::spot->LegPositions[2][1]);
    addJsonArray("RR", RestServiceTask::spot->LegPositions[0][2], RestServiceTask::spot->LegPositions[1][2], RestServiceTask::spot->LegPositions[2][2]);
    addJsonArray("LR", RestServiceTask::spot->LegPositions[0][3], RestServiceTask::spot->LegPositions[1][3], RestServiceTask::spot->LegPositions[2][3]);
    endJson();
    Serial.println(buffer);
    server.send(200, "application/json", buffer);
}

uint8_t getLegIndexFromStr(std::string legStr) {
    Serial.print("getLegIndexFromStr ");
    Serial.println(legStr.c_str());
    if (legStr.compare("RF") == 0) {
            return 0;
    }
    if (legStr.compare("LF") == 0) {
            return 1;
    }
    if (legStr.compare("RR") == 0) {
            return 2;
    }
    if (legStr.compare("LR") == 0) {
            return 3;
    }
    return 99;
}

void RestServiceTask::postLegJointAngles() {
    Serial.println("Post leg joint angles");
    if (server.hasArg("plain") == false) {
        //handle error here
    }
    String body = server.arg("plain");
    deserializeJson(jsonDocument, body);
    Serial.println(body.c_str());

    uint8_t idx = 99;
    std::string legStr = jsonDocument["leg"];
    idx = getLegIndexFromStr(legStr);
    Serial.println("Post leg joint angles");
    float hx = atof(jsonDocument["hx"]);
    float hy = atof(jsonDocument["hy"]);
    float knee = atof(jsonDocument["knee"]);

    RestServiceTask::spotFacade->setLegAngles(idx, hx, hy, knee);
}

void RestServiceTask::postLegPosition() {
    Serial.println("Post leg coordinates");
    if (server.hasArg("plain") == false) {
        //handle error here
    }
    String body = server.arg("plain");
    deserializeJson(jsonDocument, body);
    Serial.println(body.c_str());

    uint8_t idx = 99;
    std::string legStr = jsonDocument["leg"];
    idx = getLegIndexFromStr(legStr);
    float x = atof(jsonDocument["x"]);
    float y = atof(jsonDocument["y"]);
    float z = atof(jsonDocument["z"]);

    RestServiceTask::spotFacade->setLegPosition(idx, x, y, z);
}

void setupRouting() {
    server.on("/modeOfOperation", HTTP_GET, getModeOfOperation);
    server.on("/modeOfOperation", HTTP_POST, postModeOfOperation);
    server.on("/modeOfOperation", HTTP_OPTIONS, sendCrossOriginHeader);

    server.on("/legCoordinates", HTTP_GET, getLegCoordinates);
    server.on("/legCoordinates", HTTP_POST, RestServiceTask::postLegPosition);
    server.on("/legCoordinates", HTTP_OPTIONS, sendCrossOriginHeader);

    server.on("/legJointAngles", HTTP_GET, RestServiceTask::getLegJointAngles);
    server.on("/legJointAngles", HTTP_POST, RestServiceTask::postLegJointAngles);
    server.on("/legJointAngles", HTTP_OPTIONS, sendCrossOriginHeader);
            
    server.enableCORS(true);	 	 
    server.begin();
}