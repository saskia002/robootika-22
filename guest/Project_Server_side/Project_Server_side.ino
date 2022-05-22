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

const byte ROW_NUM = 4;
const byte COL_NUM = 3;

String playerSelf[9];
String playerOther[9];
int arr[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
int arrTest[3][3] = {{1, 1, 1}, {0, 0, 0}, {0, 0, 0}};
bool hasWon = false;

int bannedNumbers[9];

char keys[ROW_NUM][COL_NUM] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'X', 'Z', 'R'} 
};
byte rowPins[ROW_NUM] = {16, 5, 4,12};
byte colPins[COL_NUM] = {0, 2, 14};

SimpleKeypad kp1((char*)keys, rowPins, colPins, ROW_NUM, COL_NUM);

IPAddress local_IP(192, 168, 4, 1);
// Set your Gateway IP address
IPAddress gateway(192, 168, 4, 254);

IPAddress subnet(255, 255, 255, 0);

extern "C" {
  #include<user_interface.h>
}
#define UDP_PORT 4210
#define udpIP "192.168.4.2"
WiFiUDP UDP;

const char *ssid = "trips-traps-trull vork";
const char *pass = "123456789";
char packet[255];
char reply[]="uugabuuga received!"; 
bool pause = false;



void setup() {
  pinMode(13, LOW);
  
  WiFi.config(local_IP, gateway, subnet);
  delay(100);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  
  WiFi.softAP(ssid,pass);
  
  Serial.print("AP IP address: ");
  Serial.println(WiFi.localIP());
  UDP.begin(UDP_PORT);
  Serial.print("Listening on UDP port ");
  Serial.println(UDP.remotePort());


  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(40);

  for (int i = 1; i<10 ; i++){
    turnOnLed(i, pixels.Color(255,255,255));
  }
}

void testAndSend(char key){
  UDP.beginPacket(udpIP, 161);
  UDP.write(key);
  UDP.endPacket();
  String newKey = (String)key;
  if(newKey == "R"){
    delay(125);
    pinMode(13, HIGH);
  }
  int newVal = newKey.toInt();
  addToBanned(newVal);
  playerInput(newVal,1);
  checkWin(1);
  
  pause = true;
  switch(newVal){
    case 4:
      newVal = 6;
      break;
    case 6:
      newVal = 4;
      break;
  }
  turnOnLed(newVal, pixels.Color(255, 255, 0));
  if(hasWon){
    Serial.println("Player 1 has won!!");
    delay(1000);
    for (int i = 1; i<10 ; i++){
    turnOnLed(i, pixels.Color(0,255,0));
    
  }
    delay(2000);
    pinMode(13, HIGH);
  }
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


void addToBanned(int value){
  bannedNumbers[value-1] = value;
  Serial.println();
  
  for(int i=0; i<9; i++){
    Serial.print(bannedNumbers[i]);
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

void turnOnLed(int nLed, uint32_t color){
  
    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    
    pixels.setPixelColor(nLed-1, color);

    pixels.show();   // Send the updated pixel colors to the hardware.

    delay(100); // Pause before next pass through loop
  
}

void readForPacket(){
  int packetSize = UDP.parsePacket();
  if (packetSize) {
    Serial.print("Received packet! Size: ");
    Serial.println(packetSize);
    pause = false;
    int len = UDP.read(packet, 255);
  if (len > 0)
  {
    packet[len] = '\0';
  }
  String muutuja = (String)packet;
  if(muutuja == "7"){
    Serial.println("Saine seitsme");
  }
  int key_number = muutuja.toInt();
  if (muutuja == "R"){
    delay(125);
    pinMode(13, HIGH);
  }
  addToBanned(key_number);
  playerInput(key_number,2);
  checkWin(2);
  
  Serial.print("Packet received: ");
  Serial.print(key_number);
    switch(key_number){
    case 4:
      key_number = 6;
      break;
    case 6:
      key_number = 4;
      break;
  }
  turnOnLed(key_number, pixels.Color(0, 150, 255));
  if(hasWon){
    Serial.println("Player 2 has won!!");
    delay(1000);
    for (int i = 1; i<10 ; i++){
    turnOnLed(i, pixels.Color(255,0,0));
    
  }
    delay(2000);
    pinMode(13, HIGH);
  }
  Serial.print("!");
  Serial.println(len);
}
}

void checkWin(int symbol){
  Serial.println(symbol);
  if(arr[0][0] == symbol && arr[0][1] == symbol && arr[0][2] == symbol){
      hasWon = true;
      pause = true;
    }
  if(arr[1][0] == symbol && arr[1][1] == symbol && arr[1][2] == symbol){
      hasWon = true;
      pause = true;
    }
  if(arr[2][0] == symbol && arr[2][1] == symbol && arr[2][2] == symbol){
      hasWon = true;
      pause = true;
    }
  if(arr[0][0] == symbol && arr[1][0] == symbol && arr[2][0] == symbol){
      hasWon = true;
      pause = true;
    }
  if(arr[0][1] == symbol && arr[1][1] == symbol && arr[2][1] == symbol){
      hasWon = true;
      pause = true;
    }
  if(arr[0][2] == symbol && arr[1][2] == symbol && arr[2][2] == symbol){
      hasWon = true;
      pause = true;
    }
  if(arr[0][0] == symbol && arr[1][1] == symbol && arr[2][2] == symbol){
      hasWon = true;
      pause = true;
    }
  if(arr[2][0] == symbol && arr[1][1] == symbol && arr[0][2] == symbol){
      hasWon = true;
      pause = true;
    }
  }


void loop() {
  
  while (pause == true){
    readForPacket();
  }
  while(pause == false){
    delay(50);
    char key = kp1.getKey();
    if(key == 'R'){
      //Serial.println(key);
      testAndSend(key);
    }
    if(isDigit(key)){
      Serial.println(key);
      inBannedNumbers(key);
    }
       
  }
}


  
