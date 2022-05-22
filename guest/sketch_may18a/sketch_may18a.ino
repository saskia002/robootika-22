#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SimpleKeypad.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h>
#endif

#define PIN 15
#define NUMPIXELS 9
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//keypad
const byte ROW_NUM = 4;
const byte COL_NUM = 3;
int arr[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
bool hasWon = false;

int bannedNumbers[9];

char keys[ROW_NUM][COL_NUM] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'X', 'Z', 'R'}
};
byte rowPins[ROW_NUM] = {16, 5, 4, 12};
byte colPins[COL_NUM] = {0, 2, 14};

SimpleKeypad kp1((char*)keys, rowPins, colPins, ROW_NUM, COL_NUM);

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
char reply[] = "Pakett received!";

#define udpIP "192.168.4.1"
bool pause = false;

void setup() {
  pinMode(13, LOW); 
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(20);
 
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
    turnOnLed(1, pixels.Color(255,255,255));
    delay(50);
    turnOnLed(2, pixels.Color(255,255,255));
    delay(50);
    turnOnLed(3, pixels.Color(255,255,255));
    delay(50);
    turnOnLed(4, pixels.Color(255,255,255));
    delay(50);
    turnOnLed(5, pixels.Color(255,255,255));
    delay(50);
    turnOnLed(6, pixels.Color(255,255,255));
    delay(50);
    turnOnLed(7, pixels.Color(255,255,255));
    delay(50);
    turnOnLed(8, pixels.Color(255,255,255));
    delay(50);
    turnOnLed(9, pixels.Color(255,255,255));
    delay(50);
    turnOnLed(1, pixels.Color(0,0,0));
    delay(50);
    turnOnLed(2, pixels.Color(0,0,0));
    delay(50);
    turnOnLed(3, pixels.Color(0,0,0));
    delay(50);
    turnOnLed(4, pixels.Color(0,0,0));
    delay(50);
    turnOnLed(5, pixels.Color(0,0,0));
    delay(50);
    turnOnLed(6, pixels.Color(0,0,0));
    delay(50);
    turnOnLed(7, pixels.Color(0,0,0));
    delay(50);
    turnOnLed(8, pixels.Color(0,0,0));
    delay(50);
    turnOnLed(9, pixels.Color(0,0,0));
  }

  // Connected to WiFi
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

  //for (int i = 1; i<10 ; i++){
    //turnOnLed(i, pixels.Color(0,0,0));
  //}

  for (int i = 1; i<10 ; i++){
    turnOnLed(i, pixels.Color(255,255,255));
  }

  // Begin listening to UDP port
  UDP.begin(UDP_PORT);
  Serial.print("Listening on UDP port ");
  Serial.println(UDP.remotePort());
 
}

void inBannedNumbers(char k){
  String newK = (String)k;
  int test = newK.toInt();
  int inArray = 0;
  for(int i=0; i < 9; i++){
      if (bannedNumbers[i] == test){
        Serial.print("Number is banned");
        inArray++;
      }
    }
    if(inArray <1){
      testAndSend(k);
    }
    
}

void playerInput(int keypress, int playerIndex){
  int firstIndex = floor(keypress / 3);
  int secondIndex = (keypress % 3) - 1;
  arr[firstIndex][secondIndex] = playerIndex;
  Serial.println();
  for(int i=0; i<3; i++){
    for (int j=0; j<3; j++){
      Serial.print(arr[i][j]);
    }
  }
}

void testAndSend(char key){
  UDP.beginPacket(udpIP, UDP.remotePort());
  UDP.write(key);
  UDP.endPacket();
  String newKey = (String)key;
  if(newKey == "R"){
    delay(125);
    pinMode(13, HIGH);
  }
  int newVal = newKey.toInt();
  addToBanned(newVal);
  playerInput(newVal,2);
  switch(newVal){
    case 4:
      newVal = 6;
      break;
    case 6:
      newVal = 4;
      break;
  }
  turnOnLed(newVal, pixels.Color(0, 150, 255));
  checkWin(2);
  if(hasWon){
    //Serial.println("Player 2 has won!!");
    delay(1000);
    for (int i = 1; i<10 ; i++){
    turnOnLed(i, pixels.Color(0,255,0));
    }
    delay(2000);
    pinMode(13, HIGH);
  }
  pause = false;
}

void readForPacket(){
  int packetSize = UDP.parsePacket();
  if (packetSize) {
    Serial.print("Received packet! Size: ");
    Serial.println(packetSize);
    pause = true;
    int len = UDP.read(packet, 255);
  if (len > 0)
  {
    packet[len] = '\0';
  }
  String muutuja = (String)packet;
  int key_number = muutuja.toInt();
  
  //Serial.println(muutuja);
  if(muutuja == "R"){
    delay(125);
    pinMode(13, HIGH);
  }
  
  addToBanned(key_number);
  playerInput(key_number,1);

  switch(key_number){
    case 4:
      key_number = 6;
      break;
    case 6:
      key_number = 4;
      break;
  }
 
  turnOnLed(key_number, pixels.Color(255, 255, 0));
  checkWin(1);
  if(hasWon){
    //Serial.println("Player 2 has won!!");
    delay(1000);
    for (int i = 1; i<10 ; i++){
    turnOnLed(i, pixels.Color(255,0,0));
    }
    delay(2000);
    pinMode(13, HIGH);
  }
  Serial.print("Packet received: ");                      
  Serial.print(key_number);
  Serial.print("!");
}
}

void addToBanned(int value){
  bannedNumbers[value-1] = value;
  Serial.println();
  for(int i=0; i<9; i++){
    Serial.print(bannedNumbers[i]);
  }
}

void checkWin(int symbol){
  Serial.println(symbol);
  if(arr[0][0] == symbol && arr[0][1] == symbol && arr[0][2] == symbol){
      hasWon = true;
      pause = false;
    }
  if(arr[1][0] == symbol && arr[1][1] == symbol && arr[1][2] == symbol){
      hasWon = true;
      pause = false;
    }
  if(arr[2][0] == symbol && arr[2][1] == symbol && arr[2][2] == symbol){
      hasWon = true;
      pause = false;
    }
  if(arr[0][0] == symbol && arr[1][0] == symbol && arr[2][0] == symbol){
      hasWon = true;
      pause = false;
    }
  if(arr[0][1] == symbol && arr[1][1] == symbol && arr[2][1] == symbol){
      hasWon = true;
      pause = false;
    }
  if(arr[0][2] == symbol && arr[1][2] == symbol && arr[2][2] == symbol){
      hasWon = true;
      pause = false;
    }
  if(arr[0][0] == symbol && arr[1][1] == symbol && arr[2][2] == symbol){
      hasWon = true;
      pause = false;
    }
  if(arr[2][0] == symbol && arr[1][1] == symbol && arr[0][2] == symbol){
      hasWon = true;
      pause = false;
    }
  }

void turnOnLed(int nLed, uint32_t color){ 
    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:    
    pixels.setPixelColor(nLed-1, color);
    pixels.show();   // Send the updated pixel colors to the hardware.
    delay(100); // Pause before next pass through loop 
}

void loop() {
  while (!pause){
    readForPacket();
  }
  while(pause){
    char key = kp1.getKey();
    delay(50);
    if(key == 'R'){
      testAndSend(key);
    }
    if(isDigit(key)){
      inBannedNumbers(key);
    }
  }
}
