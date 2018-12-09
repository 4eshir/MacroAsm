#ifndef MACROASMFUNCTIONS_H_INCLUDED
#define MACROASMFUNCTIONS_H_INCLUDED

#include <regex>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>


map<string, vector<string> > mapOfMacros;
map<string, vector<string> > mapOfRepeats;
map<string, string> mapOfDefinitions;


void ChangeDefinitions(string &str)
{
    for (auto it = mapOfDefinitions.begin(); it != mapOfDefinitions.end(); ++it)
    {
        regex rDef("(.*)" + it->second + "(.*)");
    }
}


//предварительный проход, сборка в единый файл
void BuildFile(vector<string> &asmCodes)
{
    regex rInc("INCLUDE(.*)");
    for (uint16_t i = 0; i != asmCodes.size(); ++i)
    {
        if (regex_match(asmCodes[i], rInc) == 1)
        {
            string path = Split(asmCodes[i], ' ').at(1);
            path.erase(path.begin());
            path.erase(path.end() - 1);
            ifstream fin(path);
            if (fin.is_open())
            {
                vector<string> includeBody(0);
                while (!fin.eof())
                {
                    char *buffer = new char[100];
                    fin.getline(buffer, 99);
                    includeBody.emplace_back(string(buffer));
                }
                asmCodes.erase(asmCodes.begin() + i);

                asmCodes.insert(asmCodes.begin() + i, includeBody.begin(), includeBody.end());
                i += includeBody.size();
            }
            else
            {
                cerr << "Could not file in this path: " + path << endl;
            }
        }
    }
}
//--------------------------------------------


void MacrosProcessing(const int &i, int &shift, vector<string> &asmCodes)
{

    string macros = Split(asmCodes[i], ' ').at(0);
    string vars = Split(asmCodes[i], ' ').at(2);
    //получение списка формальных переменных
    vars.erase(vars.begin());
    vars.erase(vars.end() - 1);
    //--------------------------------------
    vector<string> bodyMacro;
    bodyMacro.emplace_back(vars);
    asmCodes.erase(asmCodes.begin() + i);

    //получение тела макроса
    while (asmCodes[i] != "ENDM")
    {
        bodyMacro.emplace_back(asmCodes[i]);
        asmCodes.erase(asmCodes.begin() + i);
    }
    asmCodes.erase(asmCodes.begin() + i);
    shift = 1;
    //-----------------------
    //добавление макроса в список макросов
    mapOfMacros.insert(pair<string, vector<string> >(macros, bodyMacro));

}

void DefinitionProcessing(const int &i, int &shift, vector<string> &asmCodes)
{
    string equ = Split(asmCodes[i], ' ').at(1);
    mapOfDefinitions.insert(pair<string, string>(Split(equ, '=').at(0), Split(equ, '=').at(1)));
    asmCodes.erase(asmCodes.begin() + i);
    shift = 1;
}

void UndefinitionProcessing(const int &i, int &shift, vector<string> &asmCodes)
{
    mapOfDefinitions.erase(Split(asmCodes[i], ' ').at(1));
    asmCodes.erase(asmCodes.begin() + i);
    shift = 1;
}

void RepeatProcessing(int &&i, int &shift, vector<string> &asmCodes)
{
    int j = i;
    ++i;
    vector<string> bodyRep(0);
    while (asmCodes[i] != "ENDREP")
    {
        bodyRep.emplace_back(asmCodes[i]);
        asmCodes.erase(asmCodes.begin() + i);

    }
    int counter = stoi(Split(asmCodes[j], ' ').at(1));

    asmCodes.erase(asmCodes.begin() + i); //удалить начало и конец цикла
    asmCodes.erase(asmCodes.begin() + j);
    --i;



    for (uint16_t j = 0; j != counter; ++j)
    {
        asmCodes.insert(asmCodes.begin() + i, bodyRep.begin(), bodyRep.end());

    }
}


