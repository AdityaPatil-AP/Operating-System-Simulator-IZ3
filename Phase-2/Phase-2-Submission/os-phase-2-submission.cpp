// OS Phase 2 by Group SY-10

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string.h>
#include <vector>

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

    int PTR[4];            // Page Table Register
    vector<int> addresses; // vector of allocated addresses

    bool hRead; // Check if Halt read
    bool error; // check if error, to continue traversing file to get next job

public:
    void init();          // initialise cpu
    void masterMode(int); // MasterMode function
    void read(int);       // read from file
    void write(int);      // write to file
    void terminate(vector<int>);
    void executeUserProgram();
    void loadRegister(int);
    void storeRegister(int);
    void compareRegister(int);
    void branch(int);
    void startExecution();
    void load();
    void bufferReset();                                                // reset buffer to EMPTY
    void printMemory();                                                // displays Memory
    string replaceString(string, const std::string &, const string &); // replace 'H' by 'H000'

    void displayProgramStatus();      // print all register, interrupt values.
    int allocate();                   // allocate a page from 0 to 29
    bool checkIfMemoryAvailable(int); // check if that block of memory is EMPTY
    void setPTR(int);                 // set Page Table Register with the inputted address
    void pageTableInit();             // initialise Page Table  '*'
    void updatePT(int);               // update Page Table with newly allocated page
    int addressMap(int);              // convert VA to RA
    bool checkForPage(int);           // check if the logical page exists in real Memory
    void checkTime();                 // check whether time has exceeded program limits
    int getStartAddr();               // get PTR addr
    int readPTrow(int);               // unused
    void EMinit();                    // assign error messages to keys

    ifstream inFile;
    ofstream outFile;

    pcb PCB; // create one pcb as per assumption
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
    EMinit();
    error = false;
    hRead = false;
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
    // cout << "Job id is: " << jobID << endl;
    // cout << "TTC is: " << TTC << endl;
    // cout << "LLC is: " << LLC << endl;
    // cout << "TTL is: " << TTL << endl;
    // cout << "TLL is: " << TLL << endl;

    // outFile<<"Job id is: "<<jobID<<", TTC is: "<<TTC<<", LLC is: "<<LLC<<", TTL is: "<<TTL<<", TLL is: "<<TLL<<endl;
}

void cpu::displayProgramStatus()
{

    outFile << "PCB Values:- Job id is: " << PCB.jobID << ", TTC is: " << PCB.TTC << ", LLC is: " << PCB.LLC << ", TTL is: " << PCB.TTL << ", TLL is: " << PCB.TLL << endl;
    outFile << "IC is: " << IC << ", C is: " << C << ", SI is: " << SI << ", TI is: " << TI << ", PI is: " << PI << ", IR is: ";
    for (int i = 0; i < 4; i++)
    {
        outFile << IR[i] << " ";
    }
    outFile << ", R is: ";
    for (int i = 0; i < 4; i++)
    {
        outFile << R[i] << " ";
    }
    outFile << "\n____________________________________________________________________________________________";
    outFile << endl
            << endl;
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
    PTrow++; // increment pointer to current PT row
}

int cpu ::readPTrow(int row)
{
    // unused
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
    // check for that page in addresses vector
    if (std::find(addresses.begin(), addresses.end(), RA) != addresses.end())
    {
        return true;
    }

    return false;
}

