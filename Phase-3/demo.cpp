#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

int main()
{
    ofstream outfile;
    outfile.open("output.txt");
    vector<char> m = {'a', 'b'};
    for (int i = 0; i < m.size(); i++)
    {
        outfile << m[i] << endl;
    }
    return 0;
}