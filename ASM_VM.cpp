//Brian Scherzo
//CMSC 313
//Project 2 VM Code
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
using namespace std;


//Used to make global variables in the .data section of the program
//Keeps variable names and contents together in one object
struct GlobalVar {

	string varName;
	string varContent;  
};


//Instructions and key commands that the parser recognizes
const string MOV = "mov"; 
const string ADD = "add"; 
const string SUB = "sub"; 
const string OUT = "print"; 
const string CALL = "call"; 
const string PEEK = "peek"; 
const string PROGRAM = ".text";
const string DATA = ".data";


//Registers (all purpose, current instruction, and variable memory)
string regs[10];
vector <GlobalVar> globals;
vector <string> stack;
int RIP;


//Main functions that drive the VM
void VM();
void readFile(string filename);


//Stores each line of the program
vector <vector <string>> program;


//Begins the process of parsing 
vector<string> split(string str);


//Helper functions
void checkIns(vector<string> ins);
void mov(string dest, string source);
void add(string dest, string param);
void sub(string dest, string param);
void print(string reg);
void checkRIP();
string findGlobal(string name);


//Defining predefined functions and execution code
void execFunc(string func);
string functions[3] = { "function1", "function2", "function3" };
void func1();
void func2();
void func3();


//Determines if the parser is looking at global variables or program code
bool section = false;


//Main program
int main() {
	
	string file;
	cout << "Please enter the name of the program file\nyou would like to load (include the .txt): ";
	cin >> file;
	cout << endl;

	readFile(file);
	VM();

	return 0;
}


//Loading in the specified file given in main for parsing
void readFile(string filename) {

	//Contains the space seperated line
	vector <string> command;

	//Contains the current line being read in
	string cur;

	//Opening the file and loading into the "command" vector with validation
	ifstream programFile;
	programFile.open(filename);
	
	if (programFile.is_open()) {

		cout << "Reading file..." << endl;

		while (getline(programFile, cur)) {

			command = split(cur);
			program.push_back(command);
		}

	}
	else
		cout << "Unable to load specified file" << endl;
}


//The main function operating the VM
void VM() {

	cout << "Booting VM..." << endl;
	cout << "Priming registers...\n" << endl;

	for (int k = 0; k < 10; k++) {

		regs[k] = "";
	}

	//Looking at each line, and sending the line to the parser
	//Updating the current instruction register
	for (unsigned int i = 0; i < program.size(); i++) {
		
		if (program.at(i).size() != 0) {

			RIP = static_cast <int>(i);
			checkIns(program.at(i));
		}
	}

	cout << "VM exited successfully" << endl;
}



//Splits the string being passed for parsing
vector<string> split(string str) {

	stringstream instruction(str);
	string cur;
	vector<string> vec;

	while (getline(instruction, cur, ' ')) {

		vec.push_back(cur);
	}

	return vec;
}


//A decision tree that decides how to parse the passed instruction vector
void checkIns(vector<string> ins) {

	//Determines if the parser is looking at globals or the VM code 
	//If section = false, then the parser is reading globals
	//If section = true, then the parser is making a decision on what to do with a command
	if (ins.at(0) == PROGRAM) {
	
		section = true;
		return;
	}

	if (ins.at(0) == DATA)
		return;

	if (section == false) {
		
		GlobalVar var;
		var.varName = ins.at(0);
		var.varContent = ins.at(1);

		globals.push_back(var);
	}
	else if(section) {

		if (ins.at(0) == MOV)
			mov(ins.at(1), ins.at(2));
		else if (ins.at(0) == ADD)
			add(ins.at(1), ins.at(2));
		else if (ins.at(0) == SUB)
			sub(ins.at(1), ins.at(2));
		else if (ins.at(0) == OUT)
			print(ins.at(1));
		else if (ins.at(0) == PEEK)
			checkRIP();
		else if (ins.at(0) == CALL)
			execFunc(ins.at(1));
	}
}


