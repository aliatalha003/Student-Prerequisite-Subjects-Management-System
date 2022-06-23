#pragma once
#include<string>
#include <iostream>
#include <vector>
using namespace std;
class course
{
public:
	string name;
	int code;
	int maxNum;
	vector <course> preCourses;
	int hours;
	string academicYear;

	course(string Name, int Code, int MaxNum, int Hours, string AcademicYear);

};