#include <Arduino.h>

/********************************************************************************/
/*                      Program Memory    8 bit  b address                      */
/********************************************************************************/

byte program_mem[30] = { 0x74,
                        0x00,
                        0x0A,
                        0xF8,
                        0x00,
                        0x00,
                        0x88,
                        0x00,
                        0x05,
                        0xA8,
                        0x00,
                        0x00,
                        0xA8,
                        0x00,
                        0x01,
                        0xA8,
                        0x00,
                        0x02,
                        0xA8,
                        0x00,
                        0x03,
                        0xA8,
                        0x00,
                        0x04,
                        0xA8,
                        0x00,
                        0x05,
                        0xFF,
                        0x00,
                        0x00};
byte prog_mem_add[9] =  { 0x74,
                        0x00,
                        0x0A,
                        0x34,
                        0x00,
                        0x01,
                        0xFF,
                        0x00,
                        0x00};

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

/********************************************************************************/
/*                                   signal                                     */
/********************************************************************************/
int signal_mux_sel;
int signal_add_sel;
int signal_sub_sel;
int signal_reg_file_sel;
int command;

/********************************************************************************/
/*                               data path mux                                  */
/********************************************************************************/
int data_mux_input1;
int data_mux_input2;
int data_mux_input3;
int data_mux_output1;

/********************************************************************************/
/*                          data path register file                             */
/********************************************************************************/
int data_reg_file_input1;
int data_reg_file_output1;
int data_reg_file_output2;
int data_reg_file_output3;
int data_reg_file_output4;

/********************************************************************************/
/*                          data path register file                             */
/********************************************************************************/
int data_add_output1;
int data_add_input1;
int data_add_input2;
int data_sub_output1;
int data_sub_input1;
int data_sub_input2;

int count;

void mux(int input1, int input2, int input3, int output1, int sel ) {
  switch (sel){
    case 0 :  output1 = input1;
              break;
    case 1 :  output1 = input2;
              break;
    case 2 :  output1 = input3;
              break;
    default:  output1 = input1;
              break;
    }
    Serial.printf("data_mux_input1: %d", input1);
    Serial.printf(", data_mux_input2: %d", input2);
    Serial.printf(", data_mux_input3: %d", input3);
    Serial.printf(", data_mux_output1: %d", output1);
    Serial.printf(", data_mux_sel: %d\n", sel);
}

void add(int input1, int input2, int output1, int sel){
    if (sel==1){
        output1=input1+input2;
        Serial.print("Register A :");
        Serial.println(Reg_A,HEX);
    }
    Serial.printf("data_add_input1: %d", input1);
    Serial.printf(", data_add_input2: %d", input2);
    Serial.printf(", data_add_output1: %d", output1);
    Serial.printf(", data_add_sel: %d\n", sel);
}

void subs(int input1, int input2, int output1, int sel){
    if (sel==1){
        output1=input1-input2;
        Serial.print("Register A :");
        Serial.println(Reg_A,HEX);
    }
    Serial.printf("data_sub_input1: %d", input1);
    Serial.printf(", data_sub_input2: %d", input2);
    Serial.printf(", data_sub_output1: %d", output1);
    Serial.printf(", data_sub_sel: %d\n", sel);
}

void reg_file(int input1, int output1, int output2, int output3, int output4, int sel){
    switch(sel){
      case 0:   output1 = 0;
                output2 = 0;
                output3 = 0;
                output4 = 0;
                Reg_A   = input1;
          break;
      case 1:   output1 = input1;
                output2 = Reg_A;
                output3 = 0;
                output4 = 0;
          break;
      case 2:   output1 = 0;
                output2 = 0;
                output3 = input1;
                output4 = Reg_A;
          break;
      case 3:   output1 = input1;
                output2 = Reg_A;
                output3 = input1;
                output4 = Reg_A;
          break;
      default:  output1 = 0;
                output2 = 0;
                output3 = 0;
                output4 = 0;
          break;
    }
    Serial.printf("Reg_A: %d", Reg_A);
    Serial.printf(", data_reg_file_input1: %d", input1);
    Serial.printf(", data_reg_file_output1: %d", output1);
    Serial.printf(", data_reg_file_output2: %d", output2);
    Serial.printf(", data_reg_file_output3: %d", output3);
    Serial.printf(", data_reg_file_output4: %d", output4);
    Serial.printf(", data_reg_file_sel: %d\n", sel);
}

