#pragma once
#include "course.h"
class student
{
public:
	string name;
	int ID;
	string password;

	vector<course> finished_courses;
	vector<course> courses_in_progress;

	string academic_year;

	student();
	student(string Name, int id, string pass, string Academic_year);
	void Login();
	void registerForCourse(int index);
	void viewAvailableCourses(int index);
	void viewAllCourses(int index);
	void viewCourseDetails();
	void edit(int index);
};

