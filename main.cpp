#include "CommonFunctions.h"
#include "MacroAsmFunctions.h"

using namespace std;

/**

|----------------|
|����������������|
|----------------|

proc_name MACROS <v1,v2,v3> - ������, ���
                                proc_name - ��� �������
                                v1,v2,v3 - ���������� ����������

INCLUDE <filepath> - ���������������� ��������� ���� ���������� ����� �� ���� filepath

DEF a=b - ���������������� definition, ��� a - definition ����������

UNDEF a - ���������������� undefinition, ��� a - undefinition ����������

EQU a=5 - ���������������� ���������, ��� a - ��� ���������

REP k...ENDREP - ������ �������, ���
                                  k - ���������� ����������,
                                  ENDREP - ����� �������

SUM rs rf - ������������ ������������ ��������� [rs;rf]

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
