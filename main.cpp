#include <iostream>
#include <fstream>
using namespace std;

struct registers {	
	union {
		struct {
			unsigned int rA8R : 8;	//Accumulator divided
			unsigned int rA8L : 8;
		} ;
		unsigned int rA16 : 16;	//Accumulator
	};
	union {
		struct {
			unsigned int rX8R : 8;	//Index divided
			unsigned int rX8L : 8;
		} ;
		unsigned int rX16 : 16;	//Index
	};

	unsigned int pC : 16;	//Program counter
	unsigned int sP : 16;	//Stack Pointer
	
	union {
		struct {
			unsigned int op8R : 8;	//Operand divided
			unsigned int op8L : 8;
		} ;
		unsigned int op16 : 16;	//Operand
	}; 
};

union instructionSpec {	//Instruction specifier
	struct {
		unsigned int reg1 : 1;
		unsigned int instr7 : 7;
	} unary;	//For unary instructions (0000 000r)
	struct {
		unsigned int addr3 : 3;
		unsigned int instr5 : 5;
	} ioTraps;	//For I/O instructions (0000 0aaa)
	struct {
		unsigned int addr3 : 3;
		unsigned int reg1 : 1;
		unsigned int instr4 : 4;	//Also used in switch statement to identify instruction number
	} logArith;	//For Arithmetic instructions (0000 raaa)
	
	unsigned int fullIS : 8;	//If fullIS == 00 then stop reading instructions
};

union operandSpec {	//Operand Specifier
	struct {
		unsigned int bits8R : 8;	
		unsigned int bits8L : 8;
	} load;	//Used when loading bytes from memory

	unsigned int fullOS : 16;	//**DELETE IF UNUSED?
};

unsigned int updateOperand(unsigned int m [], operandSpec os, int aaa)	//Updates operand with appropriate value 
{
	return (aaa == 0) ? os.fullOS : m[os.fullOS];	
}

int main() {
	ifstream readFile;				//Create an input stream
	readFile.open("input.txt", ios::in);		//Open file
	unsigned int mainMem [50];			//Create main memory array
	unsigned int tempHex;				//For retrieving each hex value from file
	int count = 0;					//Array index value
	
	while (readFile >> hex >> tempHex) 		//Load 1-byte from input file to variable
	{
		mainMem[count] = tempHex;		//Assign value to main memory
		count++;
	}
	
	registers reg;			//Declare a set of new registers
	reg.pC = 0;			//Initiate program counter to 0
	
	instructionSpec instrTemp;	//Declare a temporary instruction specifier that will be overriden with each iteration
	operandSpec operTemp;		//Declare a temporary operand specifier
	
	instrTemp.fullIS = mainMem[reg.pC];	//Load first memory array cell into the instruction specifier
	operTemp.fullOS = 0;			//Initiate a temporary operand specifier variable			
	reg.pC++;				//Increment program counter
	
	while (instrTemp.fullIS != 0)		//While instruction specifier is not STOP
	{
		if (instrTemp.logArith.instr4 > 2) //If instruction is not unary
		{
			operTemp.load.bits8L = mainMem[reg.pC]; 	//Load operand specifier first byte
			reg.pC++;				  	//Increment program counter
			operTemp.load.bits8R = mainMem[reg.pC]; 	//Load operand specifier second byte
			reg.pC++;				  	//Increment program counter	
			reg.op16 = updateOperand(mainMem, operTemp, instrTemp.ioTraps.addr3);	//Assign appropriate value to operand
		}
		
		switch (instrTemp.logArith.instr4)	//Check first nibble to extract instruction code
		{
			case 1 :	//Unary no operand
				if (instrTemp.unary.instr7 == 12)	//Bitwise invert
				{
					if (instrTemp.unary.reg1 == 0) 
						reg.rA16 = reg.rA16 ^ ((1<<16) - 1); //is there a shorter way to write this?
					else
						reg.rX16 = reg.rX16 ^ ((1<<16) - 1); //is there a shorter way to write this?
				}
				else if (instrTemp.unary.instr7 == 14)
				{
					if (instrTemp.unary.reg1 == 0) 
						reg.rA16<<=1;	//Arithmetic shift left Accumulator
					else
						reg.rX16<<=1;	//Arithmetic shift left Index
				}
				else
				{
					if (instrTemp.unary.reg1 == 0) 
						reg.rA16>>=1;	//Arithmetic shift right Accumulator
					else
						reg.rX16>>=1; 	//Arithmetic shift right Index
				}
				break;
			case 2 : //unary no operand
				if (instrTemp.unary.instr7 == 20) //Rotate left
				{
					if (instrTemp.unary.reg1 == 0)
						reg.rA16 = (reg.rA16 << 1) + (reg.rA16>>15);
					else
						reg.rX16 = (reg.rX16 << 1) + (reg.rX16>>15);
				}
				else	//Rotate right
				{
					if (instrTemp.unary.reg1 == 0)
						reg.rA16 = (reg.rA16 >> 1) + (reg.rA16<<15);
					else
						reg.rX16 = (reg.rX16 >> 1) + (reg.rX16<<15);
				}
				break;
			case 3 :	cout<<reg.op16; //for now
				break;
			case 4 : //FIGURE OUT IF ANY HEX/DEC CONVERSIONS NEED TO BE DONE
				break;
			case 5 :
				break;
			//no case 6
			case 7 : if (instrTemp.logArith.reg1 == 0)	//Add to r
					reg.rA16 += reg.op16;
				 else
					reg.rX16 += reg.op16;
				break;
			case 8 : if (instrTemp.logArith.reg1 == 0)	//Subtract from r
					reg.rA16 -= reg.op16;
				 else
					reg.rX16 -= reg.op16;
				break;
			case 9 : if (instrTemp.logArith.reg1 == 0)	
					reg.rA16 = reg.rA16 && reg.op16;	//Bitwise AND
				else
					reg.rX16 = reg.rX16 || reg.op16;	
				break;
			case 10 :
				break;
			//no case 11	
			case 12 : if (instrTemp.logArith.reg1 == 0)	//Load r from memory
					reg.rA16 = reg.op16;
				 else
					reg.rX16 = reg.op16;
				break;
			case 13 : if (instrTemp.logArith.reg1 == 0)	//Load byte from memory
					reg.rA8R = reg.op8R;
				 else
					reg.rX8R = reg.op8R;
				break;
			case 14 : if (instrTemp.logArith.reg1 == 0)	//Store r to memory
					mainMem[reg.op16] = reg.rA16;
				 else
					mainMem[reg.op16] = reg.rX16;
				break;
			case 15 : if (instrTemp.logArith.reg1 == 0)	//Store byte to memory
					mainMem[reg.op8R] = reg.rA8R;
				 else
					mainMem[reg.op8R] = reg.rX8R;
				break;
		}
		instrTemp.fullIS = mainMem[reg.pC];
		reg.pC++;
		/** Display register values to the screen or output file (call function to do this) */
	}
  return 0;
}
