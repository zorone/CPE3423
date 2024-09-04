#include <Arduino.h>

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