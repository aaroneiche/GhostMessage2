#include <Ticker.h>

#include <PxMatrix.h>
#include <WebSocketsServer.h>

#include "WiFiConnect.h" 

#include "credentials.h"

#include <WiFiClient.h>                      
#include <ESP8266WebServer.h>

#include <ArduinoJson.h>

WiFiConnect wc;

// For internet connection
WiFiClient client;


void drawDisplayStep();

Ticker display_ticker;
Ticker message_ticker;

#include "bitmaps.h"

#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_D 12
#define P_E 0
#define P_OE 2

PxMATRIX display(64,16,P_LAT, P_OE,P_A,P_B,P_C);
WebSocketsServer webSocket = WebSocketsServer(81);
ESP8266WebServer server(80);


uint16_t RED = display.color565(255, 0, 0);
uint16_t PINK = display.color565(255, 100, 100);
uint16_t GREEN = display.color565(0, 255, 0);
uint16_t BLUE = display.color565(0, 0, 255);
uint16_t WHITE = display.color565(255, 255, 255);
uint16_t ORANGE = display.color565(255, 128, 0);
uint16_t YELLOW = display.color565(255, 255, 0);
uint16_t CYAN = display.color565(0, 255, 255);
uint16_t MAGENTA = display.color565(255, 0, 255);
uint16_t BLACK = display.color565(0, 0, 0);

bool toggle = true;
bool dir = true;
int ghostx = -14;
int ghosty = 1;
int messageWidth = 0;
int lastMessageWidth = 0;

int pacx = 32;
int pacy = 2;

unsigned long previousMillis = 0;
const long interval = 300000;
//

String lastMessage = "";
String sentMessage = "Ghost Message!";
String ghostColor = "orange";
bool newMessage = false;


const int PAC_OPEN_LEFT = 1;
const int PAC_MID_LEFT = 2;
const int PAC_CLOSED = 3;

bool playingLastMessage = false;
int lastPacState = 1;
int pacPos = 32;

uint16_t GHOSTC = ORANGE;

DynamicJsonDocument doc(1024);
const char * message[256];
const char * color[10];



void display_updater()
{
  display.display(70);
}

void display_update_enable(bool is_enable)
{
  if (is_enable)
    display_ticker.attach(0.004, display_updater);
  else
    display_ticker.detach();
}

void drawGhost(int x, int y, uint16_t color, bool direction, bool skirt){

  display.drawBitmap(x,y,ghost,14,12,color);

  if(direction){
    display.drawBitmap(x+3,y+3,eyeWhite,4,5,WHITE);
    display.drawBitmap(x+5,y+5,pupil,2,2,BLUE);

    display.drawBitmap(x+9,y+3,eyeWhite,4,5,WHITE);
    display.drawBitmap(x+11,y+5,pupil,2,2,BLUE);
  }else{
    display.drawBitmap(x+1,y+3,eyeWhite,4,5,WHITE);
    display.drawBitmap(x+1,y+5,pupil,2,2,BLUE);

    display.drawBitmap(x+7,y+3,eyeWhite,4,5,WHITE);
    display.drawBitmap(x+7,y+5,pupil,2,2,BLUE);
  }


  if(skirt){
    display.drawBitmap(x,y+12,skirtA,14,2,color);
  }else{
    display.drawBitmap(x,y+12,skirtB,14,2,color);
  }
}


void drawMessage(int x, int y, String message){ //char *message
  display.setCursor(x, y);
  display.setTextColor(WHITE);
  display.print(message);
}


void drawPacman(int x, int y, int state) {

  if(state == PAC_OPEN_LEFT) {
    display.drawBitmap(x,y,pacOpenLeft,11,12,YELLOW);
  }else if(state == PAC_MID_LEFT) {
    display.drawBitmap(x,y,pacMidLeft,12,13,YELLOW);
  }else if(state == PAC_CLOSED){
    display.drawBitmap(x,y,pacClosed,13,13,YELLOW);
  }
  
}

