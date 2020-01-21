#ifndef RF24_NETWORK_H
#define RF24_NETWORK_H
#include <RF24.h>
#define BUFF_SIZE 32
#define DEBUG 1
class RF24Network : public RF24 
{
private:
    char *nodeAddress;
    char rxBuff[BUFF_SIZE];
    char  *separator;
    void lowLevelWriteData(char *data);
public:
    RF24Network(uint16_t cePin, uint16_t csPin) : RF24(cePin, csPin) {}
    RF24Network(uint16_t cePin, uint16_t csPin, char *separator, char *nodeAddress);
    void setNodeAddress(char *nodeAddress);
    void buildFrame(char *data);
    bool receiveData(char *data);
};

#endif


