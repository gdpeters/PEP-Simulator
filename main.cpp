#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

struct registers {
    union { //Accumulator & Index registers
        struct {
            unsigned int lsd8 : 8;
            unsigned int msd8 : 8;
        };
        unsigned int full : 16;
    } rA, rX;

    unsigned int sP = 0xFBCF;    //Stack Pointer
    unsigned int pC : 16;    //Program counter
    
    union {    //Instruction specifier
        struct {
            unsigned int r1 : 1;
            unsigned int instr7 : 7;
        } unary;    //For unary instructions (0000 000r)
        struct {
            unsigned int aaa3 : 3;
            unsigned int instr5 : 5;
        } ioTrap;    //For I/O instructions (0000 0aaa)
        struct {
            unsigned int aaa3 : 3;
            unsigned int r1 : 1;
            unsigned int instr4 : 4;
        } arith;    //For Arithmetic instructions (0000 raaa)
        unsigned int full : 8;
    } iSpec;
    
    union { //Operand Specifier, Operand, and temporary
        struct {
            unsigned int lsd8 : 8;
            unsigned int msd8 : 8;
        };
        unsigned int full : 16;
    } oSpec, operand, tempValue, tempAddr;
};

void printRegisters(registers rgstr)
{
    cout << showbase << uppercase << hex;
    cout << internal << setfill('0');
    
    cout<<"Current Registers [\n";
    cout<<"Accumulator: "<< setw(6) << rgstr.rA.full;
    cout<<"\nIndex: "<< setw(6)<< rgstr.rX.full;
    cout<<"\nStack Pointer: " << setw(6)<< rgstr.sP;
    cout<<"\nProgram Counter: "<< setw(6) << rgstr.pC;
    cout<<"\nInstruction Specifier: "<<setw(4)<< rgstr.iSpec.full;
    cout<<"\nOperand Specifier: "<< setw(6)<< rgstr.oSpec.full;
    cout<<"\nOperand: "<< setw(6)<<rgstr.operand.full<<" ]\n\n";
}

