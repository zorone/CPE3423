#include <Arduino.h>

/********************************************************************************/
/*                      Program Memory    8 bit  b address                      */
/********************************************************************************/

// From MCS-51 document, you has to explicitly specify data memory address in program memory.
// Unfortunately, you can't access data memory using another register.
// We will instead opt-in for `MOV addr, R0` and `MOV R0, addr` for accessing them.

byte program_mem[] = { 0x74,
                       0x00,
                       0x20,
                       0xF5,
                       0x00,
                       0x00,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x01,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x02,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x03,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x04,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x05,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x06,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x07,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x08,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x09,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x0A,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x0B,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x0C,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x0D,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x0E,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x0F,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x10,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x11,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x12,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x13,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x14,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x15,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x16,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x17,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x18,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x19,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x1A,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x1B,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x1C,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x1D,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x1E,
                       0x14,
                       0x00,
                       0x00,
                       0xF5,
                       0x00,
                       0x1F,
                       0x14,
                       0x00,
                       0x00,
                       0x22,
                       0x00,
                       0x00 // [294]
};

/********************************************************************************/
/*                       Data Memory    8 bit  b address                        */
/********************************************************************************/

byte         data_memory[32];
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
                        state_next=0x01;
                        break;
        case  0x01  :// decode
                      Serial.println("opcode: " + String(program_mem[Reg_PC]) + ", PC: " + Reg_PC);

                      data_memory_addr=operand_hi<<8;
                      data_memory_addr=data_memory_addr | operand_lo;
                      switch(opcode){
                        case 0x14:    // DEC A;
                                        Reg_A--;
                                        Serial.println("Reg_A: " + Reg_A);
                                        break;
                        case 0x22:    // RET;
                                        Reg_PC = 0x00;
                                        Serial.println("Reg_PC: " + Reg_PC);
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
                                        data_memory[data_memory_addr] = Reg_R0;
                                        Serial.print("Data memory[");
                                        Serial.print(data_memory_addr,HEX);
                                        Serial.print("]:");
                                        Serial.println(data_memory[data_memory_addr],HEX);
                                        break;
                        case 0xA8:    // MOV R0, data addr
                                        Reg_R0 = data_memory[data_memory_addr];
                                        Serial.print("Data memory[");
                                        Serial.print(data_memory_addr,HEX);
                                        Serial.print("]:");
                                        Serial.println(data_memory[data_memory_addr],HEX);
                                        break;
                        case 0xF5:    // MOV data addr, A;
                                        data_memory[data_memory_addr] = Reg_A;
                                        Serial.print("Data memory[");
                                        Serial.print(data_memory_addr,HEX);
                                        Serial.print("]:");
                                        Serial.println(Reg_A,HEX);
                                        break;
                        case 0xF8:    // MOV R0, A;
                                        Reg_R0 = Reg_A;
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
        case 0x02  :// execute
                      if(opcode != 0x22 && opcode != 0xFF) Reg_PC = Reg_PC+3;
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
    delay(100);
}