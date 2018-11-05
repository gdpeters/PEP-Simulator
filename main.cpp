#include <iostream>
#include <fstream>
using namespace std;

struct registers {
	union {
		struct {
			unsigned int rA8R : 8;    //Accumulator divided
			unsigned int rA8L : 8;
		};
		unsigned int rA16 : 16;    //Accumulator
	};
	union {
		struct {
			unsigned int rX8R : 8;    //Index divided
			unsigned int rX8L : 8;
		};
		unsigned int rX16 : 16;    //Index
	};

	unsigned int pC : 16;    //Program counter
	unsigned int sP : 16;    //Stack Pointer

	union {    //Instruction specifier
		struct {
			unsigned int r : 1;
			unsigned int instr7 : 7;
		} unary;    //For unary instructions (0000 000r)
		struct {
			unsigned int aaa : 3;
			unsigned int instr5 : 5;
		} ioTraps;    //For I/O instructions (0000 0aaa)
		struct {
			unsigned int aaa : 3;
			unsigned int r : 1;
			unsigned int instr4 : 4;    //Also used in switch statement to identify instruction number
		} logArith;    //For Arithmetic instructions (0000 raaa)

		unsigned int full : 8;    //If fullIS == 00 then stop reading instructions
	} instrSpecifier;

	union {    //Operand Specifier
		struct {
			unsigned int byteR : 8;
			unsigned int byteL : 8;
		};    //Used when loading bytes from memory

		unsigned int full : 16;    //**DELETE IF UNUSED?
	} operSpecifier;

	union {
		struct {
			unsigned int byteR : 8;    //Operand divided
			unsigned int byteL : 8;
		};
		unsigned int full : 16;    //Operand
	} operand;
};
union temporaryUnit {
	struct {
		unsigned int byteR : 8;
		unsigned int byteL : 8;
	};
	unsigned int full : 16;
}tempValue, tempAddr;

void updateOperand(unsigned int mem[], registers rgst, int aaa)    //Updates operand with appropriate value
{
	if (aaa == 0)					//Immediate operand = os
		rgst.operand.full = rgst.operSpecifier.full;
	else							//Direct operand = mem[os]
		rgst.operand.full = mem[rgst.operSpecifier.full];
}


