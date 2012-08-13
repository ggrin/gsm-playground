#include <stdarg.h>

#include <avr/pgmspace.h>

#include "gsm-stuff.h"




bool haveSwitchedParams = false;
bool haveSentSMS = false;
GSM gsm;


bool stop = false;
bool gsm_task = false;
bool sensor_task = true;


void setup() {

  Serial.begin(9600);
  Serial.println("Serial begun");

  initSensors();


  delay(3000);

  pinMode(8, OUTPUT); // ON
  pinMode(9, OUTPUT); // RESET

  gsm.TurnOn(19200);
  //gsm.TurnOn(115000);
  gsm.Echo(0);
}

void send_data(){

  int reg;
  reg=gsm.CheckRegistration();
  switch (reg){
  case REG_NOT_REGISTERED:
    Serial.println("not registered");
    break;
  case REG_REGISTERED:
    Serial.println("GSM module is registered");
    break;
  case REG_NO_RESPONSE:
    Serial.println("GSM doesn't respond");
    break;
  case REG_COMM_LINE_BUSY:
    Serial.println("comm line is not free");
    break;  }
  
  if(haveSwitchedParams) {

    gsm.DumpBuffer();

    Serial.println("Would send req");
    if(HTTP::start("internet", "-", "-")) {
      Serial.println("HTTP started");    
      HTTP::post_tempValues("http://heahdk.net/talk-to-me/bla", tempValues);
    } else { gsm.DumpBuffer(); }

  } else if(reg == REG_REGISTERED) {
    gsm.InitParam(PARAM_SET_1);
    haveSwitchedParams = true;
  }
}

void read_sensor_input() {
  initiateRead();
  readTemperatures();
}

void loop() {

  word lastTime;
  if(stop) { return; }


    stop = true;
    if( gsm_task ){
      send_data();
      gsm_task = false;
     }
    if( sensor_task ){
      lastTime = millis();
      read_sensor_input();
      sensor_task = false;
    }



    if(setCompleted()) {
      gsm_task = true;
      for(int i=0;i<60;i++) {
        Serial.print("DATASET ");
        Serial.print(i, DEC);
        Serial.println(":");
        for(int j=0;j<sensorsFound;j++) {
          Serial.print(" - ");
          Serial.println(tempValues[i][j]);
        }
      }
    }
    if( millis()-lastTime >= 6000)
      sensor_task = true;
}
