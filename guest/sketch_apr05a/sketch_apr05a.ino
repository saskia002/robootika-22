#include <ESP8266WiFi.h>
#include <WiFiUdp.h>


// Set WiFi credentials
#define WIFI_SSID "trips-traps-trull vork"
#define WIFI_PASS "123456789"
#define UDP_PORT 161

// Set your Static IP address
IPAddress local_IP(192, 168, 4, 2);
// Set your Gateway IP address
IPAddress gateway(192, 168, 4, 254);

IPAddress subnet(255, 255, 255, 0);

// UDP
WiFiUDP UDP;
char packet[255];
char reply[] = "sdfdsgdzfhgfjtyja5uj received!";

#define udpIP "192.168.4.1"

void setup() {

  pinMode(13, OUTPUT);

  // Setup serial port
  Serial.begin(115200);
  Serial.println();

  WiFi.config(local_IP, gateway, subnet);

  // Begin WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);

   Serial.println(WiFi.softAPIP());

  // Connecting to WiFi...
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  // Loop continuously while WiFi is not connected
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }

  // Connected to WiFi
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

  // Begin listening to UDP port
  UDP.begin(UDP_PORT);
  Serial.print("Listening on UDP port ");
  Serial.println(UDP.remotePort());
 
}

void loop() {

  // If packet received...
  int packetSize = UDP.parsePacket();
  if (packetSize) {
    Serial.print("Received packet! Size: ");
    Serial.println(packetSize);
    int len = UDP.read(packet, 255);
    if (len > 0)
    {
      packet[len] = '\0';
      digitalWrite(13, HIGH);
      delay(1000);
      digitalWrite(13, LOW);
      delay(1000);
    }

    // Send return packet
    UDP.beginPacket(udpIP, UDP.remotePort());
    UDP.write(reply);
    UDP.endPacket();
  }


 

}