int main() {
	ifstream readFile;                //Create an input stream
	readFile.open("text.txt", ios::in);        //Open file
	unsigned int mainMem[50];            //Create main memory array
	unsigned int tempHex;                //For retrieving each hex value from file
	int count = 0;                    //Array index value

	while (readFile >> hex >> tempHex)         //Load 1-byte from input file to variable
	{
		mainMem[count] = tempHex;        //Assign value to main memory
		count++;

	}

	registers reg;            //Declare a set of new registers
	reg.pC = 0;            //Initiate program counter to 0
	reg.instrSpecifier.full = mainMem[reg.pC++];    //Load first memory array cell into the instruction specifier
													//Increment program counter

	while (reg.instrSpecifier.full != 0)        //While instruction specifier is not STOP
	{
		if (reg.instrSpecifier.logArith.instr4 > 4) //If instruction is not unary
		{
			reg.operSpecifier.byteL = mainMem[reg.pC++];     //Load operand specifier first byte
			reg.operSpecifier.byteR = mainMem[reg.pC++];     //Load operand specifier second byte
			updateOperand(mainMem, reg, reg.instrSpecifier.ioTraps.aaa);    //Assign appropriate value to operand
		}

		switch (reg.instrSpecifier.logArith.instr4)    //Check first nibble to extract instruction code
		{
		case 1:
			switch (reg.instrSpecifier.unary.instr7)    //Unary no operand
			{
			case 12: //Bitwise invert
				if (reg.instrSpecifier.unary.r == 0)
					reg.rA16 = reg.rA16 ^ ((1 << 16) - 1); //is there a shorter way to write this?
				else
					reg.rX16 = reg.rX16 ^ ((1 << 16) - 1); //is there a shorter way to write this?
				break;
			case 14:
				if (reg.instrSpecifier.unary.r == 0)
					reg.rA16 <<= 1;    //Arithmetic shift left Accumulator
				else
					reg.rX16 <<= 1;    //Arithmetic shift left Index
				break;
			case 15:
				if (reg.instrSpecifier.unary.r == 0)
					reg.rA16 >>= 1;    //Arithmetic shift right Accumulator
				else
					reg.rX16 >>= 1;     //Arithmetic shift right Index
				break;
			default: cout << "Error: Case 1";
			}
			break;

		case 2: //unary no operand
			switch (reg.instrSpecifier.unary.instr7)
			{
			case 16: //Rotate left
				if (reg.instrSpecifier.unary.r == 0)
					reg.rA16 = (reg.rA16 << 1) + (reg.rA16 >> 15);
				else
					reg.rX16 = (reg.rX16 << 1) + (reg.rX16 >> 15);
				break;
			case 17: //Rotate right
				if (reg.instrSpecifier.unary.r == 0)
					reg.rA16 = (reg.rA16 >> 1) + (reg.rA16 << 15);
				else
					reg.rX16 = (reg.rX16 >> 1) + (reg.rX16 << 15);
				break;
			default: cout << "Error: Case 2";

			}
			break;

		case 3:
			switch (reg.instrSpecifier.ioTraps.instr5)
			{
			case 6:    	//Decimal input
				//int t;	//why can't 'cin' take in unsigned 16-bit int?
				//cin >> t; //NEED TO ADJUST FOR ASCII CHARACTER
				tempValue.full = 124;						//assign user input to temp value
				tempAddr.byteL = mainMem[reg.pC++];		//assign left byte of address to temp address
				tempAddr.byteR = mainMem[reg.pC++];		//assign right byte of address to temp address
				mainMem[tempAddr.full] = tempValue.byteL;
				mainMem[tempAddr.full + 1] = tempValue.byteR;
				tempValue.full = 0;		//reset temporary value
				tempAddr.full = 0;		//reset temporary address
				break;
			case 7:		//Decimal output
				if (reg.instrSpecifier.ioTraps.aaa == 0)
				{
					tempValue.byteL = mainMem[reg.pC++];
					tempValue.byteR = mainMem[reg.pC++];
					cout << tempValue.full;
					tempValue.full = 0;
				}
				else
				{
					tempAddr.byteL = mainMem[reg.pC++];
					tempAddr.byteR = mainMem[reg.pC++];

					tempValue.byteL = mainMem[tempAddr.full];
					tempValue.byteR = mainMem[tempAddr.full + 1];

					cout << tempValue.full;
					tempValue.full = 0;
					tempAddr.full = 0;
				}
				break;
			default: cout << "Error: Case 3";
			}
			break;

		case 4: 		//Character input
			reg.operSpecifier.byteL = mainMem[reg.pC++];
			reg.operSpecifier.byteR = mainMem[reg.pC++];
			//int t;	//why can't 'cin' take in unsigned 16-bit int?
			//cin >> t; //NEED TO ADJUST FOR ASCII CHARACTER
			reg.operand.full = 56;
			mainMem[reg.operSpecifier.full] = reg.operand.byteL;
			mainMem[reg.operSpecifier.full + 1] = reg.operand.byteR;
			break;



		case 5:		//Character output
			cout << reg.operand.full; //NEED TO ADJUST FOR CHARACTER OUTPUT
			break;
			//no case 6
		case 7:
			if (reg.instrSpecifier.logArith.r == 0)    //Add to r
				reg.rA16 += reg.operand.full;
			else
				reg.rX16 += reg.operand.full;
			break;
		case 8:
			if (reg.instrSpecifier.logArith.r == 0)    //Subtract from r
				reg.rA16 -= reg.operand.full;
			else
				reg.rX16 -= reg.operand.full;
			break;
		case 9: 
			if (reg.instrSpecifier.logArith.r == 0)
				reg.rA16 = reg.rA16 & reg.operand.full;    //Bitwise AND
			else
				reg.rX16 = reg.rX16 & reg.operand.full;
			break;
		case 10:
			if (reg.instrSpecifier.logArith.r == 0)
				reg.rA16 = reg.rA16 | reg.operand.full;    //Bitwise OR
			else
				reg.rX16 = reg.rX16 | reg.operand.full;
			break;
			//no case 11
		case 12:
			if (reg.instrSpecifier.logArith.r == 0)    //Load r from memory
				reg.rA16 = reg.operand.full;
			else
				reg.rX16 = reg.operand.full;
			break;
		case 13: 
			if (reg.instrSpecifier.logArith.r == 0)    //Load byte from memory
				reg.rA8R = reg.operand.byteR;
			else
				reg.rX8R = reg.operand.byteR;
			break;
		case 14: 
			if (reg.instrSpecifier.logArith.r == 0)    //Store r to memory
			{
				reg.operand.full = reg.rA16;
				mainMem[reg.operSpecifier.full] = reg.rA8L;
				mainMem[reg.operSpecifier.full + 1] = reg.rA8R;
			}	
			else
			{
				reg.operand.full = reg.rX16;
				mainMem[reg.operSpecifier.full] = reg.rX8L;
				mainMem[reg.operSpecifier.full + 1] = reg.rX8R;
			}
			break;
		case 15:
			if (reg.instrSpecifier.logArith.r == 0)    //Store byte to memory
			{
				reg.operand.full = reg.rA8R;
				mainMem[reg.operSpecifier.full] = reg.rA8R;
			}
			else
			{
				reg.operand.full = reg.rX8R;
				mainMem[reg.operSpecifier.full] = reg.rX8R;
			}
			break;
		}
				/** Display register values to the screen or output file (call function to do this) */
			cout <<"IS: "<< reg.instrSpecifier.full << " OS: " << reg.operSpecifier.full << " OP:  " << reg.operand.full << " PC: " << reg.pC << " A: " << reg.rA16 << " X: " << reg.rX16 << " SP: " << reg.sP;
			reg.instrSpecifier.full = mainMem[reg.pC++];
		}
		//clear operand & operand specifier display
		return 0;
	}
