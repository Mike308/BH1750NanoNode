#include <Arduino.h>
#include <Wire.h>
#include <BH1750.h>
#include <RF24Network.h>

BH1750 sensor;
uint64_t pipeAddress = 0xF0F0F0F0F101;
uint64_t receivingPipeAddress = 0xF0F0F0F0E101;
unsigned long currentMillis;
unsigned long previousMillis;
uint8_t rxFlag = 0;
char rxBuff[32];
char txBuff[96];
RF24Network network(9, 10, "|", "2");


void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  sensor.begin();
  Serial.begin(9600);
  network.begin();
  network.setRetries(15, 15);
  network.openWritingPipe(pipeAddress);
  network.openReadingPipe(1, receivingPipeAddress);
  network.setAutoAck(true);
  network.startListening();
  sensor.begin();
  Serial.println("BH1750 Module");
}

void loop() {
  // put your main code here, to run repeatedly:
    if (network.receiveData(rxBuff)) {
      Serial.print("+RX=");
      Serial.println(rxBuff);
      char *parsed = strtok(rxBuff, "?");
      if (!strcmp("AT+LX", parsed)) {
         rxFlag = 1;
         char lightLevel[8];
         dtostrf(sensor.readLightLevel(), 4, 1, lightLevel);
         sprintf(txBuff, "+LX=%s;GY302", lightLevel);
         network.buildFrame(txBuff); 
         Serial.print("+TX=");
         Serial.println(txBuff);
         rxFlag = 0;
      }
        currentMillis = millis();
  }
}