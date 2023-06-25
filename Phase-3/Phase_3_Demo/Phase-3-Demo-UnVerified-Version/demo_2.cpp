#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string.h>
#include <vector>
#include <queue>
#include <cmath>

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
    void init();
    void printAux();
};

class supervisoryStorage
{
public:
    queue<vector<char>> efbq;
    queue<vector<char>> ifbq;
    queue<vector<char>> ofbq;

    void efbqInit();
};

void secondaryStorage::printAux()
{
    for (int a = 0; a < 500; a++)
    {
        for (int b = 0; b < 4; b++)
        {
            cout << "aux[" << a << "][" << b << "]: " << aux[a][b] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

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

void secondaryStorage::init()
{
    memset(aux, EMPTY, sizeof(aux));
}

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
    int auxRow;

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
    void startExecution();                                             // reset buffer to EMPTY
    void printMemory();                                                // displays Memory
    string replaceString(string, const std::string &, const string &); // replace 'H' by 'H000'

    // void displayProgramStatus();      // print all register, interrupt values.
    int allocate();                   // allocate a page from 0 to 29
    bool checkIfMemoryAvailable(int); // check if that block of memory is EMPTY
    void setPTR(int);                 // set Page Table Register with the inputted address
    void pageTableInit();             // initialise Page Table  '*'
    void updatePT(int);               // update Page Table with newly allocated page
    int addressMap(int);              // convert VA to RA
    bool checkForPage(int);           // check if the logical page exists in real Memory
    void checkTime();                 // check whether time has exceeded program limits
    int getStartAddr();
    int readPTrow(int);
    void EMinit();
    ifstream inFile;
    ofstream outFile;

    void simulation();
    void IR1();
    void IR3();
    void IR2();
    void IS();
    void OS();
    void startChannel1();
    void startChannel2();
    void startChannel3();
    void startLoading(int);
    void RD(int);

    int currentPCBIndex;
};

void cpu ::startExecution()
{
    IC = 0; // initialise IC to 0
    executeUserProgram();
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

void cpu ::checkTime()
{
    if (PCBlist[currentPCBIndex].TTC >= PCBlist[currentPCBIndex].TTL)
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

        // if (hRead)
        // {
        //     break;
        // }

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
            PCBlist[currentPCBIndex].TTC++;
        }
        else if (IR[0] == 'S' && IR[1] == 'R')
        {

            storeRegister(address);
            PCBlist[currentPCBIndex].TTC += 2;
        }
        else if (IR[0] == 'C' && IR[1] == 'R')
        {

            compareRegister(address);
            PCBlist[currentPCBIndex].TTC++;
        }
        else if (IR[0] == 'B' && IR[1] == 'T')
        {

            branch(address);
            PCBlist[currentPCBIndex].TTC++;
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

void cpu ::updatePT(int addr)
{
    string myAddr = to_string(addr);
    memcpy(m[PTrow], myAddr.c_str(), myAddr.length());
    PTrow++; // increment pointer to current PT row
}

void cpu ::printMemory()
{
    cout << "Memory : " << endl;
    for (int a = 0; a < 300; a++)
    {
        for (int b = 0; b < 4; b++)
        {
            cout << "m[" << a << "][" << b << "]: " << m[a][b] << " ";
        }
        cout << endl;
    }
    cout << endl;
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
    cout << "allocate" << endl;
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

    for (int i = getStartAddr(); i < (getStartAddr() + 10); i++)
    {
        for (int j = 0; j < 4; j++)
        {
            m[i][j] = '*';
        }
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
    auxRow = 0;
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
        cout << "Str is : " << str << endl;
        if (str.length() >= 41)
        {
            cout << "Length of Card exceeds 40 !!" << endl;
            // add code to continue parsing till next correct job.
            //  error = true;
        }

        vector<char> buffer = ss.efbq.front();
        ss.efbq.pop();

        for (int i = 0; i < str.length(); i++)
        {
            buffer[i] = str[i];
        }

        ss.ifbq.push(buffer);
        startChannel3();

        if (!ss.efbq.empty())
        {
            startChannel1();
        }
    }
    else
    {
        c1.ch1F = false;
    }
}

void cpu::IS()
{

    vector<char> buffer = ss.ifbq.front();

    if (buffer[0] == '$' && buffer[1] == 'A' && buffer[2] == 'M' && buffer[3] == 'J')
    {
        init();

        pcb PCB;
        PCBlist.push_back(PCB);
        currentPCBIndex = (PCBlist.size() - 1);
        string buff(buffer.begin(), buffer.end());
        (PCBlist[currentPCBIndex]).pcb_init(buff);
        (PCBlist[currentPCBIndex]).F = 'P';
        int ptAddr = allocate() * 10;
        setPTR(ptAddr);
        PTrow = ptAddr;

        pageTableInit();
        // printMemory();
    }
    else if (buffer[0] == '$' && buffer[1] == 'D' && buffer[2] == 'T' && buffer[3] == 'A')
    {
        (PCBlist[currentPCBIndex]).F = 'D';
    }

    // Check if Control Card END
    else if (buffer[0] == '$' && buffer[1] == 'E' && buffer[2] == 'N' && buffer[3] == 'D')
    {
        LQ.push(PCBlist[currentPCBIndex]);
        startLoading(currentPCBIndex);
        PCBlist.pop_back();
        // init();
    }

    // Else it's a program card or data card
    else
    {
        // handle this edgecase
        //  if (auxRow >= 499)
        //  {
        //      cout << "Auxillary Memory exceeded!";
        //      // break outta while loop if memory overflow
        //  }

        string buff = "";
        for (int i = 0; i < buffer.size(); i++)
        {
            buff.push_back(buffer[i]);
        }

        if ((PCBlist[currentPCBIndex]).F == 'P')
        {

            buff = replaceString(buff, "H", "H000");

            memcpy(drum.aux[auxRow], buff.c_str(), buff.length());

            (PCBlist[currentPCBIndex]).no_pc++;
            if ((PCBlist[currentPCBIndex]).p_track == -1)
            {
                (PCBlist[currentPCBIndex]).p_track = auxRow;
            }
        }
        else
        {

            memcpy(drum.aux[auxRow], buff.c_str(), buff.length());

            (PCBlist[currentPCBIndex]).no_dc++;
            if ((PCBlist[currentPCBIndex]).d_track == -1)
            {
                (PCBlist[currentPCBIndex]).d_track = auxRow;
            }
        }
        auxRow += 10;
    }

    ss.ifbq.pop();
    std::fill(buffer.begin(), buffer.end(), '\0');
    ss.efbq.push(buffer);
}

void cpu::OS()
{
    return;
}

void cpu::startLoading(int currentPCBIndex)
{

    int track = PCBlist[currentPCBIndex].p_track;

    int addr = allocate();
    updatePT(addr);

    memcpy(m[addr * 10], drum.aux[track], 40);
    track = (track + 10);
    (PCBlist[currentPCBIndex].no_pc)--;

    if ((PCBlist[currentPCBIndex].no_pc) == 0)
    {
        LQ.pop();
        RQ.push(PCBlist[currentPCBIndex]);
        //   startExecution();
    }

    // printMemory();
}

void cpu::RD(int currentPCBIndex)
{
    // read one data card at a time

    int track = PCBlist[currentPCBIndex].d_track;

    int addr = allocate();
    updatePT(addr);

    memcpy(m[addr * 10], drum.aux[track], 40);
    track = (track + 10);
    (PCBlist[currentPCBIndex].no_dc)--;

    pcb currentPCB = IOQ.front();
    IOQ.pop();
    RQ.push(currentPCB);
}

void cpu::startChannel1()
{
    // IOI -= 1;
    c1.ch1F = true; // Indicating Busy.
    c1.ch1T = 0;
}

void cpu::startChannel2()
{
    // IOI -= 2;
    c2.ch2F = true; // Indicating Busy.
    c2.ch2T = 0;
}

void cpu::startChannel3()
{
    // IOI -= 4;
    c3.ch3F = true; // Indicating Busy.
    c3.ch3T = 0;
}

void cpu::IR3()
{
    IOI = IOI - 4;
    c3.ch3T = 0;
    c3.ch3F = false;

    // Priority Order.
    // 1) Output Spool First
    if (!TQ.empty())
    {
        if (!ss.efbq.empty())
        {
            // vector<char> ebq = ss.efbq.front();
            // ss.efbq.pop();
            // Output Spooling.
            // check
            OS();
            startChannel3();
        }
    }
    else if (!ss.ifbq.empty())
    {
        // Input Spooling case
        IS();
        startChannel3();
    }
    else if (!LQ.empty())
    {
        // Here we have to load one program card at a time. That we have to check.
        startLoading(currentPCBIndex);
        startChannel3();
    }
    else if (!IOQ.empty())
    {
        // gd pd
        // startExecution

        // get RA
        // int RA = addressMap(IC);

        // memcpy(IR, m[RA], 4);
        // // For address check the operand error.
        // // Maine address niche calculate kiya hai usse upar hi calculate kar lo.
        // int address;

        // if (IR[0] == 'G' && IR[1] == 'D')
        // {
        //     if (PCBlist[currentPCBIndex].no_dc == 0)
        //     {
        //         pcb curr = IOQ.front();
        //         // curr mai type of termination bhi pata hona chahiye.
        //         // Here there is terminate type number 3.
        //         IOQ.pop();
        //         TQ.push(curr);
        //     }
        //     else
        //     {
        //         int track_pos = PCBlist[currentPCBIndex].d_track;
        //         address = (IR[2] - '0') * 10 + (IR[3] - '0');
        //         // Finding the next data card.
        //         SI = 1;
        //         masterMode(address);
        //     }
        // }
        // else if (IR[0] == 'P' && IR[1] == 'D')
        // {
        //     if (PCBlist[currentPCBIndex].LLC > PCBlist[currentPCBIndex].TLL)
        //     {
        //         pcb curr = IOQ.front();
        //         // curr mai type of termination bhi pata hona chahiye.
        //         // Terminate type 2.
        //         IOQ.pop();
        //         TQ.push(curr);
        //     }
        //     else
        //     {
        //         int track_pos = PCBlist[currentPCBIndex].d_track;
        //         // Check for
        //         address = (IR[2] - '0') * 10 + (IR[3] - '0');
        //         // Finding the next data card.
        //         SI = 2;
        //         masterMode(address);
        //     }
        // }
    }
    else if (!RQ.empty())
    {
        startExecution();
    }

    startChannel3();
}

void cpu::IR2()
{
    // tested and working
    IOI = IOI - 2;
    c2.ch2T = 0;
    c2.ch2F = false;

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
        startChannel2();
    }
}

void cpu ::read(int address)
{

    // bufferReset();
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
    PCBlist[currentPCBIndex].TTC += 2;          // increment time counter
}

void cpu ::write(int address)
{

    PCBlist[currentPCBIndex].LLC++;
    if (PCBlist[currentPCBIndex].LLC > PCBlist[currentPCBIndex].TLL)
    {
        terminate({2}); // if line limit exceeded, print error
    }
    // bufferReset();                  // flush buffer

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

    PCBlist[currentPCBIndex].TTC++; // increment time counter
}

void cpu ::terminate(vector<int> em)
{
    // print two blank lines in output file
    outFile << endl
            << endl;

    PCBlist[currentPCBIndex].TTC++; // increment time counter

    if (em[0] == 0)
    {

        // normal termination if error message is zero
        outFile << "Normal Termination" << endl;
        // 	hRead = true;
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
        // 	error = true;
        // 	hRead = true;
    }

    // print all data structures' values
    // displayProgramStatus();
    //  load();
}

void cpu::masterMode(int address)
{
    // if (SI == 1)
    // {
    //     // RQ to IOQ.
    // }

    // match interrupt values with corresponding case

    // TI AND SI
    if (TI == 0 && SI == 1 && PI == 0)
    {
        pcb myPCB = RQ.front();
        RQ.pop();
        IOQ.push(myPCB);
        startChannel3();
        // read(address);
        TI = PI = 0;
        return;
    }
    else if (TI == 0 && SI == 2 && PI == 0)
    {
        pcb myPCB = RQ.front();
        RQ.pop();
        IOQ.push(myPCB);
        startChannel3();
        // 	write(address);
        TI = PI = 0;
        return;
    }
    else if (TI == 0 && SI == 3 && PI == 0)
    {
        pcb myPCB = RQ.front();
        RQ.pop();
        TQ.push(myPCB);
        startChannel3();
        terminate({0});
        TI = PI = 0;
        return;
    }
    else if (TI == 2 && SI == 1 && PI == 0)
    {

        pcb myPCB = RQ.front();
        RQ.pop();
        TQ.push(myPCB);
        terminate({3});
        TI = PI = 0;
        return;
    }
    else if (TI == 2 && SI == 2)
    {
        pcb myPCB = RQ.front();
        RQ.pop();
        IOQ.push(myPCB);
        write(address);
        terminate({3});
        TI = PI = 0;
        return;
    }
    else if (TI == 2 && SI == 3 && PI == 0)
    {

        pcb myPCB = RQ.front();
        RQ.pop();
        TQ.push(myPCB);
        terminate({0});
        TI = PI = 0;
        return;
    }

    // TI AND PI
    else if (TI == 0 && PI == 1)
    {
        pcb myPCB = RQ.front();
        RQ.pop();
        TQ.push(myPCB);
        terminate({4});
        TI = PI = 0;
        return;
    }
    else if (TI == 0 && PI == 2)
    {

        pcb myPCB = RQ.front();
        RQ.pop();
        TQ.push(myPCB);
        terminate({5});
        TI = PI = 0;
        return;
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
            TI = PI = 0;
            return;
        }
        else
        {

            // invalid pagefault case
            pcb myPCB = RQ.front();
            RQ.pop();
            TQ.push(myPCB);
            terminate({6});
            TI = PI = 0;
            return;
        }
    }
    else if (TI == 2 && PI == 1)
    {

        pcb myPCB = RQ.front();
        RQ.pop();
        TQ.push(myPCB);
        terminate({3, 4});
        TI = PI = 0;
        return;
    }
    else if (TI == 2 && PI == 2)
    {

        pcb myPCB = RQ.front();
        RQ.pop();
        TQ.push(myPCB);
        terminate({3, 5});
        TI = PI = 0;
        return;
    }
    else if (TI == 2 && PI == 3)
    {

        pcb myPCB = RQ.front();
        RQ.pop();
        TQ.push(myPCB);
        terminate({3});
        TI = PI = 0;
        return;
    }

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
        simulation();
        break;
    }
    case 3:
    {
        IR2();
        IR1();
        break;
    }
    case 4:
    {
        IR3();
        simulation();
        break;
    }
    case 5:
    {
        IR1();
        IR3();
        break;
    }
    case 6:
    {
        IR3();
        IR2();
        break;
    }
    case 7:
    {
        IR2();
        IR1();
        IR3();
        break;
    }
    }
}

void cpu::simulation()
{

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
        if (c1.ch1F == 0 && c2.ch2F == 0 && c3.ch3F == 0)
        {
            return;
        }
    }
    masterMode(0);
}

int main()
{

    ss.efbqInit();
    drum.init();
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

    c1.ch1F = true; // Indicates it is busy.
    IOI = 0;
    myCPU.simulation();

    // close input, output files
    myCPU.inFile.close();
    myCPU.outFile.close();

    return 0;
}
