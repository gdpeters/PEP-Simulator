/**
 * PEP/8 Simulator
 * @author Genevieve Peters
 * 
 * View on Github: https://github.com/gdpeters/PEP8-Simulator
 * View on Bits&Pieces blog: https://bitsandpieces.tech/2020/01/10/c-pep-8-virtual-computer/
 * 
 * This C++ program simulates the PEP/8 Virtual Computer. It computes
 * a set of hexadecimal machine code instructions from a file. Only 18 instruction
 * specifiers and 2 addressing modes are allowed. [See instructionSet.txt]
 * 
 * There should be 3-bytes to an instruction (1-byte instruction specifier, 2-byte operand)
 * except for Unary instructions which have only 1-byte.
 * 
 * To run:
 *      $ g++ pep8.cpp
 *      $ ./a.out
 * 
 * Output file and console prints register values after each instruction.
 * 
 * For clarification:
 *      Accumulator = 0
 *      Index = 1
 *      Immediate Addressing = 000
 *      Direct Addressing = 001
 */


#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <locale>
using namespace std;

unsigned int MEMORY_SIZE = 10000;   //Size can be changed as desired
unsigned int sP = MEMORY_SIZE - 1072;    //Stack Pointer positioning

struct registerAI {     //Accumulator and Index registers
    union {
        struct {
            unsigned int lsd8 : 8;
            unsigned int msd8 : 8;
        };
        unsigned int full : 16;
    };
};

struct registers {      //Processing unit
    
    unsigned int pC : 16;    //Program counter
    registerAI pickRegister[2];    //Accesses Accumulator/Index registers
    
    //Instruction Specifier divided into various bit structures
    union {
        struct {
            unsigned int r1 : 1;
            unsigned int instr7 : 7;
        } unary;    //Unary instructions (0000 000r)
        
        struct {
            unsigned int aaa3 : 3;
            unsigned int instr5 : 5;
        } ioTrap;    //I/O instructions (0000 0aaa)
        
        struct {
            unsigned int aaa3 : 3;
            unsigned int r1 : 1;
            unsigned int instr4 : 4;
        } arith;    //Arithmetic instructions (0000 raaa)
        
        unsigned int full : 8;  //Full instruction specifier byte
    } iSpec;
    
    //Operand Specifier, Operand, and temporary values (for Deci/Deco)
    union {
        struct {
            unsigned int lsd8 : 8;
            unsigned int msd8 : 8;
        };
        unsigned int full : 16;
    } oSpec, operand, tempValue, tempAddress;
};

//Prints register values when called after each computation
string printRegisters(registers rgstr)
{
    stringstream stream;
    stream << showbase << uppercase << hex << internal << setfill('0');
    stream << "Accumulator: " << setw(6) << rgstr.pickRegister[0].full
    << "\nIndex: " << setw(6) << rgstr.pickRegister[1].full
    << "\nStack Pointer: " << setw(6) << sP
    << "\nProgram Counter: " << setw(6) << rgstr.pC
    << "\nInstruction Specifier: " << setw(4) << rgstr.iSpec.full
    << "\nOperand Specifier: " << setw(6) << rgstr.oSpec.full
    << "\nOperand: " << setw(6) << rgstr.operand.full << "\n\n";
    
    return stream.str();
}

