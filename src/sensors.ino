#include "gsm-stuff.h"

#define OW_PIN 7

OneWire ow(OW_PIN);

byte sensorAddresses[4][8];
unsigned int sensorsFound = 0;

float *tempValues[60];
unsigned int tempCounter = 0;

char* addressToString(byte addr[8]) {
  static char s[19];
  snprintf(s, 19, "0x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x", addr[0], addr[1], addr[2],
           addr[3], addr[4], addr[5], addr[6], addr[7]);
  return s;
}

void initSensors() {
  findSensors();
  allocSensorBuffer();
}

void findSensors() {
  byte addr[8];
  sensorsFound = 0;
  while(ow.search(addr)) {

    if(OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC check failed!");
      continue;
    }

    if(addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.println("Device not recognized!");
      continue;
    }

    Serial.print("Found device: ");
    Serial.println(addressToString(addr));
    memcpy(sensorAddresses[sensorsFound], addr, 8);
    sensorsFound++;
  }

  Serial.println("No more devices.");

  ow.reset_search();
}

void mallocFailed() {
  Serial.println("No memory, dude!");
  stop = true;
}

void allocSensorBuffer(){
   for(int i=0;i<60;i++) {
     if(! (tempValues[i] = (float*) malloc(sensorsFound * sizeof(float)))) {
       mallocFailed();
       return;
     }
   }

}

void initiateRead() {
  ow.reset(); // reset all devices
  ow.write(0xCC); // SKIP ROM command (~ broadcast)
  ow.write(0x44, 1); // CONVERT T command, keep wire pulled HIGH
  delay(750); // wait for CONVERT T to finish
}


float readTemperature(byte address[8]) {
  int i;
  byte data[8];
  byte checksum;

  ow.reset();

  ow.select(address); // select device
  ow.write(0xBE); // READ SCRATCHPAD command

  for(i=0;i<8;i++) {
    data[i] = ow.read();
  }

  checksum = ow.read(); // 9th byte is checksum

  if(ow.crc8(data, 8) == checksum) {
    return ((data[1] << 8) + data[0]) * 0.0625;
  } else {
    return -1; // CRC mismatch
  }
}

void readTemperatures() {
  int i;
  float temps[sensorsFound - 1];
  for(i=0;i<sensorsFound;i++) {
    temps[i] = readTemperature(sensorAddresses[i]);
    /* Serial.print("Sensor #"); */
    /* Serial.print(i); */
    /* Serial.print(": "); */
    /* if(temp == -1) { */
    /*   Serial.println("CRC mismatch"); */
    /* } else { */
    /*   Serial.println(temp); */
    /* } */
  }
  tempValues[tempCounter++] = temps;
}

bool setCompleted() {
  if (tempCounter < 60) {
    return false;
  } else {
    tempCounter = 0;
    return true;
  }
}
