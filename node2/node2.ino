#include "namedMesh.h"

#define   MESH_SSID       "prefix"
#define   MESH_PASSWORD   "password"
#define   MESH_PORT       5556
#define ONBOARD_LED  2

Scheduler     userScheduler; // to control your personal task
namedMesh  mesh;

String nodeName = "node2"; // Name needs to be unique

void sendMessage();

Task taskSendMessage(TASK_SECOND * 5 , TASK_FOREVER, &sendMessage);

void sendMessage() {
  String msg = "Hello from node 2 ";
  mesh.sendBroadcast(msg);
}

void receivedCallback( uint32_t from, String &msg ) {
  String helper = msg.substring(0, 2);
  int helper2 = helper.toInt();
  Serial.print(helper2);
  if(helper2 > 30) {
    digitalWrite(ONBOARD_LED, HIGH);
  }
  else {
    digitalWrite(ONBOARD_LED, LOW);
  }
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

void setup() {
  Serial.begin(9600);
  pinMode(ONBOARD_LED,OUTPUT);

  mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION);  // set before init() so that you can see startup messages

  mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);

  mesh.setName(nodeName); // This needs to be an unique name! 

  mesh.onReceive([](uint32_t from, String &msg) {
    //Serial.printf("Received message by id from: %u, %s\n", from, msg.c_str());
    receivedCallback(from, msg);
  });

  //mesh.onReceive([](String &from, String &msg) {
    //Serial.printf("Received message by name from: %s, %s\n", from.c_str(), msg.c_str());
  //});

  mesh.onChangedConnections([]() {
    Serial.printf("Changed connection\n");
  });

  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
}
