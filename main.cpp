
//add #include headers as needed
#include <iostream>
#include <fstream>
using namespace std;


//add global variables if needed
struct registers {
	int rA : 16;
	int rX : 16;
	unsigned int pC : 16;
	unsigned int sP : 16;
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

void updateOperand(

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
		}
		/**still need to update operand (maybe w/in each case?)*/
		 /** Execute instruction by calling function (create subprograms when useful) */
		switch (inspTemp.halves.bits4L)
		{
			case 1 : //unary no operand
				break;
			case 2 : //unary no operand
				break;
			case 3 :
				break;
			case 4 : 
			case 5 :
			//no case 6
			case 7 :
			case 8 :
			case 9 :
			case 10 :
			//no case 11
			case 12 :
			case 13 :
			case 14 :
			case 15 :
		}
		
		/** Display register values to the screen or output file (call function to do this) */
	}


    
    
 /** What else are we missing here? */   
 
 
    
  return 0;
}
