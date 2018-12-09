#ifndef COMMONFUNCTIONS_H
#define COMMONFUNCTIONS_H

#include <vector>
#include <string>
#include <fstream>
#include <regex>

using namespace std;


vector<string> Split(const string &testStr, const char &symbol) noexcept
{
    vector<string> splitStr(0);
    if (testStr.size() == 0)
    {
        splitStr.push_back("");
        return splitStr;
    }
    for (uint16_t i = 0; i < testStr.size(); ++i)
    {
        string newPart = "";
        while (i != testStr.size() && testStr[i] != symbol)
        {
            newPart.push_back(testStr[i]);
            ++i;
        }
        splitStr.push_back(newPart);
    }
    return splitStr;
}

vector<string> ReadAsmCodes(const string &filename)
{
    vector<string> asmCodes(0);
    ifstream fin(filename);
    while (!fin.eof())
    {
        char *buffer = new char[100];
        fin.getline(buffer, 99);
        asmCodes.emplace_back(string(buffer));
    }
    fin.close();
    return asmCodes;
}

void WriteAsmCodes(const string &filename, const vector<string> &asmCodes)
{
    ofstream fout(filename);
    for (uint16_t i = 0; i != asmCodes.size(); ++i)
        fout << asmCodes[i] << endl;
    fout.close();
}

void ChangeString(string &testStr, const string &searchStr, const map<string, string> &mapOfDefinitions) noexcept
{
    regex rReplace("(\\s)" + searchStr);
    string tr = regex_replace(testStr, rReplace, " " + mapOfDefinitions.find(searchStr)->second);
    testStr = tr;
}

#endif // COMMONFUNCTIONS_H
