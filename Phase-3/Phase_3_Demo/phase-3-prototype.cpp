#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string.h>
#include <vector>
#include <queue>
#include <ctime>

using namespace std;

#define EMPTY '\0'
typedef uint8_t byte;
typedef uint16_t u16;
typedef size_t ulong;

const int timeSlice = 10;
int IOI;

class secondaryStorage
{
public:
    byte aux[500][4];
};

class supervisoryStorage
{
public:
    queue<vector<char>> efbq;
    queue<vector<char>> ifbq;
    queue<vector<char>> ofbq;

    void efbqInit();
};

void supervisoryStorage::efbqInit()
{
    vector<char> buffer(40, '\0');
    for (int i = 0; i < 10; i++)
    {
        efbq.push(buffer);
    }
}

secondaryStorage drum;
supervisoryStorage ss;

class channel1
{
public:
    bool ch1F = false;
    int ch1T = 0;
    const int ch1TT = 5;
};

class channel2
{
public:
    bool ch2F = false;
    int ch2T = 0;
    const int ch2TT = 5;
};

class channel3
{
public:
    bool ch3F = false;
    int ch3T = 0;
    const int ch3TT = 2;
};

channel1 c1;
channel2 c2;
channel3 c3;

class pcb
{
public:
    int jobID, TTC, LLC, TTL, TLL;
    void pcb_init(string);
    void displayPCB();

    char F;
    int no_pc, no_dc, no_ol;
    int p_track, d_track, o_track;
};

void pcb ::pcb_init(string s)
{
    TTC = LLC = 0;
    jobID = stoi(s.substr(4, 4));
    TTL = stoi(s.substr(8, 4));
    TLL = stoi(s.substr(12, 4));
    F = EMPTY;
    no_ol = no_pc = no_dc = 0;
    p_track = d_track = o_track = -1;
}

class cpu
{

private:
    byte m[300][4];                // Main Memory
    byte IR[4];                    // Instruction Register
    u16 IC;                        // Instruction Counter
    byte R[4];                     // General Purpose Register
    bool C;                        // Toggle Bit
    char buffer[40];               // buffer
    map<int, string> errorMessage; // Error Message.

    int SI, PI, TI; // Interrupts
    int PTrow;      // Ptr to Currently available row in Page Table

    int PTR[4];            // Page Table Register
    vector<int> addresses; // vector of allocated addresses

    bool hRead; // Check if Halt read
    bool error; // check if error, to continue traversing file to get next job

    queue<pcb> LQ;
    queue<pcb> RQ;
    queue<pcb> IOQ;
    queue<pcb> TQ;

    vector<pcb> PCBlist;

public:
    void init();          // initialise cpu
    void masterMode(int); // MasterMode function
    // void read(int);       // read from file
    // void write(int);      // write to file
    // void terminate(vector<int>);
    // void executeUserProgram();
    // void loadRegister(int);
    // void storeRegister(int);
    // void compareRegister(int);
    // void branch(int);
    // void startExecution();
    void load();
    // void bufferReset();                                              // reset buffer to EMPTY
    void printMemory(); // displays Memory
    // string replaceString(string, const std::string&, const string&); // replace 'H' by 'H000'

    // void displayProgramStatus();      // print all register, interrupt values.
    int allocate();                   // allocate a page from 0 to 29
    bool checkIfMemoryAvailable(int); // check if that block of memory is EMPTY
    void setPTR(int);                 // set Page Table Register with the inputted address
    void pageTableInit();             // initialise Page Table  '*'
    // void updatePT(int);               // update Page Table with newly allocated page
    // int addressMap(int);              // convert VA to RA
    // bool checkForPage(int);           // check if the logical page exists in real Memory
    // void checkTime();                 // check whether time has exceeded program limits
    int getStartAddr();
    // int readPTrow(int);
    void EMinit();
    ifstream inFile;
    ofstream outFile;

    void simulation();
    void IR1();
    void IR3();
    void IR2();
    void ch3IR(int);
};

