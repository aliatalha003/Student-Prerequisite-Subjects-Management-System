#include "course.h"
using namespace std;

course::course(string Name, int Code, int MaxNum, int Hours, string AcademicYear)
{
    name = Name;
    code = Code;
    maxNum = MaxNum;
    hours = Hours;
    academicYear = AcademicYear;
}