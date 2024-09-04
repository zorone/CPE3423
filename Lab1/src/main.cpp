/********************************************************************************/
/*                             Program Memory                                   */
/********************************************************************************/

#include <Arduino.h>

byte program_mem[8] = { 0x00,
                        0x01,
                        0x02,
                        0x03,
                        0x04,
                        0x05,
                        0x06,
                        0x07};

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