#include "student.h"
#include "admin.h"

admin a;

vector<course> availableCourses;

student::student()
{
}


student::student(string Name, int id, string pass, string Academic_year) {
	name = Name;
	ID = id;
	password = pass;
	academic_year = Academic_year;
}


void student::Login() {
	string name, pass;
	int index = -1;

	bool found = false;
	do {
		cout << "\nEnter your name : ";
		cin >> name;
		cout << "\nEnter password : ";
		cin >> pass;
		for (int i = 0; i < a.students.size(); i++)
		{
			if (name == a.students[i].name && pass == a.students[i].password)
			{
				found = true;
				index = i; //to determine the index of the student
			}
		}
		if (!found)
			cout << "\nYou have entered an invalid password or username .. Please try again\n";
	} while (!found);

	int option;
	string home;
	do {
		cout << "\n1. View List of all available courses.\n2. View details of a specific course.\n3. Register for a course.\n4. View all my courses.\n5. Change my password.\n\n";
		do {
			cin >> option;

			switch (option)
			{
			case 1:
				viewAvailableCourses(index);
				break;
			case 2:
				viewCourseDetails();
				break;
			case 3:
				registerForCourse(index);
				break;
			case 4:
				viewAllCourses(index);
				break;
			case 5:
				edit(index);
				break;
			default:
				cout << "Invalid number .. Please try again : ";
			}
		} while (option < 1 || option>5);
		cout << "\nDo you want to perform another operation ? (yes/no) : ";
		cin >> home;
		cout << "\n";
	} while (home == "yes");


}




void student::viewAllCourses(int index) {

	cout << "\nFinished courses : ";
	if (a.students[index].finished_courses.size() == 0)
		cout << "None\n";
	else {
		cout << a.students[index].finished_courses[0].name << "\n";
		for (int j = 1; j < a.students[index].finished_courses.size(); j++)
		{
			cout << "                   " << a.students[index].finished_courses[j].name << "\n";
		}
	}
	cout << "\nIn progress courses : ";
	if (a.students[index].courses_in_progress.size() == 0)
		cout << "None\n";
	else {
		cout << a.students[index].courses_in_progress[0].name << "\n";
		for (int j = 1; j < a.students[index].courses_in_progress.size(); j++)
		{
			cout << "                      " << a.students[index].courses_in_progress[j].name << "\n";
		}
	}
	cout << "\n\n";
}

void student::viewCourseDetails()
{
	int courseID, index = -1;


	do {
		cout << "\nEnter course id : ";
		cin >> courseID;

		index = -1;

		for (int i = 0; i < a.courses.size(); i++)
		{
			if (a.courses[i].code == courseID)
			{
				index = i;
				cout << "\nCourse name : " << a.courses[i].name;
				cout << "\nCourse hours : " << a.courses[i].hours << "\n";
			}
		}

		if (index == -1)
			cout << "\nInvalid course ID .. Please try again\n";

	} while (index == -1);



	if (a.courses[index].preCourses.size() > 0)
	{
		cout << "\nCourse prerequisites : \n\n";

		for (int i = 0; i < a.courses[index].preCourses.size(); i++)
		{
			cout << " " << i + 1 << ". " << a.courses[index].preCourses[i].name << "\n";
		}
	}
}

void student::edit(int index)
{
	char* err;
	sqlite3* db;
	sqlite3_stmt* stmt;
	//open database
	sqlite3_open("project.db", &db);

	cout << "\nEnter new password : ";
	string pass;
	cin >> pass;
	cout << "\n                                                    ____________________________________\n";
	cout << "\n                                                Your password is changed successfully!\n";
	cout << "\n                                                    ____________________________________\n";

	//save changes in the database and the vector
	string id = to_string(a.students[index].ID); //convert the id to string
	string update = "UPDATE students SET password='" + pass + "' WHERE id='" + id + "'";
	sqlite3_exec(db, update.c_str(), NULL, NULL, &err);

	a.students[index].password = pass;

	sqlite3_close(db);
}


void student::registerForCourse(int index) {
	char* err;
	sqlite3* db;
	sqlite3_stmt* stmt;
	//open database
	sqlite3_open("project.db", &db);

	int option, num;

	viewAvailableCourses(index);

	cout << "\nHow many courses do you want to take ?\n";

	do {
		cin >> num;
		if (availableCourses.size() - a.students[index].courses_in_progress.size() < num)
			cout << "\nInvalid number .. You can only register for " << availableCourses.size() - a.students[index].courses_in_progress.size() << " course : ";
	} while (availableCourses.size() - a.students[index].courses_in_progress.size() < num || num > availableCourses.size());

	bool inProgress;
	for (int i = 0; i < num; i++)
	{
		do {
			cout << "\nEnter course number " << i + 1 << " : ";

			inProgress = false;
			do {
				cin >> option;
				if (option<1 || option>availableCourses.size())
					cout << "\nPlease enter a valid number : ";
			} while (option<1 || option>availableCourses.size());


			for (int j = 0; j < a.students[index].courses_in_progress.size(); j++)
			{
				if (a.students[index].courses_in_progress[j].code == availableCourses[option - 1].code)
				{
					inProgress = true; //if the course is already in the in-progress vector
					break;
				}
			}

			if (inProgress)
				cout << "\nYou already took this course! Please choose another one\n";


		} while (inProgress == true);

		a.students[index].courses_in_progress.push_back(availableCourses[option - 1]);


		string query = "insert into study VALUES (" + to_string(availableCourses[option - 1].code) + ",'" + to_string(a.students[index].ID) + "','" + "in-progress" + "')";
		sqlite3_exec(db, query.c_str(), NULL, NULL, &err);


	}
	sqlite3_close(db);
}

void student::viewAvailableCourses(int index)
{
	int x = 1;
	cout << "\n\nYour available courses : \n\n";

	if (a.students[index].academic_year == "1st")
	{
		for (int i = 0; i < a.courses.size(); i++)
		{
			if (a.courses[i].academicYear == "1st")
			{
				cout << "   " << x << ". " << a.courses[i].name << "\n";
				x++;
				availableCourses.push_back(a.courses[i]);
			}
		}
	}
	else
	{

		int counter = 0;
		for (int i = 0; i < a.courses.size(); i++)
		{
			counter = 0;

			if (a.courses[i].academicYear == a.students[index].academic_year)
			{
				for (int j = 0; j < a.courses[i].preCourses.size(); j++)
				{
					for (int k = 0; k < a.students[index].finished_courses.size(); k++)
					{
						if (a.courses[i].preCourses[j].code == a.students[index].finished_courses[k].code)
							counter++;
					}
				}

				if (counter == a.courses[i].preCourses.size())
				{
					cout << "   " << i + 1 << ". " << a.courses[i].name << "\n";
					availableCourses.push_back(a.courses[i]);
				}
			}
		}
	}
}
