#include "namedMesh.h"
#include <Arduino_JSON.h>

#define   MESH_SSID       "prefix"
#define   MESH_PASSWORD   "password"
#define   MESH_PORT       5556

Scheduler     userScheduler; // to control your personal task
namedMesh  mesh;

String msgToSend;

String nodeName = "node8266"; // Name needs to be unique

void sendMessage();

Task taskSendMessage(TASK_SECOND * 5 , TASK_FOREVER, &sendMessage);

void sendMessage() {
  String to = "node2";
  mesh.sendSingle(to, msgToSend);
}

String receivedCallback( uint32_t from, String &msg ) {
  String msg1;
  //Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  JSONVar myObject = JSON.parse(msg.c_str());
    
    double temp = myObject["temp"];
    double hum = myObject["hum"];
    double pres = myObject["pres"];
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.println(" C");
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.println(" %");
    Serial.print("Pressure: ");
    Serial.print(pres);
    Serial.println(" hpa");
    msg1 = String(temp, 2) + " " + String(hum, 2) + " " + String(pres, 2);
    return msg1; 
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}


void setup() {
  Serial.begin(9600);

  mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION);  // set before init() so that you can see startup messages

  mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.setName(nodeName); // This needs to be an unique name! 
  mesh.onReceive([](uint32_t from, String &msg) {
    Serial.printf("Received message by id from: %u, %s\n", from, msg.c_str());
    msgToSend = receivedCallback(from, msg);
  });

  //mesh.onReceive([](String &from, String &msg) {
    //Serial.printf("Received message by name from: %s, %s\n", from.c_str(), msg.c_str());
  //});

  mesh.onChangedConnections([]() {
    Serial.printf("Changed connection\n");
  });
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
}