void control_unit(int mux_sel, int add_sel, int sub_sel, int reg_file_sel, int command){
    switch (command){
        case 0:
                mux_sel = 0;
                add_sel = 1;
                sub_sel = 0;
                reg_file_sel = 1;
                break;
        case 1:
                mux_sel = 0;
                add_sel = 0;
                sub_sel = 1;
                reg_file_sel = 2;
                break;
        case 2:
                mux_sel = 1;
                add_sel = 0;
                sub_sel = 0;
                reg_file_sel = 0;
                break;
        case 3:
                mux_sel = 2;
                add_sel = 0;
                sub_sel = 0;
                reg_file_sel = 0;
                break;
        default:
                mux_sel = -1;
                add_sel = 0;
                sub_sel = 0;
                reg_file_sel = 0;
                break;
    }
    Serial.printf("signal_mux_sel: %d", mux_sel);
    Serial.printf(", signal_add_sel: %d", add_sel);
    Serial.printf(", signal_sub_sel: %d", sub_sel);
    Serial.printf(", signal_reg_file_sel: %d", reg_file_sel);
    Serial.printf(", command: %d\n", command);
}

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
                  opcode=prog_mem_add[Reg_PC];
                  operand_hi=prog_mem_add[Reg_PC+1];
                  operand_lo=prog_mem_add[Reg_PC+2];
                  command += 2;
                  control_unit(signal_mux_sel, signal_add_sel, signal_sub_sel, signal_reg_file_sel, command);
                  Serial.printf("signal_mux_sel: %d", signal_mux_sel);
                  Serial.printf(", signal_add_sel: %d", signal_add_sel);
                  Serial.printf(", signal_sub_sel: %d", signal_sub_sel);
                  Serial.printf(", signal_reg_file_sel: %d", signal_reg_file_sel);
                  Serial.printf(", command: %d\n", command);
                  mux(data_mux_input1, data_mux_input2, data_mux_input3, data_mux_output1, signal_mux_sel);
                  Serial.printf("data_mux_input1: %d", data_mux_input1);
                  Serial.printf(", data_mux_input2: %d", data_mux_input2);
                  Serial.printf(", data_mux_input3: %d", data_mux_input3);
                  Serial.printf(", data_mux_output1: %d", data_mux_output1);
                  Serial.printf(", data_mux_sel: %d\n", signal_mux_sel);
                  reg_file(data_reg_file_input1, data_reg_file_output1, data_reg_file_output2, data_reg_file_output3, data_reg_file_output4, signal_reg_file_sel);
                  Serial.printf("data_reg_file_input1: %d", data_reg_file_input1);
                  Serial.printf(", data_reg_file_output1: %d", data_reg_file_output1);
                  Serial.printf(", data_reg_file_output2: %d", data_reg_file_output2);
                  Serial.printf(", data_reg_file_output3: %d", data_reg_file_output3);
                  Serial.printf(", data_reg_file_output4: %d", data_reg_file_output4);
                  Serial.printf(", data_reg_file_sel: %d\n", signal_reg_file_sel);
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
                  data_memory_addr=operand_hi<<8;
                  data_memory_addr=data_memory_addr | operand_lo;
                  command = -1;
                  Serial.println("2) Decode state");
                  switch(opcode){
                    case 0x22:    // RET;
                                    Reg_PC = 0x00;
                                    break;
                    case 0x34:    // ADD A,#data
                                    command = 0;
                                    // Reg_A = Reg_A+operand_lo;
                                    // Serial.print("Register A :");
                                    // Serial.println(Reg_A,HEX);
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
                                    Serial.print(data_memory_addr);
                                    Serial.print("]:");
                                    Serial.println(data_memory[data_memory_addr],HEX);
                                    break;
                    case 0xA8:    // MOV R0, data addr
                                    Reg_R0 = data_memory[data_memory_addr];
                                    Serial.print("Data memory[");
                                    Serial.print(data_memory_addr);
                                    Serial.print("]:");
                                    Serial.println(Reg_R0,HEX);
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
                  control_unit(signal_mux_sel, signal_add_sel, signal_sub_sel, signal_reg_file_sel, command);
                  Serial.printf("signal_mux_sel: %d", signal_mux_sel);
                  Serial.printf(", signal_add_sel: %d", signal_add_sel);
                  Serial.printf(", signal_sub_sel: %d", signal_sub_sel);
                  Serial.printf(", signal_reg_file_sel: %d", signal_reg_file_sel);
                  Serial.printf(", command: %d\n", command);
                  state_next=0x02;
                  break;
    case  0x02  :// execute
                  data_mux_input1 = operand_lo;
                  mux(data_mux_input1, data_mux_input2, data_mux_input3, data_mux_output1, signal_mux_sel);
                  Serial.printf("data_mux_input1: %d", data_mux_input1);
                  Serial.printf(", data_mux_input2: %d", data_mux_input2);
                  Serial.printf(", data_mux_input3: %d", data_mux_input3);
                  Serial.printf(", data_mux_output1: %d", data_mux_output1);
                  Serial.printf(", data_mux_sel: %d\n", signal_mux_sel);
                  reg_file(data_reg_file_input1, data_reg_file_output1, data_reg_file_output2, data_reg_file_output3, data_reg_file_output4, signal_reg_file_sel);
                  Serial.printf("data_reg_file_input1: %d", data_reg_file_input1);
                  Serial.printf(", data_reg_file_output1: %d", data_reg_file_output1);
                  Serial.printf(", data_reg_file_output2: %d", data_reg_file_output2);
                  Serial.printf(", data_reg_file_output3: %d", data_reg_file_output3);
                  Serial.printf(", data_reg_file_output4: %d", data_reg_file_output4);
                  Serial.printf(", data_reg_file_sel: %d\n", signal_reg_file_sel);
                  add(data_add_input1, data_add_input2, data_add_output1, signal_add_sel);
                  Serial.printf("data_add_input1: %d", data_add_input1);
                  Serial.printf(", data_add_input2: %d", data_add_input2);
                  Serial.printf(", data_add_output1: %d", data_add_output1);
                  Serial.printf(", data_add_sel: %d\n", signal_add_sel);
                  subs(data_sub_input1, data_sub_input2, data_sub_output1, signal_sub_sel);
                  Serial.printf("data_sub_input1: %d", data_sub_input1);
                  Serial.printf(", data_sub_input2: %d", data_sub_input2);
                  Serial.printf(", data_sub_output1: %d", data_sub_output1);
                  Serial.printf(", data_sub_sel: %d\n", signal_sub_sel);

                  Serial.println("3) Execute state");
                  if(opcode != 0x22 && opcode != 0xFF) Reg_PC = Reg_PC+3;
                  Serial.print("Register PC :");
                  Serial.println(Reg_PC,HEX);
                  Serial.print("Register A :");
                  Serial.println(Reg_A,HEX);
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
 delay(500);
}