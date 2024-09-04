#include <Arduino.h>

/********************************************************************************/
/*                      Program Memory    8 bit  b address                      */
/********************************************************************************/

byte program_mem[12] = { 0x74,
                        0x00,
                        0xA5,

                        0x44,
                        0x00,
                        0x5A,

                        0x54,
                        0x00,
                        0x50,

                        0x22,
                        0x00,
                        0x00};

/********************************************************************************/
/*                       Data Memory    8 bit  b address                        */
/********************************************************************************/

byte         data_mem[32];
unsigned int data_memory_addr;

/********************************************************************************/
/*                              8 bit  Registers                                */
/********************************************************************************/
byte Reg_A;
byte Reg_B;

byte Reg_PC;    // program counter Register
byte Reg_R0;

/********************************************************************************/
/*                            8-bit  State Machine                              */
/********************************************************************************/
byte state_current;
byte state_next;
byte opcode;
byte operand_hi;
byte operand_lo;

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
                  operand_hi=program_mem[Reg_PC+1];
                  operand_lo=program_mem[Reg_PC+2];
                  Serial.println("1) Fetch state:");
                  Serial.print("opcode    :");
                  Serial.println(opcode,HEX);
                  Serial.print("opcode_hi :");
                  Serial.println(operand_hi,HEX);
                  Serial.print("opcode_lo :");
                  Serial.println(operand_lo,HEX);
                  Serial.print("Register A :");
                  Serial.println(Reg_A,HEX);
                  state_next=0x01;
                  break;
    case  0x01  :// decode
                  Serial.println("2) Decode state");
                  switch(opcode){
                    case 0x22:    // RET;
                                    Reg_PC = 0x00;
                                    break;
                    case 0x34:    // ADD A,#data
                                    Reg_A = Reg_A+operand_lo;
                                    Serial.print("Register A :");
                                    Serial.println(Reg_A,HEX);
                                    break;
                    case 0x44:    // ORL A,#data
                                    Reg_A |= operand_lo;
                                    Serial.print("Register A :");
                                    Serial.println(Reg_A,HEX);
                                    break;
                    case 0x54:    // AND A,#data
                                    Reg_A &= operand_lo;
                                    Serial.print("Register A :");
                                    Serial.println(Reg_A,HEX);
                                    break;
                    case 0x74:    // MOV A, #data
                                    Reg_A = operand_lo;
                                    Serial.print("Register A :");
                                    Serial.println(Reg_A,HEX);
                                    break;
                    case 0x88:    // MOV data addr,R0
                                    break;
                    case 0xF8:    // MOV R0, A;
                                    data_mem[operand_lo] = Reg_A;
                                    Serial.print("Register R0 :");
                                    Serial.println(Reg_R0,HEX);
                                    break;
                    case 0xFF:    // MOV PC, #0x00;
                                    Reg_PC = 0x00;
                                    break;
                    default:
                            break;
                  }
                  state_next=0x02;
                  break;
    case  0x02  :// execute
                  Serial.println("3) Execute state");
                  if(opcode != 0x22 && opcode != 0xFF) Reg_PC = Reg_PC+3;
                  Serial.print("Register PC :");
                  Serial.println(Reg_PC,HEX);
                  state_next=0x00;
                  break;
    default:
      Serial.println("Unknown state");
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
 delay(5000);
}