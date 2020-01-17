/*
  
  REST calls

  "/arduino/irrigation/control/on"     -> send packet for Motor on;
  "/arduino/irrigation/control/off"    -> send packet for Motor off;
*/

#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>

#define BAUDRATE 115200

// Listen to the default port 5555, the LG01 webserver
// will forward there all the HTTP requests you send
BridgeServer server;

void setup() {
  // Bridge startup
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin(BAUDRATE);
  digitalWrite(13, HIGH);

  // Listen for incoming connection only from localhost
  // (no one from the external network could connect)
  server.listenOnLocalhost();
  server.begin();
}

void loop() {
  // Get clients coming from server
  BridgeClient client = server.accept();

  // There is a new client?
  if (client) {
    // Process request
    process(client);

    // Close connection and free resources.
    client.stop();
  }

  delay(50); // Poll every 50ms
}

void process(BridgeClient client) {
  String command = client.readStringUntil('/');
  Console.println(command);
  if (command == "irrigation") {
    irrigationCommand(client);
  }
}

void irrigationCommand(BridgeClient client) {
  String command, copy_str, control, value;
  int index;
  
  command = client.parseInt();
    Console.println(command);
  index= command.indexOf("/");
  if(index != -1){
    control = command.substring(0, index);
    value = command.substring(index+1, command.length());
  }
  
  Console.println(command);
  irrigationContorl(control, value);

}

void irrigationContorl(String control, String value){
  if(control == "control" || control == "Control"){
    if(value == "on" || value == "On" || value == "ON"){
       
    } else if (value == "off" || value == "Off" || value == "OFF") {
      
    } else {
      
    }
  }
}
