
#include <cstring>
#include <fstream>
#include <iostream>
#include <math.h>
#include <string.h>
#include <typeinfo>

using namespace std;

#define EMPTY '\0'
typedef uint8_t byte;
typedef uint16_t u16;
typedef size_t ulong;
bool notTerminate = true;
int SI = -99;

struct cpu {

    byte m[100][4];
    byte IR[4];
    u16 IC;
    byte R[4];
    bool C;
    byte buffer[40];
	int mrow;
	bool instructionFlag;
};

struct cpu myCPU;

string replaceString(string subject, const std::string &search, const string &replace) {
    ulong pos = 0;
    while ((pos = subject.find(search, pos)) != string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
    return subject;
}

void init()
{
    memset(myCPU.buffer, EMPTY, sizeof(myCPU.buffer));
    memset(myCPU.m, EMPTY, sizeof(myCPU.m));
    memset(myCPU.IR, EMPTY, sizeof(myCPU.IR));
    memset(myCPU.R, EMPTY, sizeof(myCPU.R));
    myCPU.IC = -12;
    myCPU.C = 0;
	myCPU.mrow = 0;
	myCPU.instructionFlag = true;
}

void masterMode(int SI,
    int address,
    ifstream* inFile,
    ofstream* outFile,
    int infilePos,
    int outfilePos)
{

    switch(SI) {
    case 1:
	{
	string aline;
	getline(*inFile, aline);
	if(address % 10 != 0) {
	    return;
	}
	
	memset(myCPU.buffer, EMPTY, sizeof(myCPU.buffer));
	memcpy(myCPU.buffer, aline.c_str(), aline.size());
	memcpy(myCPU.m[address], myCPU.buffer, sizeof(myCPU.buffer));
		
	break;	
	}
	

    case 2:
	{
	char myline[40];
	memset(myCPU.buffer, EMPTY, sizeof(myCPU.buffer));
	memcpy(myCPU.buffer, myCPU.m[address], 40);
	memcpy(myline, myCPU.buffer, 40);
	*outFile << myline << endl;
	break;	
	}
	
    case 3:
	{
	cout << "Halting current program." << endl;
	(*outFile) << endl << endl;
	notTerminate = false;
	break;	
	}
	
    }
}

void printPattern(char c, int location){
	for(int i = 0;i < 4;i++){
		for(int j = i;j >= 0;j--){
			cout << location << " " << j <<  endl;
			myCPU.m[location][j] = '*';
		}
		location += 10;
	}
	return;
}

void executeUserProgram(ifstream* inFile, ofstream* outFile, int infilePos, int outfilePos)
{
	
    // user Mode
    while(notTerminate) {
	memcpy(myCPU.IR, myCPU.m[myCPU.IC], 4);
	myCPU.IC++;

	int address = (myCPU.IR[2] - '0') * 10 + (myCPU.IR[3] - '0');

	if(myCPU.IR[0] == 'G' && myCPU.IR[1] == 'D') {
	    
	    SI = 1;
		masterMode(SI, address, inFile, outFile, infilePos, outfilePos);
	} else if(myCPU.IR[0] == 'P' && myCPU.IR[1] == 'D') {
	    
	    SI = 2;
		masterMode(SI, address, inFile, outFile, infilePos, outfilePos);
	} else if(myCPU.IR[0] == 'H') {
	    
	    SI = 3;
		masterMode(SI, address, inFile, outFile, infilePos, outfilePos);
	} else if(myCPU.IR[0] == 'L' && myCPU.IR[1] == 'R') {
	    
	    memcpy(myCPU.R, myCPU.m[address], 4);
		if(myCPU.R[0] == '*'){
			int location = (myCPU.R[2] - '0') * 10 + (myCPU.R[3] - '0');
			char c = '*';
			printPattern(c, location);
		}

	} else if(myCPU.IR[0] == 'S' && myCPU.IR[1] == 'R') {
	    
	    memcpy(myCPU.m[address], myCPU.R, 4);

	} else if(myCPU.IR[0] == 'C' && myCPU.IR[1] == 'R') {
	   
	    bool equal = true;
	    for(int i = 0; i < 4; ++i) {
		if(myCPU.m[address][i] != myCPU.R[i]) {
		    equal = false;
		    break;
		}
	    }
	    myCPU.C = equal;

	} else if(myCPU.IR[0] == 'B' && myCPU.IR[1] == 'T') {
	    
	    if(myCPU.C) {
		myCPU.IC = address;
	    }
	}
    }
}

void startExecution(ifstream* inFile, ofstream* outFile, int infilePos, int outfilePos)
{
    myCPU.IC = 0;
    executeUserProgram(inFile, outFile, infilePos, outfilePos);
}

int main()
{

    ifstream inFile("./job_structure.txt");
    ofstream outFile("./output.txt");

    string line;

    // LOAD

    while(getline(inFile, line)) {


	// Check if Control Card AMJ
	if(line.substr(0, 4) == "$AMJ") {

	    init();
	    myCPU.instructionFlag = true;
	}

	// Check if Control Card DTA
	else if(line.substr(0, 4) == "$DTA") {

	    int infilePos = inFile.tellg();
	    int outfilePos = outFile.tellp();
	    startExecution( &inFile, &outFile, infilePos, outfilePos);
	    myCPU.instructionFlag = false;
	}

	// Check if Control Card END
	else if(line.substr(0, 4) == "$END") {

	    myCPU.instructionFlag = false;
		notTerminate = true;
	    init();
	}

	// Else it's a program card
	else if(myCPU.instructionFlag) {
		line = replaceString(line, "H", "H000");
	    if(myCPU.mrow == 99) {
		cout << "Memory exceeded! Aborting program...";
		break; // break outta while loop if memory overflow
	    } else {
		// enter the line into buffer
		memcpy(myCPU.buffer, line.c_str(), line.size());
		// copy buffer contents to memory
		memcpy(myCPU.m[myCPU.mrow], myCPU.buffer, sizeof(myCPU.buffer));
		// update the current row of the memory
		myCPU.mrow += ceil(line.size() / 4);
		memset(myCPU.buffer, EMPTY, sizeof(myCPU.buffer));
		
	    }
	}
    }
	for(int i = 0;i < 100;i++){
		for(int j = 0;j < 4;j++){
			cout << myCPU.m[i][j];
		}
		cout << endl;
	}

		
    inFile.close();
    outFile.close();

    return 0;
}