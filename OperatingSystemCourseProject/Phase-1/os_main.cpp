// Group - 10 - Phase 1.

// Assumptions -
// 1) Jobs entered without error in input file.
// 2) No Physical Separation between Jobs.
// 3) Job outputs separated in output file by two blank lines.
// 4) Program loadead in memory starting at location 00.
// 5) No Multiprogramming, load and run one program at a time.
// 6) Service Interrupt(SI) for service request.

#include <iostream>
#include <string>
#include <fstream>
#include <regex>
#include <stdint.h>
using namespace std;

#define emp '*'

#define WORD_LIMIT 100
#define WORD_SIZE 4
#define INSTRUCTION_SIZE 4
#define BUFFER_SIZE 40
typedef uint16_t u16;

// Memory Structure..
class OS{
    public:
    char memory[WORD_LIMIT][WORD_SIZE]; // Main Memory.
    char IR[INSTRUCTION_SIZE]; // Instruction Register
    char R[WORD_SIZE]; // General Purpose Register.
    char buffer[BUFFER_SIZE]; // Memory space, instruction register, general purpose register
    // and buffer memory.
    u16 IC; // Instruction Counter.
    bool C; // Toggle Bit.
    u16 SI; // Service Interrupt

    public : 
    OS(ifstream* i, ofstream *o);
    void load(); // Loading the jobs.
    void init(); // Initialising the Data Structures.
    void startExecution(); // Starting the Program Execution.
    void ExecuteUserProgram(); // Instruction Execution.
    void mos();// Master Mode.
    void read(); // reading from input file.
    void write(); // writing the output file.
    void terminate(); // terminating the current process.

    // Getters and Setters.- Let's see later on what we can do.
    ifstream *input; // data type for reading a file
    ofstream *output; // data type for creating a file if not created, and writing to a file.
};

OS::OS(ifstream *i, ofstream *o){
    this->input = i;
    this->output = o;
}

void OS::init(){
    // Implement the initialisation of the memory for the Operating System.
    memset(memory, emp, WORD_LIMIT * WORD_SIZE);
    memset(IR, emp, INSTRUCTION_SIZE);
    memset(R, emp, WORD_SIZE);
    memset(buffer, emp, BUFFER_SIZE);
    // IC = 0; We will set it to 0 when we are at MOS/StartExecution Function.
    C = false;
    SI = 3;
}

void OS::startExecution(){
    IC = 0;
    ExecuteUserProgram();
}

void OS::read(){
    (*input).getline(buffer, 41);
    // cout << buffer << endl;
    int loc = (IR[2] - '0')*10 + (IR[3] - '0');
    memcpy(&memory[loc][0], buffer, 40);
    SI = 3;
}

void OS::write(){
    int loc = (IR[2] - '0')*10 + (IR[3] - '0');
    char line[40];
    memcpy(line, &memory[loc][0], 40);
    (*output) << line << endl;
    SI = 3;
}

void OS::terminate(){
    (*output) << "\n\n";
}

void OS::mos(){
    if(SI == 1){
        read();
    }
    else if(SI == 2){
        write();
    }
    else if(SI == 3){
        terminate();
    }
}

void OS::ExecuteUserProgram(){
    while(true){
        // Accessing the Current Instruction.
        for(int i = 0;i < 4;i++){
            IR[i] = memory[IC][i];
        }
        // You can use memcpy instead as well.
        // Incrementing the IC.
        IC++;
        // Operand
        int loc = (IR[2] - '0') * 10 + (IR[3] - '0');
        // Examining the IR[1,2]..
        if(IR[0] == 'G' && IR[1] == 'D'){
            SI = 1;
            mos();
        }
        else if(IR[0] == 'P' && IR[1] == 'D'){
            SI = 2;
            mos();
        }
        else if(IR[0] == 'H'){
            SI = 3;
            mos();
            break;
        }
        else if(IR[0] == 'L' && IR[1] == 'R'){
            // loading the data from memory[loc][i] to R.
            memcpy(R, &memory[loc][0], 4);
        }
        else if(IR[0] == 'S' && IR[1] == 'R'){
            memcpy(&memory[loc][0], R, 4);
        }
        else if(IR[0] == 'C' && IR[1] == 'R'){
            bool match = true;
            for(int i = 0;i < 4;i++){
                if(memory[loc][i] != R[i]){
                    match = false;
                }
            }
            if(match) C = true;
            else C = false; 
        }
        else if(IR[0] == 'B' && IR[1] == 'T'){
            if(C){
                IC = loc;
            }
        }
    }
}

void check(char *buffer){
    for(int i = 0;i < 37;i++){
        if(buffer[i] == '\0'){
            buffer[i] = emp;
        }
        if(buffer[i] == 'H'){
            for(int j = 36;j > i;j--){
                buffer[j + 3] = buffer[j];
            }
            buffer[i + 1] = '0';
            buffer[i + 2] = '0';
            buffer[i + 3] = '0'; 
        }
    }
}

void OS::load(){
    cout << "Starting to Load Data : " << endl;
   int currline = 0;
   do{
        // cout << buffer << endl;
        (*input).getline(buffer,41);
        // cout << buffer << endl;
        if(buffer[0] == '$' && buffer[1] == 'A' && buffer[2] == 'M' && buffer[3] == 'J'){
            // cout << "Control Card" << endl;
            init(); 
        }
        else if(buffer[0] == '$' && buffer[1] == 'D' && buffer[2] == 'T' && buffer[3] == 'A'){
            // cout << "Data Card" << endl;
            startExecution();
        }
        else if(buffer[0] == '$' && buffer[1] == 'E' && buffer[2] == 'N' && buffer[3] == 'D'){
            // cout << "End" << endl;
            currline = 0;
            continue;
        }
        else{
            // It's a program Card.
            if(currline >= 100){
                cout << "Memory Limit Exceeded !!!\n" << endl;
                cout << "Program quitting abruptly" << endl;
                break;
            }
            // cout << "Yaayyy!! It's a Program Card" << endl;
            // Check for Halt
            check(buffer);
            memcpy(&memory[currline][0], buffer, 40);
            // for(int i = currline;i < currline + 10;i++){
            //     for(int j = 0;j < 4;j++){
            //         memory[]
            //     }
            // }
            // Void Print Memory.
            // for(int i = 0;i < 100;i++){
            //     for(int j = 0;j < 4;j++){
            //         cout << memory[i][j] << " ";
            //     }
            //     cout << endl;
            // }
            currline += 10;
        }
        memset(buffer, emp, BUFFER_SIZE);
   }while(!(*input).eof());
    
}

int main(){
    // Connection to Input and Output Files.
    ifstream i("./job_structure.txt");
    ofstream o("./output.txt");

    // Creation of the operating System.
    OS os(&i, &o);

    // Loading the Operating System..
    if(!(os.input)){
        cout << "Input File Doesn't Exist" << endl;
    }
    else{
        cout << "Starting the Processor : " << endl;
    }
    os.load();
    // Void Print Memory.
    // for(int i = 0;i < 100;i++){
    //     for(int j = 0;j < 4;j++){
    //         cout << os.memory[i][j] << " ";
    //     }
    //     cout << endl;
    // }
    cout << "Jobs Completed!!" << endl;
    return 0;
}