//Enter filename on command line or sample file will be generated
int main(int argc, char* argv[]) {

    string fileName;
    ifstream readFile;
    string runningOutput = "";

    if (argc > 1)//if file is provided
    {
        fileName = argv[1];
    	//Input file with hexadecimal machine code
    	readFile.open(fileName, ios::in);

    }
    else//if file is not provided, sample hex code will be generated
    {
    	fileName = "hello.txt";
    	ofstream testFile;
    	testFile.open(fileName);
    
    	testFile<<"C0 00 48\nE1 00 30\nC8 00 65\nE9 00 32\n78 00 07\n51 00 31\n51 00 33\nF9 00 35\n51 00 35\n51 00 35\nC1 00 32\n70 00 0A\nE1 00 40\n51 00 41\n00";

        testFile.close();
        readFile.open(fileName, ios::in);
    }

    ofstream outputFile;
    //Output file with register values after each computation
    outputFile.open("output.txt");
   
    registers cpu;          //Declare a set of new registers and reset
    cpu.pickRegister[0].full = 0;
    cpu.pickRegister[1].full = 0;
    cpu.oSpec.full = 0;
    cpu.pC = 0;

    unsigned int mainMem[MEMORY_SIZE];
    unsigned int tempHex;
    int byteIndex = 0;
    cout<<endl<<"Reading file...";
    outputFile<<"Reading file...";
    //Load every byte from input file into main memory
    while (readFile >> hex >> tempHex)
    {
        mainMem[byteIndex++] = tempHex;
        cout<<hex<<tempHex<<" ";
        outputFile<<hex<<tempHex<<" ";
    }
    cout<<endl<<endl;
    outputFile<<endl<<endl;
    //Load first instruction from memory array
    cpu.iSpec.full = mainMem[cpu.pC++];
    
    while (cpu.iSpec.full != 0)     //While the instruction is not 00 = Stop
    {
        //Update operand specifier if not Unary
        if (cpu.iSpec.arith.instr4 > 3)
        {
            cpu.oSpec.msd8 = mainMem[cpu.pC++];
            cpu.oSpec.lsd8 = mainMem[cpu.pC++];
        }
        
        if (cpu.iSpec.arith.instr4 == 5)
        {
            cpu.operand.full = (cpu.iSpec.arith.aaa3 == 0) * (cpu.oSpec.full - mainMem[cpu.oSpec.full]) + mainMem[cpu.oSpec.full];
        }
        //Update operand if instruction code is not unary, I/O, or storing registers
        if ((cpu.iSpec.arith.instr4 > 5) && (cpu.iSpec.arith.instr4 < 14))
        {
            cpu.operand.msd8 = (cpu.iSpec.arith.aaa3 == 0) * (cpu.oSpec.msd8 - mainMem[cpu.oSpec.full]) + mainMem[cpu.oSpec.full];
            cpu.operand.lsd8 = (cpu.iSpec.arith.aaa3 == 0) * (cpu.oSpec.lsd8 - mainMem[cpu.oSpec.full + 1]) + mainMem[cpu.oSpec.full + 1];
        }

        //Check first nibble to extract instruction code
        switch (cpu.iSpec.arith.instr4)
        {
            case 1:     //Shifts and Invert
            {
                cpu.operand.full = 0;    //Unary no operand
                switch (cpu.iSpec.unary.instr7)
                {
                    case 12: //Bitwise invert
                        cpu.pickRegister[cpu.iSpec.unary.r1].full = cpu.pickRegister[cpu.iSpec.unary.r1].full ^ ((1 << 16) - 1);
                        cout<<"---Instruction: Bitwise invert---"<<endl;
                        outputFile<<"---Instruction: Bitwise invert---"<<endl;
                        break;
                    case 14: //Arithmetic shift left
                        cout<<"---Instruction: Arithmetic shift left---"<<endl;
                        outputFile<<"---Instruction: Arithmetic shift left---"<<endl;
                        cpu.pickRegister[cpu.iSpec.unary.r1].full <<= 1;
                        break;
                    case 15: //Arithmetic shift right
                        cout<<"---Instruction: Arithmetic shift right---"<<endl;
                        outputFile<<"---Instruction: Arithmetic shift right---"<<endl;
                        cpu.pickRegister[cpu.iSpec.unary.r1].full >>= 1;
                        break;
                }
                break;
            }
            case 2:     //Rotate
            {
                cpu.operand.full = 0;    //Unary no operand
                switch (cpu.iSpec.unary.instr7)
                {
                    case 16: //Rotate left
                        cout<<"---Instruction: Rotate left---"<<endl;
                        outputFile<<"---Instruction: Rotate left---"<<endl;
                        cpu.pickRegister[cpu.iSpec.unary.r1].full = (cpu.pickRegister[cpu.iSpec.unary.r1].full << 1) + (cpu.pickRegister[cpu.iSpec.unary.r1].full >> 15);
                        break;
                    case 17: //Rotate right
                        cout<<"---Instruction: Rotate right---"<<endl;
                        outputFile<<"---Instruction: Rotate right---"<<endl;
                        cpu.pickRegister[cpu.iSpec.unary.r1].full = (cpu.pickRegister[cpu.iSpec.unary.r1].full >> 1) + (cpu.pickRegister[cpu.iSpec.unary.r1].full << 15);
                        break;
                }
                break;
            }
            case 3:     //Decimal I/O
            {
                
                switch (cpu.iSpec.ioTrap.instr5)
                {
                    case 6:    //Decimal input
                        cout<<"---Instruction: Decimal input---"<<endl;
                        outputFile<<"---Instruction: Decimal input---"<<endl;
                        cout<<"Enter a decimal: ";
                        outputFile<<"Enter a decimal: ";
                        int decI;
                        cin >> decI;
                        outputFile<<decI;
                        cpu.tempValue.full = decI;
                        cpu.tempAddress.msd8 = mainMem[cpu.pC++];
                        cpu.tempAddress.lsd8 = mainMem[cpu.pC++];
                        mainMem[cpu.tempAddress.full] = cpu.tempValue.msd8;
                        mainMem[cpu.tempAddress.full + 1] = cpu.tempValue.lsd8;
                        break;
                    case 7:    //Decimal output
                        
                        unsigned int decO;
                        if (cpu.iSpec.ioTrap.aaa3 == 1) //Direct addressing
                        {
                            cpu.tempAddress.msd8 = mainMem[cpu.pC++];
                            cpu.tempAddress.lsd8 = mainMem[cpu.pC++];
                            cpu.tempValue.msd8 = mainMem[cpu.tempAddress.full];
                            cpu.tempValue.lsd8 = mainMem[cpu.tempAddress.full + 1];
                        }
                        else    //immediate addressing
                        {
                            cpu.tempValue.msd8 = mainMem[cpu.pC++];
                            cpu.tempValue.lsd8 = mainMem[cpu.pC++];
                        }
                        
                        decO = cpu.tempValue.full;
                        runningOutput = runningOutput + to_string(decO);
                        cout<<"---Instruction: Decimal output---"<<endl;
                        outputFile<<"---Instruction: Decimal output---"<<endl;
                        cout<<"Decmial output: "<<decO<<endl<<endl;
                        outputFile<<"Decmial output: "<<decO<<endl<<endl;
                        break;
                }
                break;
            }
            case 4:    //Character input only takes 1-byte (0x00)
            {
                cout<<"---Instruction: Character input---"<<endl;
                outputFile<<"---Instruction: Character input---"<<endl;
                cout<<"Enter a character: ";
                outputFile<<"Enter a character: ";
                char charI;
                cin>>charI;
                mainMem[cpu.oSpec.full] = charI;
                cpu.operand.full = mainMem[cpu.oSpec.full];
                cout<<endl;
                break;
            }
            case 5:    //Character output only takes 1-byte (00)
            {
                char charO = (char) cpu.operand.full;
                runningOutput = runningOutput + charO;
                cout<<"---Instruction: Character output---"<<endl;
                outputFile<<"---Instruction: Character output---"<<endl;
                cout<<"Character output: "<<charO<<endl<<endl;
                outputFile<<"Character output: "<<charO<<endl<<endl;
                break;
            }
                
            //Case 6 not included in instruction set
                
            case 7:    //Add to register
            {
                cpu.pickRegister[cpu.iSpec.arith.r1].full += cpu.operand.full;
                cout<<"---Instruction: Add to register---"<<endl;
                outputFile<<"---Instruction: Add to register---"<<endl;
                break;
            }
            case 8:    //Subtract from register
            {
                cout<<"---Instruction: Subtract from register---"<<endl;
                outputFile<<"---Instruction: Subtract from register---"<<endl;
                cpu.pickRegister[cpu.iSpec.arith.r1].full -= cpu.operand.full;
                break;
            }
            case 9: //Bitwise AND
            {
                cout<<"---Instruction: Bitwise AND---"<<endl;
                outputFile<<"---Instruction: Bitwise AND---"<<endl;
                cpu.pickRegister[cpu.iSpec.arith.r1].full &= cpu.operand.full;
                break;
            }
            case 10: //Bitwise OR
            {
                cout<<"---Instruction: Bitwise OR---"<<endl;
                outputFile<<"---Instruction: Bitwise OR---"<<endl;
                cpu.pickRegister[cpu.iSpec.arith.r1].full |= cpu.operand.full;
                break;
            }
                
            //Case 11 not included in instruction set
                
            case 12: //Load register from memory
            {
                cout<<"---Instruction: Load register from memory---"<<endl;
                outputFile<<"---Instruction: Load register from memory---"<<endl;
                cpu.pickRegister[cpu.iSpec.arith.r1].full = cpu.operand.full;
                break;
            }
            case 13: //Load byte from memory
            {
                cout<<"---Instruction: Load byte from memory---"<<endl;
                outputFile<<"---Instruction: Load byte from memory---"<<endl;
                cpu.pickRegister[cpu.iSpec.arith.r1].lsd8 = cpu.operand.lsd8;
                break;
            }
            case 14: //Store register to memory
            {
                cout<<"---Instruction: Store register to memory---"<<endl;
                outputFile<<"---Instruction: Store register to memory---"<<endl;
                cpu.operand.full = cpu.pickRegister[cpu.iSpec.arith.r1].full;
                mainMem[cpu.oSpec.full] = cpu.operand.msd8;
                mainMem[cpu.oSpec.full + 1] = cpu.operand.lsd8;
                break;
            }
            case 15: //Store byte from register to memory
            {
                cout<<"---Instruction: Store byte from register to memory---"<<endl;
                outputFile<<"---Instruction: Store byte from register to memory---"<<endl;
                cpu.operand.full = cpu.pickRegister[cpu.iSpec.arith.r1].lsd8;
                mainMem[cpu.oSpec.full] = cpu.operand.lsd8;
                break;
            }
        }
        cpu.tempValue.full = 0;     //Reset temporary value
        cpu.tempAddress.full = 0;   //Reset temporary address
        outputFile<<printRegisters(cpu);    //Print registers to output file
        cout<<printRegisters(cpu);    //Print registers to console
        
        //Load next instruction into the Instruction Specifier
        cpu.iSpec.full = mainMem[cpu.pC++];
    }
    
    cout<<"Program Output: "<<runningOutput<<endl<<endl;
    outputFile<<"Program Output: "<<runningOutput<<endl<<endl;
    outputFile.close(); //Close output file

    return 0;
}