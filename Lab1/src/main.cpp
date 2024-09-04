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
byte Reg_A;
byte Reg_B;

byte Reg_PC;    // program counter Register

/********************************************************************************/
/*                            8-bit  State Machine                              */
/********************************************************************************/
byte state_current;
byte state_next;

int count;
void setup(){
  Serial.begin(9600);
  count=0;
  Serial.println("Setup process");
  delay(1000);
}

void loop(){
  /************************ start state machine ********************************/
  switch(){
    case  :       // fetch
      break;
    case  :       // decode
      break;
    case  :       // execute
      break;
    default:
      break;
  }
  Serial.print("loop: ");
  Serial.println(count);
  if(count>=65535){
    count =0;
  }else{
    count++;
  }
  delay(100);
}