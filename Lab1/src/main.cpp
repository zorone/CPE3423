#include <Arduino.h>

/********************************************************************************/
/*                      Program Memory    8 bit  b address                      */
/********************************************************************************/

byte program_mem[8] = { 0x0A,
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
byte opcode;

int count;
void setup(){
  Serial.begin(9600);
  count=0;
  Serial.println("Setup process");
  state_current = 0x00;
  state_next    = 0x00;
  Reg_PC        = 0x00;
}

void loop(){
  /************************ start state machine ********************************/
  switch(state_current){
    case  0x00  :// fetch
                  opcode=program_mem[Reg_PC];
                  Serial.print("Fetch state   :");
                  Serial.println(opcode,HEX);
                  state_next=0x01;
                  break;
    case  0x01  :// decode
                  Serial.println("Decode state");
                  state_next=0x02;
                  break;
    case  0x02  :// execute
                  Serial.println("Execute state");
                  state_next=0x00;
                  break;
    default:
      Serial.println("Unnone state");
      state_next=0x00;
      break;
  }
  state_current = state_next;
  /************************* end state machine *********************************/

  /**
    Serial.print("loop: ");
    Serial.println(count);
    if(count>=65535){
      count =0;
    }else{
      count++;
    }
    delay(100);
  */
}