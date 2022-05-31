#include <Arduino.h>
#include "SummedPacket.h"

SummedPacket referencePack(4);
uint8_t id = 254;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(5, OUTPUT);
}

void loop() {
  if(Serial.available()){
    if(referencePack.FetchByte(Serial)){
      switch(referencePack[0]){
        case 0:
          id = referencePack[1];
          ++referencePack[1];
        case 1:
          if(id == referencePack[1] || referencePack[1] == 255){
            if(referencePack[2] == 0)
              digitalWrite(5, LOW);
            else if(referencePack[2] == 255)
              digitalWrite(5, HIGH);
            else
              analogWrite(5, referencePack[2]);
            
            ++referencePack[3];            
          }
      }

      referencePack.WriteToStream(Serial);
    }
  }
}