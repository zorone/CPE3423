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

uint16_t Reg_PC;    // program counter Register
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

void mux() {
  switch (signal_mux_sel){
    case 0 :  data_mux_output1 = data_mux_input1;
              break;
    case 1 :  data_mux_output1 = data_mux_input2;
              break;
    case 2 :  data_mux_output1 = data_mux_input3;
              break;
    default:  data_mux_output1 = data_mux_input1;
              break;
    }
    data_reg_file_input1 = data_mux_output1;
}

void add(){
    if (signal_add_sel==1){
        data_add_output1=data_add_input1+data_add_input2;
    }
    data_mux_input2 = data_add_output1;
}

void subs(){
    if (signal_sub_sel==1){
        data_sub_output1=data_sub_input1-data_sub_input2;
    }
    data_mux_input3 = data_sub_output1;
}

void reg_file(){
    switch(signal_reg_file_sel){
      case 0:   data_reg_file_output1 = 0;
                data_reg_file_output2 = 0;
                data_reg_file_output3 = 0;
                data_reg_file_output4 = 0;
                Reg_A   = data_reg_file_input1;
                Serial.printf("Register A : %#04x\n", Reg_A);
          break;
      case 1:   data_reg_file_output1 = data_reg_file_input1;
                data_reg_file_output2 = Reg_A;
                data_reg_file_output3 = 0;
                data_reg_file_output4 = 0;
          break;
      case 2:   data_reg_file_output1 = 0;
                data_reg_file_output2 = 0;
                data_reg_file_output3 = data_reg_file_input1;
                data_reg_file_output4 = Reg_A;
          break;
      case 3:   data_reg_file_output1 = data_reg_file_input1;
                data_reg_file_output2 = Reg_A;
                data_reg_file_output3 = data_reg_file_input1;
                data_reg_file_output4 = Reg_A;
          break;
      default:  data_reg_file_output1 = 0;
                data_reg_file_output2 = 0;
                data_reg_file_output3 = 0;
                data_reg_file_output4 = 0;
          break;
    }
    data_add_input1 = data_reg_file_output1;
    data_add_input2 = data_reg_file_output2;
    data_sub_input1 = data_reg_file_output3;
    data_sub_input2 = data_reg_file_output4;
}

void control_unit(){
    switch (command){
        case 0:
                signal_mux_sel = 0;
                signal_add_sel = 1;
                signal_sub_sel = 0;
                signal_reg_file_sel = 1;
                break;
        case 1:
                signal_mux_sel = 0;
                signal_add_sel = 0;
                signal_sub_sel = 1;
                signal_reg_file_sel = 2;
                break;
        case 2:
                signal_mux_sel = 1;
                signal_add_sel = 0;
                signal_sub_sel = 0;
                signal_reg_file_sel = 0;
                break;
        case 3:
                signal_mux_sel = 2;
                signal_add_sel = 0;
                signal_sub_sel = 0;
                signal_reg_file_sel = 0;
                break;
        default:
                signal_mux_sel = -1;
                signal_add_sel = 0;
                signal_sub_sel = 0;
                signal_reg_file_sel = -1;
                break;
    }
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

    // https://stackoverflow.com/questions/14733761/printf-formatting-for-hexadecimal
    
  switch(state_current){
    case  0x00  :// fetch
                  opcode=prog_mem_add[Reg_PC];
                  operand_hi=prog_mem_add[Reg_PC+1];
                  operand_lo=prog_mem_add[Reg_PC+2];
                  command += 2;  // Increment command by 2, this is by architecture design
                  control_unit();
                  mux();
                  reg_file();
                  state_next=0x01;
                  break;
    case  0x01  :// decode
                  Serial.printf("opcode: %#04x, PC: %#06x ", program_mem[Reg_PC], Reg_PC);

                  data_memory_addr=operand_hi<<8;
                  data_memory_addr=data_memory_addr | operand_lo;
                  command = -1;
                  switch(opcode){
                    case 0x14:    // DEC A;
                                    Reg_A--;
                                    Serial.printf("Reg_A: %#04x\n", Reg_A);
                                    break;
                    case 0x22:    // RET;
                                    Reg_PC = 0x00;
                                    Serial.printf("Reg_PC: %#06x\n", Reg_PC);
                                    break;
                    case 0x34:    // ADD A,#data
                                    command = 0;
                                    // Serial.printf("Register A : %#04x\n", Reg_A);    // will show when trigger reg_file()
                                    // Reg_A = Reg_A+operand_lo;
                                    // Serial.print("Register A :");
                                    // Serial.println(Reg_A,HEX);
                                    break;
                    case 0x44:    // ORL A,#data
                                    Reg_A |= operand_lo;
                                    Serial.printf("Register A : %#04x\n", Reg_A);
                                    break;
                    case 0x54:    // AND A,#data
                                    Reg_A &= operand_lo;
                                    Serial.printf("Register A : %#04x\n", Reg_A);
                                    break;
                    case 0x74:    // MOV A, #data
                                    Reg_A = operand_lo;
                                    Serial.printf("Register A : %#04x\n", Reg_A);
                                    break;
                    case 0x88:    // MOV data addr,R0
                                    data_memory[data_memory_addr] = Reg_R0;
                                    Serial.printf("Data memory[%#04x]: %#04x\n", data_memory_addr, data_memory[data_memory_addr]);
                                    break;
                    case 0xA8:    // MOV R0, data addr
                                    Reg_R0 = data_memory[data_memory_addr];
                                    Serial.printf("Data memory[%#04x]: %#04x\n", data_memory_addr, data_memory[data_memory_addr]);
                                    break;
                    case 0xF5:    // MOV data addr, A;
                                    data_memory[data_memory_addr] = Reg_A;
                                    Serial.printf("Data memory[%#04x]: %#04x\n", data_memory_addr, Reg_A);
                                    break;
                    case 0xF8:    // MOV R0, A;
                                    Reg_R0 = Reg_A;
                                    Serial.printf("Register R0 : %#04x\n", Reg_R0);
                                    break;
                    case 0xFF:    // MOV PC, #0x00;
                                    Reg_PC = 0x00;
                                    Serial.printf("Reg_PC: %#06x\n", Reg_PC);
                                    break;
                    default:
                            break;
                  }                
                  control_unit();
                  state_next=0x02;
                  break;
    case  0x02  :// execute
                  data_mux_input1 = operand_lo;
                  mux();
                  reg_file();
                  add();
                  subs();

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