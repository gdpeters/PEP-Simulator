
//add #include headers as needed
#include <iostream>
#include <fstream>
using namespace std;


//add global variables if needed
struct registers {
	union {
		struct {
			unsigned int rA8R : 8;
			unsigned int rA8L : 8;
		} ;
		unsigned int rA16 : 16;
	};
	union {
		struct {
			unsigned int rX8R : 8;
			unsigned int rX8L : 8;
		} ;
		unsigned int rX16 : 16;
	};

	unsigned int pC : 16;
	unsigned int sP : 16;
	
	union {	//operand that will be displayed. Same as operand specifier if immediate addressing mode (000).
		struct {
			unsigned int op8R : 8;
			unsigned int op8L : 8;
		} ;
		unsigned int op16 : 16;
	}; 
};

union instructionSpec {

	struct {
		unsigned int reg1 : 1;
		unsigned int instr7 : 7;
	} unary;

	struct {
		unsigned int addr3 : 3;
		unsigned int instr5 : 5;
	} ioTraps;

	struct {
		unsigned int addr3 : 3;
		unsigned int reg1 : 1;
		unsigned int instr4 : 4;
	} logArith;
	
	struct {
		unsigned int bits4R : 4;
		unsigned int bits4L : 4;
	} halves;

	unsigned int fullIS : 8;
};

union operandSpec {

	struct {
		unsigned int bits8R : 8;
		unsigned int bits8L : 8;
	} halves;

	unsigned int fullOS : 16;
};

int updateOperand(int m [], operandSpec os, int aaa)
{
	return (aaa == 0) ? os : m[os];	
}



int main() {

/** Reads file and loads memory array */
	ifstream readFile;					//Creates an input stream
	readFile.open("input.txt", ios::in);			//opens file
	unsigned int mainMem [50];				//Creates main memory array
	unsigned int tempHex;					//Used in retrieving each hex value
	int count = 0;
	while (readFile >> hex >> tempHex) 			//Loads 1-byte from input file to variable
	{
		mainMem[count] = tempHex;			//Assigns value to main memory
		count++;
	}
	

/** Initiate registers, instruction specifier, operand specifier, and operand */
	registers reg;	//declare a set of new registers
	registers *regPointer;
	reg.pC = 0;	//initiate program counter to 0

/** Initiate an output file if we choose this route as opposed to screen display */

/** Iterate through memory array
Load and determine instruction specifier and increment PC 
Load operand specifier, extract register and addressing mode, update operand, and increment PC */
	
	instructionSpec inspTemp = mainMem[reg.pC];	//load first memory array cell into the instruction specifier
	reg.pC++;					//increment program counter
	operandSpec opspTemp = 0;			//initiate a temporary operand specifier variable				
	while (inspTemp.fullIS != 0)		//while instruction specifier is not STOP
	{
		if (inspTemp.halves.bits4L > 2) //if instruction is not unary
		{
			opspTemp.halves.bits8L = mainMem[reg.pC]; //load operand specifier first byte
			reg.pC++;				  //increment program counter
			opspTemp.halves.bits8R = mainMem[reg.pC]; //load operand specifier second byte
			reg.pC++;				  //increment program counter	
			reg.op16 = updateOperand(mainMem, opspTemp, inspTemp.ioTraps.addr3);
		}
		
		 /** Execute instruction by calling function (create subprograms when useful) */
		//**NEED TO CONDENSE, PUT INTO FUNCTIONS**//
		switch (inspTemp.halves.bits4L)
		{
			case 1 : //unary no operand
				if (inspTemp.unary.instr7 == 12) //bitwise invert
				{
					if (inspTemp.unary.reg1 == 0) 
					{
						reg.rA16 = reg.rA16 ^ ((1<<16) - 1); //is there a shorter way to write this?
					}
					else
					{
						reg.rX16 = reg.rX16 ^ ((1<<16) - 1); //is there a shorter way to write this?
					}
				}
				else if (inspTemp.unary.instr7 == 14)
				{
					if (inspTemp.unary.reg1 == 0) 
					{
						reg.rA16<<=1;	//arithmetic shift left
					}
					else
					{
						reg.rX16<<=1; 
					}
				}
				else
				{
					if (inspTemp.unary.reg1 == 0) 
					{
						reg.rA16>>=1;	//arithmetic shift right
					}
					else
					{
						reg.rX16>>=1; 
					}
				}
				break;
			case 2 : //unary no operand
				if (inspTemp.unary.instr7 == 20) //rotate left
				{
					if (inspTemp.unary.reg1 == 0)
						reg.rA16 = (reg.rA16 << 1) + (reg.rA16>>15);
					else
						reg.rX16 = (reg.rX16 << 1) + (reg.rX16>>15);
				}
				else	//rotate right
				{
					if (inspTemp.unary.reg1 == 0)
						reg.rA16 = (reg.rA16 >> 1) + (reg.rA16<<15);
					else
						reg.rX16 = (reg.rX16 >> 1) + (reg.rX16<<15);
				}
				break;
			case 3 : 
				break;
			case 4 : //FIGURE OUT IF ANY HEX/DEC CONVERSIONS NEED TO BE DONE
			case 5 :
			//no case 6
			case 7 : if (inspTemp.logArith.reg1 == 0)	//Add to r
					reg.rA16 += reg.op16;
				 else
					reg.rX16 += reg.op16;
				break;
			case 8 : if (inspTemp.logArith.reg1 == 0)	//Subtract from r
					reg.rA16 -= reg.op16;
				 else
					reg.rX16 -= reg.op16;
				break;
			case 9 : if (inspTemp.logArith.reg1 == 0)
					reg.rA16 = reg.rA16 && reg.op16;
				else
					reg.rX16 = reg.rX16 || reg.op16;
				break;
			case 10 :
			//no case 11
			case 12 : if (inspTemp.logArith.reg1 == 0)	//Load r from memory
					reg.rA16 = reg.op16;
				 else
					reg.rX16 = reg.op16;
				break;
			case 13 : if (inspTemp.logArith.reg1 == 0)	//Load byte from memory
					reg.rA8R = reg.op8R;
				 else
					reg.rX8R = reg.op8R;
				break;
			case 14 : if (inspTemp.logArith.reg1 == 0)	//Store r to memory
					mainMem[reg.op16] = reg.rA16;
				 else
					mainMem[reg.oP16] = reg.rX16;
				break;
			case 15 : if (inspTemp.logArith.reg1 == 0)	//Store byte from r to memory
					mainMem[reg.op8] = reg.rA8R;
				 else
					mainMem[reg.op8] = reg.rX8R;
				break;
		}
		inspTemp = mainMem[reg.pC];
		reg.pC++;
		/** Display register values to the screen or output file (call function to do this) */
	}


    
    
 /** What else are we missing here? */   
 
 
    
  return 0;
}
