// OS Phase 2 by Group SY-10

#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

#define EMPTY '\0'
typedef uint8_t byte;
typedef uint16_t u16;
typedef size_t ulong;

class pcb
{
public:
    int jobID, TTC, LLC, TTL, TLL;
    void pcb_init(string);
    void displayPCB();
};

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

    int PTR[4]; // Page Table Register
    vector<int> addresses;

public:
    void init();
    void masterMode(int);
    void read(int);
    void write(int);
    void terminate(vector<int>);
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
    void displayProgramStatus();

    int allocate();
    bool checkIfMemoryAvailable(int);
    void setPTR(int);
    void pageTableInit();
    void updatePT(int);
    int addressMap(int);
    bool checkForPage(int);
    void checkTime();
    int getStartAddr();
    int readPTrow(int);
    void EMinit();

    ifstream inFile;
    ofstream outFile;

    pcb PCB;
};

void cpu ::bufferReset()
{
    memset(buffer, EMPTY, sizeof(buffer));
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
    SI = 3;
    PI = 0;
    TI = 0;
    PTrow = -1;
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

void pcb ::displayPCB()
{
    cout << "Job id is: " << jobID << endl;
    cout << "TTC is: " << TTC << endl;
    cout << "LLC is: " << LLC << endl;
    cout << "TTL is: " << TTL << endl;
    cout << "TLL is: " << TLL << endl;
}

void cpu::displayProgramStatus()
{
    cout << "PCB Values : " << endl;
    PCB.displayPCB();
    cout << "IC is : " << IC << endl;
    cout << "C is : " << C << endl;
    cout << "SI is : " << SI << endl;
    cout << "TI is : " << TI << endl;
    cout << "PI is : " << PI << endl;
    cout << "IR is : " << endl;
    for (int i = 0; i < 4; i++)
    {
        cout << IR[i] << " ";
    }
    cout << "IC is : " << IC << endl;
    cout << "R is : " << R << endl;
    cout << "\nMemory is : " << endl;
    printMemory();
}

void cpu ::printMemory()
{
    // print memory to check if enterred correctly
    for (int a = 0; a < 300; a++)
    {
        for (int b = 0; b < 4; b++)
        {
            cout << "m[" << a << "][" << b << "]: " << m[a][b] << " ";
        }
        cout << endl;
    }
}

void pcb ::pcb_init(string s)
{
    TTC = LLC = 0;
    jobID = stoi(s.substr(4, 4));
    TTL = stoi(s.substr(8, 4));
    TLL = stoi(s.substr(12, 4));
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

    addresses.push_back(random);
    return random;
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
}

int cpu ::getStartAddr()
{
    int start = 0;
    for (int i = 0; i < 4; i++)
    {
        start = PTR[i] + start * 10;
    }
    return start;
}

void cpu ::pageTableInit()
{

    for (int i = getStartAddr(); i < (getStartAddr() + 10); i++)
    {
        for (int j = 0; j < 4; j++)
        {
            m[i][j] = '*';
        }
    }
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

void cpu ::updatePT(int addr)
{
    string myAddr = to_string(addr);
    memcpy(m[PTrow], myAddr.c_str(), myAddr.length());
    PTrow++;
}

int cpu ::readPTrow(int row)
{

    int myRowVal;

    for (int i = 0; i < 4; i++)
    {

        if (m[row][i] == '*')
        {
            break;
        }

        myRowVal = (m[row][i] - '0') + (myRowVal * 10);
    }

    return myRowVal;
}

bool cpu ::checkForPage(int RA)
{

    if (std::find(addresses.begin(), addresses.end(), RA) != addresses.end())
    {
        return true;
    }

    return false;
}

int cpu ::addressMap(int VA)
{

    if (VA < 0 || VA > 99)
    {
        PI = 2;
        return -1;
    }

    int RA;
    int myPTR = getStartAddr();
    int mPTE = 0;

    int PTE = (VA / 10) + myPTR;

    mPTE = readPTrow(PTE);

    RA = (mPTE * 10) + (VA % 10);

    if (!checkForPage(RA))
    {
        PI = 3;
        return -1;
    }

    return RA;
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
        terminate({1});
    }
    memcpy(m[address], buffer, sizeof(buffer));

    SI = 0;
    executeUserProgram();
}

void cpu ::write(int address)
{
    PCB.LLC++;
    if (PCB.LLC > PCB.TLL)
    {
        terminate({2});
    }
    bufferReset();
    memcpy(buffer, m[address], 41);
    outFile << buffer << endl;
    SI = 0;
    executeUserProgram();
}

void cpu ::terminate(vector<int> em)
{
    outFile << endl
            << endl;
    if (em[0] == 0)
    {
        cout << "Normal Termination" << endl;
    }
    else
    {
        cout << "Abnormal Termination" << endl;
        for (int i = 0; i < em.size(); i++)
        {
            cout << errorMessage[(em[i])] << endl;
        }
    }
    displayProgramStatus();
    load();
}

void cpu::masterMode(int address)
{
    // TI AND SI
    if (TI == 0 && SI == 1)
    {
        read(address);
    }
    else if (TI == 0 && SI == 2)
    {
        write(address);
    }
    else if (TI == 0 && SI == 3)
    {
        terminate({0});
    }
    if (TI == 2 && SI == 1)
    {
        terminate({3});
    }
    else if (TI == 2 && SI == 2)
    {
        write(address);
        terminate({3});
    }
    else if (TI == 2 && SI == 3)
    {
        terminate({0});
    }

    // TI AND PI
    if (TI == 0 && PI == 1)
    {
        terminate({4});
    }
    else if (TI == 0 && PI == 2)
    {
        terminate({5});
    }
    else if (TI == 0 && PI == 3)
    {
        // IF Valid Page Fault then allocate page frame. decrement the IC. Update PT
        //
        if ((IR[0] == 'G' && IR[1] == 'D') || (IR[0] == 'S' && IR[1] == 'R'))
        {
            int addr = allocate();
            updatePT(addr);

            IC--;
            executeUserProgram();
        }
        else
        {
            terminate({6});
        }
    }
    if (TI == 2 && PI == 1)
    {
        terminate({3, 4});
    }
    else if (TI == 2 && PI == 2)
    {

        terminate({3, 5});
    }
    else if (TI == 2 && PI == 3)
    {
        terminate({3});
    }
}

void cpu ::checkTime()
{
    if (PCB.TTC == PCB.TTL)
    {
        TI = 2;
    }
}

void cpu ::loadRegister(int address)
{
    memcpy(R, m[address], 4);
}

void cpu ::storeRegister(int address)
{
    memcpy(m[address], R, 4);
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
    while ((PI == 0) && (TI == 0))
    {

        int RA = addressMap(IC);

        memcpy(IR, m[RA], 4);
        IC++;

        int address = (IR[2] - '0') * 10 + (IR[3] - '0');
        address = addressMap(address);

        if (IR[0] == 'G' && IR[1] == 'D')
        {

            SI = 1;
            masterMode(address);
            PCB.TTC += 2;
        }
        else if (IR[0] == 'P' && IR[1] == 'D')
        {

            SI = 2;
            masterMode(address);
            PCB.TTC++;
        }
        else if (IR[0] == 'H' && IR[1] == 0)
        {

            SI = 3;
            masterMode(address);
            PCB.TTC++;
            break;
        }
        else if (IR[0] == 'L' && IR[1] == 'R')
        {

            loadRegister(address);
            PCB.TTC++;
        }
        else if (IR[0] == 'S' && IR[1] == 'R')
        {

            storeRegister(address);
            PCB.TTC += 2;
        }
        else if (IR[0] == 'C' && IR[1] == 'R')
        {

            compareRegister(address);
            PCB.TTC++;
        }
        else if (IR[0] == 'B' && IR[1] == 'T')
        {

            branch(address);
            PCB.TTC++;
        }
        else
        {
            PI = 1;
        }

        checkTime();
    }

    if ((PI != 0) || (TI != 0))
    {
        masterMode(0);
    }
}

void cpu ::startExecution()
{
    IC = 0;
    executeUserProgram();
}

void cpu ::load()
{
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
            PCB.pcb_init(buff);

            // allocate frame for Page Table and set PTR
            int ptAddr = allocate() * 10;
            setPTR(ptAddr);
            PTrow = ptAddr;

            // Initialise the Page Table
            pageTableInit();
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
            updatePT(addr);

            // copy buffer contents to memory
            memcpy(m[(addr * 10)], buff.c_str(), buff.length());

            bufferReset();
        }
    }
}

int main()
{

    cpu myCPU;

    myCPU.inFile.open("job_structure.txt");
    myCPU.outFile.open("output.txt");

    if (!(myCPU.inFile))
    {
        cout << "Input File Doesn't Exist" << endl;
    }
    else
    {
        cout << "Starting OS - simulation. " << endl;
    }

    myCPU.load();

    myCPU.inFile.close();
    myCPU.outFile.close();

    return 0;
}