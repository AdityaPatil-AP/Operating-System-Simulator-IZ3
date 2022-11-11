#include <cstdlib>
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

class cpu;

class pcb
{
    friend class cpu;

private:
    int jobID, TTC, LLC, TTL, TLL;

public:
    void init(string);
    void displayPCB();
};

class cpu
{
private:
    byte m[100][4];
    byte IR[4];
    u16 IC;
    byte R[4];
    bool C;
    char buffer[40];

    int SI;
    int PTrow;

    int VA, RA, EM;
    int PTR[4];

public:
    void init();
    void masterMode(int, int);
    void read(int);
    void write(int);
    void halt();
    void executeUserProgram();
    void loadRegister(int);
    void storeRegister(int);
    void compareRegister(int);
    void branch(int);
    void startExecution();
    void load();
    void bufferReset();
    void printMemory();
    string replaceString(string, const std::string &, const string &);

    int allocate();
    bool checkIfMemoryAvailable(int);
    void setPTR(int);
    void pageTableInit();
    void updatePT(int);

    ifstream inFile;
    ofstream outFile;
};

void pcb ::init(string s)
{
    TTC = LLC = 0;
    jobID = stoi(s.substr(4, 4));
    TTL = stoi(s.substr(8, 4));
    TLL = stoi(s.substr(12, 4));
}

void pcb ::displayPCB()
{
    cout << "Job id is: " << jobID << endl;
    cout << "TTC is: " << TTC << endl;
    cout << "LLC is: " << LLC << endl;
    cout << "TTL is: " << TTL << endl;
    cout << "TLL is: " << TLL << endl;
}

void cpu ::printMemory()
{
    // print memory to check if enterred correctly
    for (int a = 0; a < 100; a++)
    {
        for (int b = 0; b < 4; b++)
        {
            cout << "m[" << a << "][" << b << "]: " << m[a][b] << " ";
        }
        cout << endl;
    }
}

void cpu ::bufferReset()
{
    memset(buffer, EMPTY, sizeof(buffer));
}

string cpu ::replaceString(string subject, const std::string &search, const string &replace)
{
    ulong pos = 0;
    while ((pos = subject.find(search, pos)) != string::npos)
    {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
    return subject;
}

void cpu::init()
{

    bufferReset();
    memset(m, EMPTY, sizeof(m));
    memset(IR, EMPTY, sizeof(IR));
    memset(R, EMPTY, sizeof(R));
    memset(PTR, EMPTY, sizeof(PTR));
    IC = -12;
    C = 0;
    SI = 0;
    PTrow = 0;
}

void cpu ::read(int address)
{
    bufferReset();
    if (address % 10 != 0)
    {
        return;
    }
    inFile.getline(buffer, 41);
    if (buffer[0] == '$' && buffer[1] == 'E' && buffer[2] == 'N' && buffer[3] == 'D')
    {
        return;
    }

    memcpy(m[address], buffer, sizeof(buffer));

    SI = 0;
}

void cpu ::write(int address)
{
    bufferReset();
    memcpy(buffer, m[address], 41);
    outFile << buffer << endl;

    SI = 0;
}

void cpu ::halt()
{
    cout << "Halting current program." << endl;
    outFile << endl
            << endl;
}

void cpu::masterMode(int SI, int address)
{

    switch (SI)
    {
    case 1:
    {

        read(address);

        break;
    }

    case 2:
    {

        write(address);

        break;
    }

    case 3:
    {
        halt();
        break;
    }
    }
}

void cpu ::storeRegister(int address)
{
    memcpy(m[address], R, 4);
}

void cpu ::loadRegister(int address)
{
    memcpy(R, m[address], 4);
}

void cpu ::compareRegister(int address)
{
    bool equal = true;
    for (int i = 0; i < 4; ++i)
    {
        if (m[address][i] != R[i])
        {
            equal = false;
            break;
        }
    }
    C = equal;
}

void cpu ::branch(int address)
{
    if (C)
    {
        IC = address;
    }
}

void cpu ::executeUserProgram()
{

    // user Mode
    while (true)
    {
        memcpy(IR, m[IC], 4);
        IC++;

        int address = (IR[2] - '0') * 10 + (IR[3] - '0');

        if (IR[0] == 'G' && IR[1] == 'D')
        {

            SI = 1;
            masterMode(SI, address);
        }
        else if (IR[0] == 'P' && IR[1] == 'D')
        {

            SI = 2;
            masterMode(SI, address);
        }
        else if (IR[0] == 'H')
        {

            SI = 3;
            masterMode(SI, address);
            break;
        }
        else if (IR[0] == 'L' && IR[1] == 'R')
        {

            loadRegister(address);
        }
        else if (IR[0] == 'S' && IR[1] == 'R')
        {

            storeRegister(address);
        }
        else if (IR[0] == 'C' && IR[1] == 'R')
        {

            compareRegister(address);
        }
        else if (IR[0] == 'B' && IR[1] == 'T')
        {

            branch(address);
        }
    }
}

void cpu ::startExecution()
{
    IC = 0;
    executeUserProgram();
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

        bool check = checkIfMemoryAvailable(random * 10);

        if (check)
        {
            break;
        }
    }

    return (random * 10);
}

void cpu ::setPTR(int myPTR)
{

    int count = 3;

    while (myPTR != 0)
    {
        int num = myPTR % 10;

        PTR[count] = num;
        count--;

        myPTR = myPTR / 10;
    }

    //   for(int i = 0; i < 4; i++) {
    // cout << PTR[i] << " ";
    //   }
}

void cpu ::pageTableInit()
{

    int start = 0;

    for (int i = 0; i < 4; i++)
    {
        start = start * 10 + PTR[i];
    }

    for (int i = start; i < (start + 10); i++)
    {
        for (int j = 0; j < 4; j++)
        {
            m[i][j] = '*';
        }
    }

    // for(int i = start; i < (start + 10); i++) {
    // for(int j = 0; j < 4; j++) {
    //    cout << m[i][j] << ' ';
    // }

    // cout << endl;
    // }
}

void cpu ::updatePT(int addr)
{
}

void cpu ::load()
{

    // LOAD
    string buff;

    while (getline(inFile, buff))
    {

        if (buff.length() > 40)
        {
            cout << "Invalid Card." << endl;
            break;
        }

        // Check if Control Card AMJ
        if (buff.substr(0, 4) == "$AMJ")
        {

            init();

            // create and initialise PCB
            pcb PCB;
            PCB.init(buff);
            // PCB.displayPCB();
            //  break;

            // allocate frame for Page Table and set PTR
            setPTR(allocate());

            // Initialise the Page Table
            pageTableInit();
            // break;
        }

        // Check if Control Card DTA
        else if (buff.substr(0, 4) == "$DTA")
        {

            startExecution();
        }

        // Check if Control Card END
        else if (buff.substr(0, 4) == "$END")
        {

            init();
        }

        // Else it's a program card
        else
        {
            buff = replaceString(buff, "H", "H000");

            int addr = allocate();
            // updatePT(addr);

            // copy buffer contents to memory
            memcpy(m[addr], buff.c_str(), 40);
            printMemory();

            bufferReset();
        }
    }
}

int main()
{

    cpu myCPU;

    myCPU.inFile.open("input.txt");
    myCPU.outFile.open("output.txt");

    myCPU.load();

    myCPU.inFile.close();
    myCPU.outFile.close();

    return 0;
}