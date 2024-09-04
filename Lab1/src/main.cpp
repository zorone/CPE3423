#include <Arduino.h>

/********************************************************************************/
/*                      Program Memory    8 bit  b address                      */
/********************************************************************************/

byte program_mem[8] = { 0xFF,
                        0xFF,
                        0xFF,
                        0xFF,
                        0xFF,
                        0xFF,
                        0xFF,
                        0xFF};

/********************************************************************************/
/*                       Data Memory    8 bit  b address                        */
/********************************************************************************/

byte data_mem[32];

/********************************************************************************/
/*                              8 bit  Registers                                */
/********************************************************************************/
byte Register_A;
byte Register_B;

byte Register_Program_counter;

int count;
void setup(){
  Serial.begin(9600);
  count=0;
  Serial.println("Setup process");
  delay(1000);
}

void loop(){
  Serial.print("loop: ");
  Serial.println(count);
  if(count>=65535){
    count =0;
  }else{
    count++;
  }
  delay(100);
}