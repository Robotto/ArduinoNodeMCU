/*
 *  This sketch sends data via HTTP GET requests to users-phys.au.dk .
 *  and gets weather data for the Stenomuseum from /vejrdata/vejret/WD/Stenomuseet.htm
 *  the windchill is parsed to a float from the data using hacky string magic.
 */

#include <ESP8266WiFi.h>

const char* ssid     = "robottoap";
const char* password = "dobbelthamster";

const char* host = "users-phys.au.dk";

void setup() {
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//TO DO: reconnect on losing wifi

int value = 0;

void loop() {
  delay(5000);
  int linecounter=0;
  String vindchillString;
  float vindchill;
  String line="";


  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  // We now create a URI for the request
  String url = "/vejrdata/vejret/WD/Stenomuseet.htm";

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(10);

  int waitTime=0;
  while(!client.available()) { //wait for reply... 
    delay(100);
    Serial.println(".");
    waitTime++; 
    if(waitTime>50) { //more than five seconds have passed without http response
              Serial.println("timeout... resetting..");
              ESP.restart();
              while(1);
      }
    } 

  // Read all the lines of the reply from server because that's how to do it.. right? .. TO DO: check for a flush() method.
  while(client.available()){
    line = client.readStringUntil('\r');
    linecounter++;
    if(linecounter==31) vindchillString=line.substring(53,58); //get the line with the vindchill string
    delay(10);
  }

  if(line!="")
  {
  vindchill=vindchillString.toFloat(); //to float!
  Serial.println("String: " + String(vindchillString));
  Serial.println("float: " + String(vindchill));
  }

  Serial.println();
  Serial.println("closing connection");
}