//Performs the move instruction
void mov(string dest, string source) {

	string index = dest.substr(1);
	int intIndex = stoi(index);
	intIndex -= 1;

	//Seeing if there is a defined global with the name passed from source
	string check = findGlobal(source);
	
	//Moving contents of one register to another
	if (source.substr(0, 1) == "r" && source.length() == 2) {

		string index2 = source.substr(1);
		int intIndex2 = stoi(index2);
		intIndex2 -= 1;

		regs[intIndex] = regs[intIndex2];
		regs[intIndex2] = "";
	}
	//Moving contents of valid global variable to a register
	else if (check != "-1") {

		regs[intIndex] = check;
	}
	else
		regs[intIndex] = source;

	
}


//Performs the addition operation
void add(string dest, string param) {

	string index = dest.substr(1);
	int intIndex = stoi(index);
	intIndex -= 1;

	string check = findGlobal(param);

	//Adds contents of one register to another and clears the source register
	if (param.substr(0, 1) == "r") {

		string index2 = param.substr(1);
		int intIndex2 = stoi(index2);
		intIndex2 -= 1;

		int num1 = stoi(regs[intIndex]);
		int num2 = stoi(regs[intIndex2]);
		int result = num1 + num2;

		regs[intIndex] = to_string(result);
		regs[intIndex2] = "";
	}
	//Adds contents of a global variable to a register 
	else if (check != "-1") {

		int num1 = stoi(regs[intIndex]);
		int num2 = stoi(check);
		int result = num1 + num2;

		regs[intIndex] = to_string(result);
	}
	else {

		int num1 = stoi(regs[intIndex]);
		int num2 = stoi(param);
		int result = num1 + num2;

		regs[intIndex] = to_string(result);
	}
}


//Perfoms the subtraction operation
void sub(string dest, string param) {

	string index = dest.substr(1);
	int intIndex = stoi(index);
	intIndex -= 1;

	string check = findGlobal(param);

	//Subtracts contents of a register from another
	if (param.substr(0, 1) == "r") {

		string index2 = param.substr(1);
		int intIndex2 = stoi(index2);
		intIndex2 -= 1;

		int num1 = stoi(regs[intIndex]);
		int num2 = stoi(regs[intIndex2]);
		int result = num1 - num2;

		regs[intIndex] = to_string(result);
		regs[intIndex2] = "";
	}
	//Subtracts contents of a global variable from a register
	else if (check != "-1") {

		int num1 = stoi(regs[intIndex]);
		int num2 = stoi(check);
		int result = num1 - num2;

		regs[intIndex] = to_string(result);
	}
	else {

		int num1 = stoi(regs[intIndex]);
		int num2 = stoi(param);
		int result = num1 - num2;

		regs[intIndex] = to_string(result);
	}
}


//Allows the user to print contents of a specific register with bounds checking
void print(string reg) {

	string location = reg.substr(1);
	cout << endl;

	if (regs[stoi(location) - 1] == "")
		cout << "Memory in r" + location + " is unused" << endl;
	
	else
		cout << "Contents of r" + location + ": " + regs[stoi(location) - 1] << endl;

	cout << endl;
}


//Checks what the RIP register is currently looking at
void checkRIP() {

	if (RIP == 0)
		cout << "No instruction currently in memory" << endl;
	else 
		cout << "Processing instruction at line: " + to_string(RIP) << endl;
}


//Executes one of the three pre-defined functions
void execFunc(string func) {

	if (func == functions[0])
		func1();
	else if (func == functions[1])
		func2();
	else if (func == functions[2])
		func3();
}
void func1(){

	cout << "Dr. Pottegier is pretty epic" << endl;
}
void func2(){

	int num = 1 + 2;
	string localNum = to_string(num);
	stack.push_back(localNum);
	cout << "The operation: \"1+2\" is calculated and pushed to the stack. Top of the stack reads: " + stack.back() << endl;
	stack.back() = "";
}
void func3(){

	cout << "This is the 3rd predefined VM function" << endl;
}


//Goes through the vector of GlobalVars
//If the correct one exists, the contents are returned
//If the specified variable doesn't exist, a "-1"
string findGlobal(string name) {

	string temp = "-1";

	for (unsigned int cur = 0; cur < globals.size(); cur++) {

		if (name == globals.at(cur).varName)
			temp = globals.at(cur).varContent;
	}

	return temp;
}