void FileParser_pass1 (vector<string> &asmCodes)
{
    vector<regex> rMacros = {regex("(.*)MACRO(.*)"),
                             regex("DEF(.*)"),
                             regex("UNDEF(.*)"),
                             regex("EQU(.*)"),
                             regex("REP(.*)")};

    int shift = 0;

    for (uint16_t i = 0; i != asmCodes.size(); ++i)
    {
        i -= shift;
        shift = 0;
        /*
        Обработка макросов
        */
        if (regex_match(asmCodes[i], rMacros[0]) == 1)
            MacrosProcessing(i, shift, asmCodes);

        //поиск макроподстановок
        if (regex_match(asmCodes[i], rMacros[1]) == 1 || regex_match(asmCodes[i], rMacros[3]) == 1)
            DefinitionProcessing(i, shift, asmCodes);
        //----------------------

        if (regex_match(asmCodes[i], rMacros[2]) == 1)
            UndefinitionProcessing(i, shift, asmCodes);

        //поиск циклов и замена
        if (regex_match(asmCodes[i], rMacros[4]) == 1)
            RepeatProcessing(i, shift, asmCodes);
        //---------------------

    }
}



void SumProcessing(int &&i, vector<string> &asmCodes)
{
    int r1 = stoi(Split(asmCodes[i], ' ').at(1));
    int r2 = stoi(Split(asmCodes[i], ' ').at(2));
    for (uint16_t j = r1; j != r2; ++j, ++i)
    {
        asmCodes.insert(asmCodes.begin() + i, "add " + to_string(j) + " " + to_string(j + 1));
    }
    asmCodes.erase(asmCodes.begin() + i);

}


void FileParser_pass2 (vector<string> &asmCodes)
{
    regex rSum("SUM(.*)");
    for (uint16_t i = 0; i < asmCodes.size(); ++i)
    {
        //макроподстановка
        for (auto it = mapOfDefinitions.begin(); it != mapOfDefinitions.end(); ++it)
        {
            ChangeString(asmCodes[i], it->first, mapOfDefinitions);
        }
        //----------------

        if (regex_match(asmCodes[i], rSum) == 1)
            SumProcessing(i, asmCodes);
        //подстановка макросов
        for (auto it = mapOfMacros.begin(); it != mapOfMacros.end(); ++it)
        {
            if (Split(asmCodes[i], ' ').at(0) == it->first)
            {
                string vars = Split(asmCodes[i], ' ').at(1);
                vars.erase(vars.begin());
                vars.erase(vars.end() - 1);
                vector<string> vecOfVars = Split(vars, ',');
                vector<string> vecOfFormalVars = Split(it->second.at(0), ',');

                //замена формальных переменных
                for (uint16_t c = 1; c != it->second.size(); ++c)
                {

                    for (uint16_t k = 0; k != vecOfVars.size(); ++k)
                    {
                        regex rVar("(\\s)" + vecOfFormalVars[k]);
                        regex rVarMatch("(.*)" + vecOfFormalVars[k] + "(.*)");
                        if (regex_match(it->second.at(c), rVarMatch) == 1)
                        {
                            it->second.at(c) = regex_replace(it->second.at(c), rVar, " " + vecOfVars[k]);
                        }
                    }

                }
                //----------------------------

                //макроподстановки внутри макроса
                asmCodes.erase(asmCodes.begin() + i);
                for (uint16_t i = 1; i != it->second.size(); ++i)
                    for (auto itd = mapOfDefinitions.begin(); itd != mapOfDefinitions.end(); ++itd)
                        ChangeString(it->second.at(i), itd->first, mapOfDefinitions);
                asmCodes.insert(asmCodes.begin() + i, it->second.begin() + 1, it->second.end());

                if (regex_match(asmCodes[i], rSum) == 1)
                    SumProcessing(i, asmCodes);
                //-------------------------------
            }
        }
        //--------------------
    }
}


#endif // MACROASMFUNCTIONS_H_INCLUDED
