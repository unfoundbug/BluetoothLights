#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h> // Hardware-specific library
#include "BluetoothSerial.h"
#include <ArduinoJson.h>
#include "SummedPacket.h"
#define TFT_MOSI 19
#define TFT_SCLK 18
#define TFT_CS 5
#define TFT_DC 16
#define TFT_RST 23
#define TFT_BL 4

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
BluetoothSerial SerialBT;

const int allowCount = 1;
const int devLen = 6;
char drawBuffer[32];

volatile bool screenLock = false;

StaticJsonDocument<200> doc;

SummedPacket referencePack(4);

uint8_t AllowedDevices[allowCount][devLen] = {
  {0x64, 0x03, 0x7f, 0x92, 0xa7, 0x46}
};

void DisplayLine(int line, const char * kpcFormat, ...){
  while(screenLock);
  screenLock = true;
  tft.setCursor(0, line*10);
  tft.fillRect(0, (10*line) , 240, 10, ST77XX_BLACK);
  char buffer[256];
  va_list args;
  va_start (args, kpcFormat);
  vsprintf (drawBuffer,kpcFormat, args);
  va_end (args);
  tft.print(drawBuffer);
  screenLock = false;
}

void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);

void setup()   {
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, 33, 32);

  /*
  while (!Serial)  delay(10);  // Wait until serial console is opened
  */
  tft.init(135, 240); 
  Serial.println("TFT initialized");

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  tft.setTextWrap(false);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);
  DisplayLine(0,  "UnfoundBug Lights");

  DisplayLine(9, "Display init");
  SerialBT.begin("UnFoundBug.Lights" ); //Bluetooth device name
  SerialBT.register_callback(callback);
  SerialBT.setTimeout(500);
  DisplayLine(8, "BT Initialised");
}

void loop() {
  DisplayLine(1,  "Cur Step: %d", micros());
  String newMessage = SerialBT.readStringUntil('^');
  if(newMessage.length() > 0){
    Serial.printf("Recieved: %s\n", newMessage.c_str());
    DeserializationError error = deserializeJson(doc, newMessage.c_str());
    if(!error){
      DisplayLine(2, "%d: %s", millis(), newMessage.c_str());
      uint8_t light = doc["light"].as<uint8_t>();
      uint8_t level = doc["level"].as<uint8_t>();
      DisplayLine(4, "%d: Light %02X", millis(), light);
      DisplayLine(5, "%d: Level %02X", millis(), level);

      referencePack[0] = light;
      referencePack[1] = level;

      int len = referencePack.GetSendSize();
    }
  }
}
	
void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
  if (event == ESP_SPP_SRV_OPEN_EVT) {
    Serial.printf("BT attempt - %02X:%02X:%02X:%02X:%02X:%02X\n"
    , param->srv_open.rem_bda[0]
    , param->srv_open.rem_bda[1]
    , param->srv_open.rem_bda[2]
    , param->srv_open.rem_bda[3]
    , param->srv_open.rem_bda[4]
    , param->srv_open.rem_bda[5]);

    bool devOk = false;

    for(int i = 0; i < allowCount; ++i){
      for(int j = 0; j < devLen; ++j){
        if(AllowedDevices[i][j] != param->srv_open.rem_bda[j]){
          break;
        }
      }

      Serial.printf("BT matched %i\n", i);
      devOk = true;
    }

    if(devOk){
      DisplayLine(8, "BT open - %02X:%02X:%02X:%02X:%02X:%02X"
        , param->srv_open.rem_bda[0]
        , param->srv_open.rem_bda[1]
        , param->srv_open.rem_bda[2]
        , param->srv_open.rem_bda[3]
        , param->srv_open.rem_bda[4]
        , param->srv_open.rem_bda[5]);
    }
    else{
      SerialBT.disconnect();
    }

    
  }
  else if (event == ESP_SPP_CLOSE_EVT){
    DisplayLine(8, "BT connection closed");    
  }
}