int main() {
    ifstream readFile;
    readFile.open("hello.txt", ios::in);

    unsigned int mainMem[50];
    unsigned int tempHex;
    int count = 0;
    
    while (readFile >> hex >> tempHex)
    {
        mainMem[count++] = tempHex;  //Load 1-byte from input file into main memory
    }
    
    registers reg;         //Declare a set of new registers
    reg.pC = 0;
    reg.iSpec.full = mainMem[reg.pC++];    //Load first memory array cell into the instruction specifier

    while (reg.iSpec.full != 0)
    {
        if (reg.iSpec.arith.instr4 > 3) //Update operand specifier for I/O and Arithmetic instructions
        {
            reg.oSpec.msd8 = mainMem[reg.pC++];
            reg.oSpec.lsd8 = mainMem[reg.pC++];
            if ((reg.iSpec.arith.instr4 > 5) && (reg.iSpec.arith.instr4 < 14))  //Update operand for those using immediate and direct addressing modes
            {
                if (reg.iSpec.arith.aaa3 == 0)
                    reg.operand.full = reg.oSpec.full;
                else
                {
                    reg.operand.msd8 = mainMem[reg.oSpec.full]; //Assigns first byte
                    reg.operand.lsd8 = mainMem[reg.oSpec.full + 1]; //Assigns second byte
                }
            }
        }
        
        switch (reg.iSpec.arith.instr4)    //Check first nibble to extract instruction code
        {
            case 1:
                reg.operand.full = 0;    //Unary no operand
                reg.oSpec.full = 0;
                switch (reg.iSpec.unary.instr7)
            {
                case 12: //Bitwise invert
                    if (reg.iSpec.unary.r1 == 0)
                        reg.rA.full = reg.rA.full ^ ((1 << 16) - 1);
                    else
                        reg.rX.full = reg.rX.full ^ ((1 << 16) - 1);
                    break;
                case 14: //Arithmetic shift left
                    if (reg.iSpec.unary.r1 == 0)
                        reg.rA.full <<= 1;
                    else
                        reg.rX.full <<= 1;
                    break;
                case 15: //Arithmetic shift right
                    if (reg.iSpec.unary.r1 == 0)
                        reg.rA.full >>= 1;
                    else
                        reg.rX.full >>= 1;
                    break;
                default: cout << "Error: Case 1";
            }
                break;
                
            case 2:
                reg.operand.full = 0;    //Unary no operand
                reg.oSpec.full = 0;
                switch (reg.iSpec.unary.instr7)
            {
                case 16: //Rotate left
                    if (reg.iSpec.unary.r1 == 0)
                        reg.rA.full = (reg.rA.full << 1) + (reg.rA.full >> 15);
                    else
                        reg.rX.full = (reg.rX.full << 1) + (reg.rX.full >> 15);
                    break;
                case 17: //Rotate right
                    if (reg.iSpec.unary.r1 == 0)
                        reg.rA.full = (reg.rA.full >> 1) + (reg.rA.full << 15);
                    else
                        reg.rX.full = (reg.rX.full >> 1) + (reg.rX.full << 15);
                    break;
                default: cout << "Error: Case 2";
                    
            }
                break;
                
            case 3:
                switch (reg.iSpec.ioTrap.instr5)
            {
                case 6:    //Decimal input
                    int decIn;
                    cin>>decIn;
                    reg.tempValue.full = decIn;
                    reg.tempAddr.msd8 = mainMem[reg.pC++];
                    reg.tempAddr.lsd8 = mainMem[reg.pC++];
                    mainMem[reg.tempAddr.full] = reg.tempValue.msd8;
                    mainMem[reg.tempAddr.full + 1] = reg.tempValue.lsd8;
                    break;
                case 7:    //Decimal output
                    if (reg.iSpec.ioTrap.aaa3 == 0)
                    {
                        reg.tempValue.msd8 = mainMem[reg.pC++];
                        reg.tempValue.lsd8 = mainMem[reg.pC++];
                        cout << dec << reg.tempValue.full << endl;        //Need to sort out hex vs dec
                    }
                    else
                    {
                        reg.tempAddr.msd8 = mainMem[reg.pC++];
                        reg.tempAddr.lsd8 = mainMem[reg.pC++];
                        reg.tempValue.msd8 = mainMem[reg.tempAddr.full];
                        reg.tempValue.lsd8 = mainMem[reg.tempAddr.full + 1];
                        cout << dec << reg.tempValue.full << endl;        //Need to sort out hex vs dec
                    }
                    break;
                default: cout << "Error: Case 3";
            }
                reg.tempValue.full = 0;    //reset temporary value
                reg.tempAddr.full = 0;    //reset temporary address
                break;
                
            case 4:    //Character input only takes 1-byte (00)
                //cin? NEED TO FINISH
                //update operand: reg.operand.full = mainMem[reg.oSpec.full];
                break;
                
            case 5:    //Character output only takes 1-byte (00)
                reg.operand.full = mainMem[reg.oSpec.full];
                cout << (char)reg.operand.full << endl;         //Need to sort out hex vs ASCII
                break;
                //no case 6
            case 7:    //Add to register
                if (reg.iSpec.arith.r1 == 0)
                    reg.rA.full += reg.operand.full;
                else
                    reg.rX.full += reg.operand.full;
                break;
            case 8:    //Subtract from register
                if (reg.iSpec.arith.r1 == 0)
                    reg.rA.full -= reg.operand.full;
                else
                    reg.rX.full -= reg.operand.full;
                break;
            case 9: //Bitwise AND
                if (reg.iSpec.arith.r1 == 0)
                    reg.rA.full &= reg.operand.full;
                else
                    reg.rX.full &= reg.operand.full;
                break;
            case 10: //Bitwise OR
                if (reg.iSpec.arith.r1 == 0)
                    reg.rA.full |= reg.operand.full;
                else
                    reg.rX.full |= reg.operand.full;
                break;
                //no case 11
            case 12: //Load register from memory
                if (reg.iSpec.arith.r1 == 0)
                    reg.rA.full = reg.operand.full;
                else
                    reg.rX.full = reg.operand.full;
                break;
            case 13: //Load byte from memory
                if (reg.iSpec.arith.r1 == 0)
                    reg.rA.lsd8 = reg.operand.lsd8;
                else
                    reg.rX.lsd8 = reg.operand.lsd8;
                break;
            case 14: //Store register to memory
                if (reg.iSpec.arith.r1 == 0)
                {
                    reg.operand.full = reg.rA.full;
                    mainMem[reg.oSpec.full] = reg.operand.msd8;
                    mainMem[reg.oSpec.full + 1] = reg.operand.lsd8;
                }
                else
                {
                    reg.operand.full = reg.rX.full;
                    mainMem[reg.oSpec.full] = reg.operand.msd8;
                    mainMem[reg.oSpec.full + 1] = reg.operand.lsd8;
                }
                break;
            case 15: //Store byte from register to memory
                if (reg.iSpec.arith.r1 == 0)
                {
                    reg.operand.full = reg.rA.lsd8;
                    mainMem[reg.oSpec.full] = reg.operand.lsd8;
                }
                else
                {
                    reg.operand.full = reg.rX.lsd8;
                    mainMem[reg.oSpec.full] = reg.rX.lsd8;
                }
                break;
        }
        
        printRegisters(reg);
        reg.iSpec.full = mainMem[reg.pC++];
    }
    return 0;
}
