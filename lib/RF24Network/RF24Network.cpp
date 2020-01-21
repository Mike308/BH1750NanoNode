
#include "RF24Network.h"

RF24Network::RF24Network(uint16_t cePin, uint16_t csPin, char *separator,
                         char *nodeAddress)
    : RF24(cePin, csPin) {
  this->nodeAddress = nodeAddress;
  this->separator = separator;
}

void RF24Network::lowLevelWriteData(char *data) {
  char txBuffor[32];
  uint8_t length = strlen(data);
  uint8_t iterations = ceil((strlen(data)) / 32.00);
  uint8_t i = 0;
  bool result = false;
  this->stopListening();
  if (iterations == 1) {
    this->write(data, length);
  } else if (iterations == 2) {
    for (uint8_t j = 0; j < 31; j++) {
      txBuffor[j] = data[j];
    }

    result = this->write(&txBuffor, 31);
    memset(txBuffor, 0, 32);
#if DEBUG == 1
    Serial.print("I frame: ");
    Serial.println(result);
#endif
    for (uint8_t j = 31; j < length; j++) {
      txBuffor[i] = data[j];
      Serial.print(txBuffor[i]);
      i++;
      if (i > length - 32) {
        i = 0;
      }
    }
    result = this->write(&txBuffor, 31);
    memset(txBuffor, 0, 32);
#if DEBUG == 1
    Serial.print("II frame: ");
    Serial.println(result);
#endif
  } else if (iterations == 3) {
    i = 0;
    for (uint8_t j = 0; j < 31; j++) {
      txBuffor[j] = data[j];
    }
    result = this->write(&txBuffor, 31);
    memset(txBuffor, 0, 32);
#if DEBUG == 1
    Serial.print("I frame: ");
    Serial.println(result);
#endif
    i = 0;
    for (uint8_t j = 31; j < 62; j++) {
      txBuffor[i] = data[j];
      i++;
    }
    result = this->write(&txBuffor, 31);
    i = 0;
    memset(txBuffor, 0, 32);
#if DEBUG == 1
    Serial.print("II frame: ");
    Serial.println(result);
#endif
    for (uint8_t j = 62; j < length; j++) {
      txBuffor[i] = data[j];
      i++;
    }
    this->write(&txBuffor, 31);
    memset(txBuffor, 0, 32);
#if DEBUG == 1
    Serial.print("III frame: ");
    Serial.println(result);
#endif
  }
  this->startListening();
}

void RF24Network::buildFrame(char *data) {
  char frameBuffor[96];
  sprintf(frameBuffor, "%s%s%s$", nodeAddress, separator, data);
  this->lowLevelWriteData(frameBuffor);
}

bool RF24Network::receiveData(char *data) {
  if (this->available()) {
    char msg[BUFF_SIZE];
    uint8_t len = 0;
    len = this->getDynamicPayloadSize();
    this->read(&msg, len);

#if DEBUG == 1
    Serial.print("RX: ");
    Serial.print("[");
    Serial.print(separator);
    Serial.print(nodeAddress);
    Serial.print("]");
    Serial.println(msg);
#endif

    char *parsed = strtok(msg, separator);

#if DEBUG == 1
    Serial.print("Parsed first element: ");
    Serial.println(parsed);
#endif

    if (!strcmp(parsed, nodeAddress)) {
      char *parsedData = strtok(NULL, separator);
      uint8_t i = 0;
      for (i = 0; i < strlen(parsedData); i++) {
          data[i] = parsedData[i];
      }

#if DEBUG == 1
      Serial.print("RX Data for node: ");
      Serial.println(data);
#endif

      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}
