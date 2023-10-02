//DEFINITIONS
#include <Arduino.h>
#include <RF24.h>
RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00001"; //channel (must be the same like from the receiver)

//connections: joystick <-> pin 
#define PR_ANA_PIN 6;   //joystick for propeller ist connected to analog pin 6
#define AL_ANA_PIN 5;   //joystick for aileron ist connected to analog pin 5
#define EL_ANA_PIN 4;   //joystick for elevator ist connected to analog pin 4
#define RD_ANA_PIN 3;   //joystick for rudder ist connected to analog pin 3

struct Send_Data{
  short propeller;
  short rudder;
  short elevator;
  short aileron;
};
Send_Data data;

//SETUP
bool firstrun;
void setup() {
  Serial.begin(9600);

  //+++ NRF24 config +++//
  radio.begin();
  radio.openWritingPipe(address);     //select channel
  radio.setPALevel(RF24_PA_MAX);      //set state of power
  radio.stopListening();              //define NRF24-L01 as transmitter

  firstrun = true;
  delay(2000);
  Serial.println("Arduino online");
  Serial.println("-------------------------");
}

//MAIN
void loop() { 
  //read init states
  short joystickInitValues[7];   
  byte anaE;             
  
  //write default init-values from the joysticks in arraybuffer 
  if(firstrun==true) {
    anaE=3;
    Serial.println("====[firstrun active]====");
    for (byte i = 3; i < 7; i++){
      joystickInitValues[i] = analogRead(anaE);
      anaE++;
    }
  }

  //propeller: 0%=joystick down | 50%=joystick centered | 100%=joystick up 
  //rudder and aileron: positive values= joystick left | nnegative values= joystick right
  //elevator: posotove value=joystick down | negative values= joystick up
  data.propeller = ((analogRead(PR_ANA_PIN) - joystickInitValues[6])*(-1));

  data.rudder = ((analogRead(RD_ANA_PIN)) - joystickInitValues[3]);
                                            
  data.elevator = ((analogRead(EL_ANA_PIN)) - joystickInitValues[4]);                                 
  
  data.aileron = ((analogRead(AL_ANA_PIN)) - joystickInitValues[5]);

  radio.write(&data, sizeof(Send_Data)); 
} 