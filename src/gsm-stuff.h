
#ifndef GSM_STUFF_H
#define GSM_STUFF_H

extern bool stop;

#include <GSM_Shield.h>

extern GSM gsm;

namespace HTTP {
  bool start(char *apn, char *user, char *pass);
  int get(char *url);
  int post_tempValues(char *url, float **tempValues);
}

#include <OneWire.h>

extern float *tempValues[60];
extern OneWire ow;
extern unsigned int sensorsFound;
void initSensors();
void initiateRead();
void readTemperatures();
bool setCompleted();

#endif
