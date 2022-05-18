#include <Arduino.h>

uint8_t messageBuffer[8];
byte bufferOffset = 0;
uint8_t moduleId = 0;

// #DEFINE _SINGLEMODE

void RunMessage(){
  switch(messageBuffer[2]){
    case 1:
        moduleId = messageBuffer[3];
        messageBuffer[3]++;
      break;
    case 2:
      if(moduleId == messageBuffer[3]
      ||
      messageBuffer[3] == 0){
          if(messageBuffer[4] == 0){
            digitalWrite(DD5, LOW);
          }
          else if(messageBuffer[4] == 255){
            digitalWrite(DD5, HIGH);
          }
          else
            analogWrite(DD5, messageBuffer[4]);
      }
      break;
  }
  Serial.write(messageBuffer, 8);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(DD5, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(DD5, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(DD5, LOW);

}

void loop() {
  // put your main code here, to run repeatedly:

  if(Serial.available() >= 8)
  {
    Serial.readBytes(messageBuffer, 8);
    uint8_t activeCs = 0;
    for(int i = 0; i < 6; ++i){
      activeCs += messageBuffer[i];
    }
  #if _SINGLEMODE
    Serial.println("Message recieved");
  #endif
    if(messageBuffer[7] == activeCs){
#if _SINGLEMODE
      Serial.println("Message ok");
#endif
      RunMessage();
    }
    else{
 #if _SINGLEMODE
      Serial.print("Message fail: expected ");
      Serial.print(activeCs);
      Serial.print(" but got ");
      Serial.println(messageBuffer[7]);
#endif
      bufferOffset = 0;
    }
  }
}