int cpu ::addressMap(int VA)
{
    // set PI if invalid VA provided
    if (VA < 0 || VA > 99)
    {
        PI = 2;
        return -1;
    }

    int RA;

    int myPTR = 0;
    int mPTE = 0;

    for (int i = 0; i < 4; i++)
    {
        myPTR = PTR[i] + myPTR * 10;
    }

    // get Page Table Entry
    int PTE = (VA / 10) + myPTR;

    // get memory value at PTE
    for (int i = 0; i < 4; i++)
    {

        if (m[PTE][i] == '*')
        {
            break;
        }

        mPTE = (m[PTE][i] - '0') + (mPTE * 10);
    }

    // calculate RA
    RA = (mPTE * 10) + (VA % 10);

    if (!checkForPage(RA / 10))
    {

        // if page doesn't exist, set PI to 3
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

    // copy file contents to buffer
    inFile.getline(buffer, 41);
    if (buffer[0] == '$' && buffer[1] == 'E' && buffer[2] == 'N' && buffer[3] == 'D')
    {
        terminate({1});
        // terminate if no data card present
    }
    memcpy(m[address], buffer, sizeof(buffer)); // copy buffer contents to memory
    PCB.TTC += 2;                               // increment time counter
}

void cpu ::write(int address)
{

    PCB.LLC++;
    if (PCB.LLC > PCB.TLL)
    {
        terminate({2}); // if line limit exceeded, print error
    }
    bufferReset(); // flush buffer

    // Now
    //  copy memory contents to buffer and
    //  print buffer contents to file, end with newline

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

    PCB.TTC++; // increment time counter
}

void cpu ::terminate(vector<int> em)
{
    // print two blank lines in output file
    outFile << endl
            << endl;

    PCB.TTC++; // increment time counter

    if (em[0] == 0)
    {

        // normal termination if error message is zero
        outFile << "Normal Termination" << endl;
        hRead = true;
    }
    else
    {

        // abnormal termination otherwise
        outFile << "Abnormal Termination" << endl;
        for (int i = 0; i < em.size(); i++)
        {

            // print out all error messages
            outFile << errorMessage[em[i]] << endl;
        }
        error = true;
        hRead = true;
    }

    // print all data structures' values
    displayProgramStatus();
    load();
}

void cpu::masterMode(int address)
{

    // match interrupt values with corresponding case

    // TI AND SI
    if (TI == 0 && SI == 1 && PI == 0)
    {
        read(address);
    }
    else if (TI == 0 && SI == 2 && PI == 0)
    {
        write(address);
    }
    else if (TI == 0 && SI == 3 && PI == 0)
    {
        terminate({0});
    }
    else if (TI == 2 && SI == 1 && PI == 0)
    {
        terminate({3});
    }
    else if (TI == 2 && SI == 2)
    {
        write(address);

        terminate({3});
    }
    else if (TI == 2 && SI == 3 && PI == 0)
    {

        terminate({0});
    }

    // TI AND PI
    else if (TI == 0 && PI == 1)
    {

        terminate({4});
    }
    else if (TI == 0 && PI == 2)
    {

        terminate({5});
    }
    else if (TI == 0 && PI == 3)
    {
        if ((IR[0] == 'G' && IR[1] == 'D') || (IR[0] == 'S' && IR[1] == 'R'))
        {
            // valid pagefault  case
            int addr = allocate();
            updatePT(addr);
            IC--;
            PI = 0;
        }
        else
        {

            // invalid pagefault case
            terminate({6});
        }
    }
    else if (TI == 2 && PI == 1)
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

    TI = PI = 0;
}

void cpu ::checkTime()
{
    if (PCB.TTC >= PCB.TTL)
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

        if (hRead)
        {
            break;
        }

        // get RA
        int RA = addressMap(IC);

        memcpy(IR, m[RA], 4);
        IC++;

        int address;

        // calculate operand value
        if (IR[0] != 'H' && IR[0] != 'B')
        {
            address = (IR[2] - '0') * 10 + (IR[3] - '0');
            address = addressMap(address);
        }
        else
        {
            address = (IR[2] - '0') * 10 + (IR[3] - '0');
        }

        // check if time exceeded
        checkTime();

        if (IR[0] == 'G' && IR[1] == 'D')
        {
            SI = 1;
            masterMode(address);
        }
        else if (IR[0] == 'P' && IR[1] == 'D')
        {
            SI = 2;
            masterMode(address);
        }
        else if (IR[0] == 'H' && IR[1] == '0')
        {

            SI = 3;

            masterMode(address);
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

            // invalid opcode, set PI
            PI = 1;
        }

        // 	checkTime();
    }

    if ((PI != 0) || (TI != 0))
    {
        // call master mode if PI/TI interrupts set.
        masterMode(0);
    }
}

void cpu ::startExecution()
{
    IC = 0; // initialise IC to 0
    executeUserProgram();
}

void cpu ::load()
{

    string buff;

    while (getline(inFile, buff))
    {

        if (buff.length() > 41)
        {

            // check if card length is valid
            cout << "Length of Card exceeds 40 !!" << endl;
            error = true;
        }

        // if any error in previous program, continue traversing file till next '$AMJ' is read
        if (error == true && buff.substr(0, 4) != "$AMJ")
        {
            continue;
        }

        // Check if Control Card AMJ
        if (buff.substr(0, 4) == "$AMJ")
        {
            error = false;
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

    myCPU.inFile.open("input1.txt");
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

    myCPU.load();

    // close input, output files
    myCPU.inFile.close();
    myCPU.outFile.close();

    return 0;
}