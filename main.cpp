#include <iostream>
#include <fstream>
using namespace std;

struct registers {
	union {
		struct {
			unsigned int lsd8 : 8;    
			unsigned int msd8 : 8;
		};
		unsigned int full : 16;    //Accumulator
	} rA;
	union {
		struct {
			unsigned int lsd8 : 8;    //Index divided
			unsigned int msd8 : 8;
		};
		unsigned int full : 16;    //Index
	} rX;
	
	unsigned int sP : 16;    //Stack Pointer
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
			unsigned int instr4 : 4;    //Also used in switch statement to identify instruction number
		} arith;    //For Arithmetic instructions (0000 raaa)
		unsigned int full : 8;    //If fullIS == 00 then stop reading instructions
	} iSpec;

	union {    //Operand Specifier
		struct {
			unsigned int lsd8 : 8;
			unsigned int msd8 : 8;
		};    //Used when loading bytes from memory
		unsigned int full : 16;    //**DELETE IF UNUSED?
	} oSpec;

	union {
		struct {
			unsigned int lsd8 : 8;    //Operand divided
			unsigned int msd8 : 8;
		};
		unsigned int full : 16;    //Operand
	} operand;
};

union tempUnit {
	struct {
		unsigned int lsd8 : 8;
		unsigned int msd8 : 8;
	};
	unsigned int full : 16;
} tempValue, tempAddr;

void printRegisters(registers rgstr)
{
	cout<<"Current Registers [ \n";
	cout<<"Program Counter: "<< rgstr.pC;
	cout<<"\nAccumulator: "<<rgstr.rA.full;
	cout<<"\nIndex: "<< rgstr.rX.full;
	cout<<"\nStack Pointer: " << rgstr.sP;		//How are we updating SP. I don't see a change in PEP/8.
	cout<<"\nInstruction Specifier: "<<rgstr.iSPec.full;
	cout<<"\nOperand Specifier: "<<rgstr.oSpec.full;
	cout<<"\nOperand: "<< rgstr.operand.full<<" ]\n\n"; 
}
int main() {
	ifstream readFile;               
	readFile.open("text.txt", ios::in);       
	unsigned int mainMem[50];            
	unsigned int tempHex;                
	int count = 0;                    

	while (readFile >> hex >> tempHex)         //Load 1-byte from input file to variable
	{
		mainMem[count++] = tempHex;        
	}

	registers reg;         //Declare a set of new registers
	reg.pC = 0;            //Initiate program counter to 0
	reg.iSpec.full = mainMem[reg.pC++];    //Load first memory array cell into the instruction specifier
													
	while (reg.iSpec.full != 0)        
	{
		if (reg.iSpec.arith.instr4 > 3) //Update operand specifier for I/O and Arithmetic instructions
		{
			reg.oSpec.msd8 = mainMem[reg.pC++];     
			reg.oSpec.lsd8 = mainMem[reg.pC++];    
			if ((reg.iSpec.arith.instr4 > 4) && (reg.iSpec.arith.instr4 < 14))  //Update operand for those using immediate and direct addressing modes
			{
				if (reg.iSpec.arith.aaa3 == 0)
					reg.operand.full = reg.oSpec.full;
				else
					reg.operand.full = mainMem[reg.oSpec.full];
			}
		}
			
		switch (reg.iSpec.logArith.instr4)    //Check first nibble to extract instruction code
		{
			case 1:
				switch (reg.iSpec.unary.instr7)    //Unary no operand
				{
					case 12: //Bitwise invert
						if (reg.iSpec.unary.r1 == 0)
							reg.rA.full = reg.rA.full ^ ((1 << 16) - 1); //is there a shorter way to write this?
						else
							reg.rX.full = reg.rX.full ^ ((1 << 16) - 1); //is there a shorter way to write this?
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
				switch (reg.iSpec.unary.instr7) //Unary no operand
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
					case 6:	//Decimal input
				//why can't 'cin' take in unsigned 16-bit int?
				//NEED TO ADJUST FOR ASCII CHARACTER
						int decIn;
						cin>>decIn;
						tempValue.full = decIn;						
						tempAddr.msd8 = mainMem[reg.pC++];		
						tempAddr.lsd8 = mainMem[reg.pC++];		
						mainMem[tempAddr.full] = tempValue.msd8;
						mainMem[tempAddr.full + 1] = tempValue.lsd8;
						tempValue.full = 0;		//reset temporary value
						tempAddr.full = 0;		//reset temporary address
						break;
					case 7:		//Decimal output
						if (reg.iSpec.ioTrap.aaa3 == 0)
						{
							tempValue.msd8 = mainMem[reg.pC++];
							tempValue.lsd8 = mainMem[reg.pC++];
							cout << tempValue.full;
							tempValue.full = 0;
						}
						else
						{
							tempAddr.msd8 = mainMem[reg.pC++];
							tempAddr.lsd8 = mainMem[reg.pC++];
							tempValue.msd8 = mainMem[tempAddr.full];
							tempValue.lsd8 = mainMem[tempAddr.full + 1];
							cout << tempValue.full;
							tempValue.full = 0;
							tempAddr.full = 0;
						}
						break;
					default: cout << "Error: Case 3";
				}
				break;

			case 4:		//Character input
				reg.operand.full = mainMem[reg.oSpec.full]; //only 1-byte?
				break;
				
			case 5:		//Character output
				cout << reg.operand.full; //NEED TO ADJUST FOR CHARACTER OUTPUT
				break;
			//no case 6
			case 7:		//Add to register
				if (reg.iSpec.arith.r1 == 0)    
					reg.rA.full += reg.operand.full;
				else
					reg.rX.full += reg.operand.full;
				break;
			case 8:		//Subtract from register
				if (reg.iSpec.arith.r1 == 0)    
					reg.rA.full -= reg.operand.full;
				else
					reg.rX.full -= reg.operand.full;
				break;
			case 9: 	//Bitwise AND
				if (reg.iSpec.arith.r1 == 0)
					reg.rA.full &= reg.operand.full;    
				else
					reg.rX.full &= reg.operand.full;
				break;
			case 10:	//Bitwise OR
				if (reg.iSpec.arith.r1 == 0)
					reg.rA.full |= reg.operand.full;    
				else
					reg.rX.full |= reg.operand.full;
				break;
			//no case 11
			case 12:	//Load register from memory
				if (reg.iSpec.arith.r1 == 0)    
					reg.rA.full = reg.operand.full;
				else
					reg.rX.full = reg.operand.full;
				break;
			case 13: 	//Load byte from memory
				if (reg.iSpec.arith.r1 == 0)   
					reg.rA.lsd8 = reg.operand.lsd8;
				else
					reg.rX.lsd8 = reg.operand.lsd8;
				break;
			case 14: 	//Store register to memory
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
			case 15:	//Store byte register to memory
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
