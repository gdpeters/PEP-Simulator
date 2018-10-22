
//add #include headers as needed

//add unions, structs, and global variables
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
		unsigned int digit4R : 4;
		unsigned int digit4L : 4;
	} loadIS;

	unsigned int fullIS : 8;
};


int main() {

/** Read file and load memory array */

/** Initiate registers, instruction specifier, operand specifier, and operand */

/** Initiate an output file if we choose this route as opposed to screen display */

/** Iterate through memory array */

    /** Load and determine instruction specifier and increment PC */
    
    /** Load operand specifier, extract register and addressing mode, update operand, and increment PC */
    
    /** Execute instruction by calling function (create subprograms when useful) */
    
    /** Display register values to the screen or output file (call function to do this) */
    
    
 /** What else are we missing here? */   
 
 
    
  return 0;
}
