
#include <SPI.h>
#include <Wire.h>
#include <DisplayController.h>
#include <WifiApServer.h>

extern "C" {
  #include <user_interface.h>
}

//global variables
String BSSID = "asdf";
String Networks[][1] = {
  {"NETWORK_SSID","PSK"}
};


DisplayController *displayController;
WifiApServer *server;
void OnEvent(System_Event_t*se);

void Display(String s, bool persist);

void setup() {
  Serial.begin(115200);
  Serial.println("");
  
  Serial.println("InitDisplay()");
  InitDisplay();
  Serial.println("InitDisplay() complete");
  
  InitServer();
  
  displayController->ChangeInterval = 3000;
}

void loop() {
  displayController->Next();
  delay(10);
  server->HandleClient();
}

void Display(String s, bool persist = false) {
  Serial.println(s);
  displayController->Add(s,persist);
}

void InitServer() {
  Display("InitServer()");
  server = new WifiApServer();
  
  //register callback events
  Display("registering callback events");
  wifi_set_event_handler_cb(OnEvent);
  
  server->OnConnect = OnConnect;
  server->OnDisconnect = OnDisconnect;
  server->OnClientConnect = OnClientConnect;
  server->OnClientDisconnect = OnClientDisconnect;
  server->OnIpAssigned = OnIpAssigned;

  // connect to network
  Display("connecting to network");
  int i=0;
  int iMax = sizeof Networks / sizeof Networks[0];
  bool connected = false;
  String ssid,psk;
  while(!connected) {
    ssid= Networks[i][0];
    psk= Networks[i][1];
    Display("connecting to \"" + ssid + "\" with psk \"" + psk + "\"");
    connected = server->Connect(ssid,psk,BSSID);
    if(!connected) {
      Display("Failed to connect to " + ssid);
      if(++i >= iMax) {
        i=0;
      }
    }
  }
  Display("connecting to network complete");
  Display("Connected to \"" + ssid + "\" with psk \"" + psk + "\"", true);


  //register paths
  Display("registering custom paths");
  server->RegisterPath("/test", []() {
    server->Send ( 200, "text/plain", "you found me "+ String(millis()));
  });
  Display("registering custom paths complete");
  
  Display("InitServer() complete");
}

void InitDisplay() {
  displayController = new DisplayController();
}

void OnEvent(System_Event_t*se) {
  server->OnEvent(se);
}

void OnConnect() { 
  Display("We are Connected"); 
};
void OnDisconnect() { 
  Display("We have Disconnected"); 
};
void OnClientConnect() { 
  Display("Client Connected"); 
};
void OnClientDisconnect() { 
  Display("Client Disconnected"); 
};
void OnIpAssigned(IPAddress ip){ 
  Display("Local IP Address: " + ipToString(ip), true) ;
};

