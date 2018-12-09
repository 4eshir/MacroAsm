#include "CommonFunctions.h"
#include "MacroAsmFunctions.h"

using namespace std;

/**

|----------------|
|Макроподстановки|
|----------------|

proc_name MACROS <v1,v2,v3> - макрос, где
                                proc_name - имя макроса
                                v1,v2,v3 - формальные переменные

INCLUDE <filepath> - макроподстановка исходного кода указанного файла по пути filepath

DEF a=b - макроподстановка definition, где a - definition переменная

UNDEF a - макроподстановка undefinition, где a - undefinition переменная

EQU a=5 - макроподстановка константы, где a - имя константы

REP k...ENDREP - макрос повтора, где
                                  k - количество повторений,
                                  ENDREP - конец макроса

SUM rs rf - макрокоманда суммирования регистров [rs;rf]

**/




int main()
{
    vector<string> vs;
 	vs = ReadAsmCodes("macroasm.txt");
 	BuildFile(vs);
 	FileParser_pass1(vs);
 	FileParser_pass2(vs);
 	WriteAsmCodes("asm.txt", vs);
}
