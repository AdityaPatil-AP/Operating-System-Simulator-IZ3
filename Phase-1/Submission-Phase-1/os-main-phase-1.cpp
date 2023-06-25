// OS Phase 1

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
    int mrow;

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

    ifstream inFile;
    ofstream outFile;
};

void cpu ::printMemory()
{
    // print memory to check if enterred correctly
    for (int a = 0; a < 100; a++)
    {
        for (int b = 0; b < 4; b++)
        {
            cout << m[a][b] << " ";
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
    IC = -12;
    C = 0;
    mrow = 0;
    SI = 0;
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
    string buffer1 = "";
    for (int i = address; i < address + 10; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (m[i][j] != '\0')
                buffer1 += m[i][j];
        }
    }

    outFile << buffer1 << endl;

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
        // printMemory();
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
void cpu ::load()
{

    // LOAD
    string line;

    while (getline(inFile, line))
    {

        // Check if Control Card AMJ
        if (line.substr(0, 4) == "$AMJ")
        {

            init();
        }

        // Check if Control Card DTA
        else if (line.substr(0, 4) == "$DTA")
        {

            startExecution();
        }

        // Check if Control Card END
        else if (line.substr(0, 4) == "$END")
        {

            init();
        }

        // Else it's a program card
        else
        {
            line = replaceString(line, "H", "H000");
            if (mrow == 99)
            {
                cout << "Memory exceeded! Aborting program...";
                break; // break outta while loop if memory overflow
            }
            else
            {

                memcpy(buffer, line.c_str(), line.size());
                // copy buffer contents to memory
                memcpy(m[mrow], buffer, sizeof(buffer));
                // update the current row of the memory
                mrow += ceil(sizeof(buffer) / 4);

                bufferReset();
            }
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