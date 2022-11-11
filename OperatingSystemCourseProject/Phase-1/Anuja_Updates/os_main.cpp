// Group - 10 - Phase 1.

// Assumptions -
// 1) Jobs entered without error in input file.
// 2) No Physical Separation between Jobs.
// 3) Job outputs separated in output file by two blank lines.
// 4) Program loaded in memory starting at location 00.
// 5) No Multiprogramming, load and run one program at a time.
// 6) Service Interrupt(SI) for service request.

#include <iostream>
#include <string>
#include <fstream>
#include <regex>
using namespace std;
#define empty '\0'

#define WORD_LIMIT 100
#define WORD_SIZE 4
#define INSTRUCTION_SIZE 4
#define BUFFER_SIZE 40
typedef u_int16 u16;

// Memory Structure..
struct OS{
    char memory[WORD_LIMIT][WORD_SIZE], IR[INSTRUCTION_SIZE], R[WORD_SIZE], buffer[BUFFER_SIZE]; // Memory space, instruction register, general purpose register
    // and buffer memory.
    int IC; // Instruction Counter.
    bool C; // Toggle Bit.
    int SI; // Service Interrupt
    ifstream input; // data type for reading a file
    ofstream output; // data type for creating a file if not created, and writing to a file.
}typedef OS;

void init(OS &os){
    // Implement the initialisation of the memory for the Operating System.


}

void load(OS &os){
    string line;
    int number = 0;
    while(getline(os.input, os.buffer)){
        // Checking if a control card..
        if(line.substr(0, 4) == "$AMJ"){
            init(os);

        }
        else if(line.substr(0, 4) == "$")
    }
    
}

int main(){
    // Creation of the operating System.
    OS os;
    // Attaching the job structure to the Operating System.
    os.input.open("./job_structure.txt");
    os.output.open("./output.txt", ios::app);
    // Loading the Operating System..
    load(os);

    return 0;
}



// void hello(ifstream* read, ofstream* write, int number){
//     cout << "Hello World" << endl;
//     string line;
//     (*read).seekg(number - 2);
//     getline(*read, line);
//     (*write) << line << endl;
// }

// while(getline(os.input, line)){
//         // cout << number << endl;
//         if(regex_match(line, regex("(\\$AMJ)(.*)"))){
//             // continue;
//         }
//         else if(regex_match(line, regex("(\\$DTA)(.*)"))){
//             // int number = read.tellg();
//             hello(&read, &write, number);
//         }
//         else{
//             // continue;
//         }
//         number = read.tellg();
//     }
//     cout << endl;