void cpu ::printMemory()
{
    cout << "Memory : " << endl;
    for (int i = 0; i < 300; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            cout << m[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

bool cpu ::checkIfMemoryAvailable(int start)
{
    for (int i = start; i < (start + 10); i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (m[i][j] != EMPTY)
            {
                return false;
            }
        }
    }
    return true;
}

int cpu ::allocate()
{
    int random;
    while (true)
    {
        random = (rand() % 30);
        // get random int from 0 to 29

        bool check = checkIfMemoryAvailable(random * 10);
        // check if the block is available

        if (check)
        {
            break;
            // break while loop if available
        }
    }

    addresses.push_back(random); // add the allocated page to addresses vector
    return random;
}

int cpu ::getStartAddr()
{
    // read PTR value
    int start = 0;
    for (int i = 0; i < 4; i++)
    {
        start = PTR[i] + start * 10;
    }
    return start;
}

void cpu ::pageTableInit()
{
    cout << "Inside Page Table Init : " << endl;
    for (int i = getStartAddr(); i < (getStartAddr() + 10); i++)
    {
        for (int j = 0; j < 4; j++)
        {
            m[i][j] = '*';
        }
        cout << endl;
    }
}

void cpu ::setPTR(int myPTR)
{

    int count = 3;

    // peel of individual digits and store in PTR array
    while (myPTR != 0)
    {
        int num = myPTR % 10;

        PTR[count] = num;
        count--;

        myPTR = myPTR / 10;
    }
}

void cpu::init()
{

    memset(m, EMPTY, sizeof(m));
    memset(IR, EMPTY, sizeof(IR));
    memset(R, EMPTY, sizeof(R));
    memset(PTR, EMPTY, sizeof(PTR));
    IC = -12;
    C = 0;
    SI = 3;
    PI = 0;
    TI = 0;
    PTrow = -1;
    EMinit();
    // error = false;
    // hRead = false;
}

void cpu::EMinit()
{
    errorMessage[0] = "No error";
    errorMessage[1] = "Out of Data";
    errorMessage[2] = "Line Limit Exceeded";
    errorMessage[3] = "Time Limit Exceeded";
    errorMessage[4] = "Operation Code Error";
    errorMessage[5] = "Operand Error";
    errorMessage[6] = "Invalid Page Fault";
}

void cpu::IR1()
{

    IOI--;
    c1.ch1T = 0;

    string str;

    if (getline(inFile, str))
    {

        if (str.length() > 41)
        {
            cout << "Length of Card exceeds 40 !!" << endl;
            // add code to continue parsing till next correct job.
            //  error = true;
        }
        // cout << str << endl;
        vector<char> buffer = ss.efbq.front();
        ss.efbq.pop();

        for (int i = 0; i < str.length(); i++)
        {
            buffer[i] = str[i];
        }

        ss.ifbq.push(buffer);

        // vector<char> demo = ss.ifbq.front();
        // cout << demo[0] << endl;
        c3.ch3F = true;
        c3.ch3T = 0;
        //      for(int i = 0; i<buffer.size(); i++){
        //     cout<<buffer[i]<<" ";

        // }

        c1.ch1F = true;
    }
    else
    {
        c1.ch1F = false;
    }
}

void cpu::ch3IR(int caseOfIR3)
{
    vector<char> buffer = ss.ifbq.front();
    cout << "Inside ch3IR" << endl;

    if (buffer[0] == '$' && buffer[1] == 'A' && buffer[2] == 'M' && buffer[3] == 'J')
    {
        init();

        pcb PCB;
        PCBlist.push_back(PCB);

        (PCBlist.back()).F = 'P';

        string buff(buffer.begin(), buffer.end());
        (PCBlist.back()).pcb_init(buff);

        int ptAddr = allocate() * 10;
        setPTR(ptAddr);
        PTrow = ptAddr;
        cout << ptAddr << endl;
        pageTableInit();
        printMemory();
    }
    else if (buffer[0] == '$' && buffer[1] == 'D' && buffer[2] == 'T' && buffer[3] == 'A')
    {
        (PCBlist.back()).F = 'D';
    }

    // Check if Control Card END
    else if (buffer[0] == '$' && buffer[1] == 'E' && buffer[2] == 'N' && buffer[3] == 'D')
    {

        // init();
    }

    // 	// Else it's a program card
    // 	else {
    // 	    buff = replaceString(buff, "H", "H000");

    // 	    int addr = allocate();
    // 	    updatePT(addr);

    // 	    // copy buffer contents to memory
    // 	    memcpy(m[(addr * 10)], buff.c_str(), buff.length());

    // 	    bufferReset();
    // 	}

    ss.ifbq.pop();
    std::fill(buffer.begin(), buffer.end(), '\0');
    ss.efbq.push(buffer);
}

void cpu::IR3()
{
    IOI = IOI - 4;
    c3.ch3T = 0;

    int caseOfIR3;

    if (!((ss.ifbq).empty()))
    {
        caseOfIR3 = 1;
        ch3IR(caseOfIR3);
    }
}

void cpu::load()
{

    vector<char> buffer = ss.ifbq.front();

    if (buffer[0] == '$' && buffer[1] == 'A' && buffer[2] == 'M' && buffer[3] == 'J')
    {
        init();

        pcb PCB;
        PCBlist.push_back(PCB);

        // PCBlist[(PCBlist.size() - 1)].P = true;

        string buff(buffer.begin(), buffer.end());
        PCBlist[(PCBlist.size() - 1)].pcb_init(buff);

        int ptAddr = allocate() * 10;
        setPTR(ptAddr);
        PTrow = ptAddr;

        pageTableInit();
    }
    // 	else if(buff.substr(0, 4) == "$DTA") {

    // 	    startExecution();
    // 	}

    // 	// Check if Control Card END
    // 	else if(buff.substr(0, 4) == "$END") {

    // 	    init();
    // 	}

    // 	// Else it's a program card
    // 	else {
    // 	    buff = replaceString(buff, "H", "H000");

    // 	    int addr = allocate();
    // 	    updatePT(addr);

    // 	    // copy buffer contents to memory
    // 	    memcpy(m[(addr * 10)], buff.c_str(), buff.length());

    // 	    bufferReset();
    // 	}

    ss.ifbq.pop();
    std::fill(buffer.begin(), buffer.end(), '\0');
    ss.efbq.push(buffer);
}

void cpu::IR2()
{
    // tested and working
    IOI = IOI - 2;
    c2.ch2T = 0;

    vector<char> buffer = ss.ofbq.front();

    for (int i = 0; i < buffer.size(); i++)
    {
        outFile << buffer[i];
    }

    outFile << endl;

    ss.ofbq.pop();
    std::fill(buffer.begin(), buffer.end(), '\0');
    ss.efbq.push(buffer);

    if (!ss.ofbq.empty())
    {
        IR2();
    }
}

void cpu::masterMode(int address)
{
    // cout<<"IOI is: "<<IOI<<endl;
    switch (IOI)
    {
    case 0:
    {
        break;
    }
    case 1:
    {
        IR1();
        simulation();
        break;
    }
    case 2:
    {
        IR2();
        break;
    }
    case 4:
    {
        cout << 4 << endl;
        IR3();
        simulation();
        break;
    }
    }
}

void cpu::simulation()
{
    cout << "Inside simulation" << endl;
    while (IOI == 0)
    {
        if (c1.ch1F)
        {
            c1.ch1T++;
            if (c1.ch1T == c1.ch1TT)
            {
                IOI++;
            }
        }
        if (c2.ch2F)
        {
            c2.ch2T++;
            if (c2.ch2T == c2.ch2TT)
            {
                IOI = IOI + 2;
            }
        }
        if (c3.ch3F)
        {
            c3.ch3T++;
            if (c3.ch3T == c3.ch3TT)
            {
                IOI = IOI + 4;
            }
        }
    }
    cout << "IOI check : " << IOI << endl;
    masterMode(0);
}

int main()
{
    // struct buffer {
    //     char data[40];
    // }typedef buffer;

    // queue<buffer> efbq;
    // queue<buffer> ifbq;
    // queue<buffer> ofbq;

    // buffer mybuff;
    // mybuff.data = "abc";

    // cout<<mybuff.data<<endl;
    // cout<<ss.efbq.size()<<endl;
    // cout<<ss.ifbq.empty();
    // cout<<timeSlice<<endl;

    ss.efbqInit();
    cpu myCPU;

    myCPU.inFile.open("input.txt");
    myCPU.outFile.open("output.txt");

    if (!(myCPU.inFile))
    {

        // if input file match not found, display message

        cout << "Input File Doesn't Exist" << endl;
    }
    else
    {
        cout << "Starting OS - simulation. " << endl;
    }
    c1.ch1F = true;
    IOI = 0;
    myCPU.simulation();

    // close input, output files
    myCPU.inFile.close();
    myCPU.outFile.close();

    return 0;
}