void drawDisplayStep() {
    
    if(newMessage) {
      messageWidth = sentMessage.length() * -6;
      display.clearDisplay();
      toggle = (toggle) ? false: true;
      drawGhost(ghostx,ghosty,GHOSTC,dir,toggle);

      if(dir == true){
        ghostx++;
      }else if(dir == false){
        ghostx--;
      }

      if(!dir){
        drawMessage(ghostx + 16, 4, sentMessage);
      }

      if(ghostx > display.width() && dir == true){
        dir = false;
      }else if(ghostx < messageWidth - 14 - display.width() && dir == false) {
        dir = true;
        ghostx = -14;
        lastMessage = sentMessage;
        newMessage = false;
      }
    }
    
    /* 
    messageWidth = sentMessage.length() * -6;
    display.fillScreen(0);
    toggle = (toggle) ? false: true;
    drawGhost(ghostx,ghosty,GHOSTC,dir,toggle);

    if(dir == true){
      ghostx++;
    }else if(dir == false){
      ghostx--;
    }

    if(!dir){
      drawMessage(ghostx + 16, 4, sentMessage);
    }

    if(ghostx > display.width() && dir == true){
      dir = false;
    }else if(ghostx < messageWidth - 14 - display.width() && dir == false) {
      dir = true;
      ghostx = -14;
      lastMessage = sentMessage;
    }
 */
}

void handleRoot() {
  server.send(200, "text/plain", "hello from esp8266!\r\n");
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            // USE_SERIAL.printf("[%u] Disconnected!\n", num);
            Serial.println('Disconnection');
            break;
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket.remoteIP(num);
                // USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
                Serial.println('Connection!');
				
				        // send message to client
				        webSocket.sendTXT(num, "Connected");
            }
            break;
        case WStype_TEXT:
            Serial.printf("[%u] get Text: %s\n", num, payload);
            deserializeJson(doc, payload);
            sentMessage = String(doc["message"]);
            ghostColor = String(doc["color"]);
            if(ghostColor == "red"){
              GHOSTC = RED;
            }else if(ghostColor == "pink"){
              GHOSTC = PINK;
            }else if(ghostColor == "orange"){
              GHOSTC = ORANGE;
            }else if(ghostColor == "cyan"){
              GHOSTC = CYAN;
            }else if(ghostColor == "black"){
              GHOSTC = BLACK;              
            }else{
              GHOSTC = GREEN;
            }
            newMessage = true;

            break;
        case WStype_BIN:
            // USE_SERIAL.printf("[%u] get binary length: %u\n", num, length);
            hexdump(payload, length);

            // send message to client
            // webSocket.sendBIN(num, payload, length);
            break;
    }

}

void configModeCallback(WiFiConnect *mWiFiConnect) {
  Serial.println("Entering Access Point");
}

void startWiFi(boolean showParams = false) {
 
  wc.setDebug(true);
  
  /* Set our callbacks */
  wc.setAPCallback(configModeCallback);

  //wc.resetSettings(); //helper to remove the stored wifi connection, comment out after first upload and re upload

    /*
       AP_NONE = Continue executing code
       AP_LOOP = Trap in a continuous loop - Device is useless
       AP_RESET = Restart the chip
       AP_WAIT  = Trap in a continuous loop with captive portal until we have a working WiFi connection
    */
    if (!wc.autoConnect(SSID,PASSWORD)) { // try to connect to wifi
      /* We could also use button etc. to trigger the portal on demand within main loop */
      wc.startConfigurationPortal(AP_WAIT);//if not connected show the configuration portal
    }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Welcome to GhostMessage!");

  //Setup captured portal - so we can connect. 
  startWiFi();

  server.on("/", handleRoot);


  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  display.begin(8);

  display_update_enable(true);
  display.setBrightness(32);
  display.setTextWrap(false);
  
  // display.begin();
  
  // display.setTextSize(1);
  // delay(5000);
  // display.setTextColor(WHITE);
  // display.setCursor(0,0);

/* 
  drawPacman(pacPos,2,PAC_OPEN_LEFT);

  for(int i = 0; i <= 3; i++) {
    display.fillScreen(0);
    display.setCursor(0,0);
    display.print("Ghost");
    display.setCursor(0,9);
    display.print("Message");
    drawPacman(pacPos,2,i);
    delay(65);
    if(i == 3){
      i = 0;
    }
    if(pacPos < -12){
      break;
    }
    pacPos--;
  } 
*/

  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  message_ticker.attach_ms(70, drawDisplayStep);

  Serial.println("Setup complete.");
}


void loop() {
  webSocket.loop();
  server.handleClient();
}
