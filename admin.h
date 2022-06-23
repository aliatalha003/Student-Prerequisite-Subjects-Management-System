#pragma once
#include "course.h"
#include "student.h"
#include "sqlite/sqlite3.h"

class admin
{
public:
	string name = "admin";
	string password = "admin123";

	vector<student> students;
	vector<course> courses;

	admin();
	void logIn();
	void addStudent();
	void addCourse();
	void addPreCourses(int num, course c);
	void viewStudents();
	void viewCourses();
	void edit();
	void editStudentCourseStatus();
};
