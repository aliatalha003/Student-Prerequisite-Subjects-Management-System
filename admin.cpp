#include "admin.h"

char* err;
sqlite3* db;
sqlite3_stmt* stmt;
sqlite3_stmt* stmt2;


admin::admin() {
	//opening database
	sqlite3_open("project.db", &db);

	//making sure that tables are created
	sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS courses(name TEXT,id INT PRIMARY KEY,numOfStudents INT,courseHours INT,academic_year TEXT)", NULL, NULL, &err);
	sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS students(name VARCHAR(50) ,id INT PRIMARY KEY,password VARCHAR(50),academicYear VARCHAR(50) )", NULL, NULL, &err);
	sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS study(course_code INT,stud_id INT ,status Text,FOREIGN KEY(course_code) REFERENCES courses(id),FOREIGN KEY(stud_id) REFERENCES students(id))", NULL, NULL, &err);
	sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS prerequisites(id INT, pre_course TEXT,FOREIGN KEY(id) REFERENCES courses(id))", NULL, NULL, &err);

	//accessing courses table 
	sqlite3_prepare_v2(db, "select name, id, numOfStudents,courseHours,academic_year from courses", -1, &stmt, 0);

	//stroing courses in courses vector
	while (sqlite3_step(stmt) != SQLITE_DONE) {
		const unsigned char* name = sqlite3_column_text(stmt, 0);
		int id = sqlite3_column_int(stmt, 1);
		int num = sqlite3_column_int(stmt, 2);
		int hours = sqlite3_column_int(stmt, 3);
		const unsigned char* academicYear = sqlite3_column_text(stmt, 4);

		string sName((char*)name);
		string sYear((char*)academicYear);

		course currentCourse(sName, id, num, hours, sYear); //first make an object then push it in the vector

		courses.push_back(currentCourse);
	}

	int j = 0; //counter which start from 0 to store prerequisites for each course

	//nested loop to store prerequisites for each course
	while (sqlite3_step(stmt) != SQLITE_DONE) {

		string id = to_string(sqlite3_column_int(stmt, 1)); //this formula to access columns which has data type of integer .. columns are also zero based 

		int index = -1;

		//accessing prerequisites table but for a specific course

		string select = "select id, pre_course from prerequisites where id='" + id + "'";
		sqlite3_prepare_v2(db, select.c_str(), -1, &stmt2, 0);


		while (sqlite3_step(stmt2) != SQLITE_DONE) {

			const unsigned char* name2 = sqlite3_column_text(stmt2, 1); //this formula to access columns which has data type of char
			string sName2((char*)name2); //to convert from char to string

			for (int i = 0; i < courses.size(); i++)
			{
				if (sName2 == courses[i].name)
				{
					index = i;
					break;
				}
			}
			if (index != -1)
				courses[j].preCourses.push_back(courses[index]);
		}

		j++; //increment the counter
	}

	//accessing students table
	sqlite3_prepare_v2(db, "select name, id,password,academicYear from students", -1, &stmt, 0);

	//store students in studnets vector
	while (sqlite3_step(stmt) != SQLITE_DONE) {
		const unsigned char* name3 = sqlite3_column_text(stmt, 0);
		int id = sqlite3_column_int(stmt, 1);
		const unsigned char* password = sqlite3_column_text(stmt, 2);
		const unsigned char* academicYear = sqlite3_column_text(stmt, 3);

		string sName3((char*)name3);
		string sPass((char*)password);
		string sAcademic = ((char*)academicYear);

		student stud(sName3, id, sPass, sAcademic);//first make an object then push it in the vector
		students.push_back(stud);

	}

	int index = -1;

	for (int i = 0; i < students.size(); i++)
	{
		string select = "select course_code, stud_id,status from study where stud_id='" + to_string(students[i].ID) + "'";
		sqlite3_prepare_v2(db, select.c_str(), -1, &stmt, 0);

		while (sqlite3_step(stmt) != SQLITE_DONE) {
			int id = sqlite3_column_int(stmt, 0);
			const unsigned char* status = sqlite3_column_text(stmt, 2);
			string sStatus = ((char*)status);


			for (int j = 0; j < courses.size(); j++)
			{
				if (courses[j].code == id)
					index = j;
			}

			course c = courses[index];

			if (sStatus == "finished")
				students[i].finished_courses.push_back(c);
			else
				students[i].courses_in_progress.push_back(c);
		}
	}
	sqlite3_close(db);
}



void admin::logIn()
{

	string username, pass;
	do {
		cout << "\nEnter username : ";
		cin >> username;
		cout << "\nEnter password : ";
		cin >> pass;
		if (username != name || pass != password)
			cout << "\nYou have entered an invalid password or username .. Please try again\n";
	} while (username != name || pass != password);

	int option;
	string home;
	do {
		cout << "\n1. Add a new student.\n2. Add a new course.\n3. View List of all students in a specific course.\n4. View List of all courses (Finished - Progressed) of a specific student.\n5. Edit a course data.\n6. Edit a student courses status \n\n";
		do {
			cin >> option;
			switch (option)
			{
			case 1:
				addStudent();
				break;
			case 2:
				addCourse();
				break;
			case 3:
				viewStudents();
				break;
			case 4:
				viewCourses();
				break;
			case 5:
				edit();
				break;
			case 6:
				editStudentCourseStatus();
				break;
			default:
				cout << "\nInvalid number .. You must enter a number in this range (1-6) : ";
			}

		} while (option < 1 || option>6);
		cout << "\nDo you want to perform another operation ? (yes/no) : ";
		cin >> home;
		cout << "\n";
	} while (home == "yes");
}

void admin::addStudent()
{

	sqlite3_open("project.db", &db);

	string studName, studPass, academicYear;
	int studId;

	//take student data
	cout << "\nEnter student name : ";
	cin >> studName;
	cout << "\nEnter student id : ";
	cin >> studId;
	cout << "\nEnter student password : ";
	cin >> studPass;
	do {
		cout << "\nEnter student academic year : ";
		cin >> academicYear;
		if (academicYear != "1st" && academicYear != "2nd" && academicYear != "3rd" && academicYear != "4th")
			cout << "\nInvalid academic year .. Please try again\n";
	} while (academicYear != "1st" && academicYear != "2nd" && academicYear != "3rd" && academicYear != "4th");

	//a pure sql query to add a new row in students table (must be a string)

	string query = "insert into students VALUES ('" + studName + "'," + to_string(studId) + ",'" + studPass + "','" + academicYear + "')";
	//to execute the query 
	sqlite3_exec(db, query.c_str(), NULL, NULL, &err);

	//make an object then push it
	student stud(studName, studId, studPass, academicYear);
	students.push_back(stud);

	cout << "\n                                                     ____________________\n";
	cout << "\n                                                     Added Successfully!!\n";
	cout << "                                                     ____________________\n";


	sqlite3_close(db);
}

void admin::addCourse()
{
	//open database
	sqlite3_open("project.db", &db);

	//create courses table

	//take course data from user
	string courseName, academicYear;
	int courseId, numOfStudents, num, courseHours;
	cout << "\nEnter course name : ";
	cin >> courseName;
	cout << "\nEnter course code : ";
	cin >> courseId;
	cout << "\nEnter maximum number of students : ";
	cin >> numOfStudents;
	cout << "\nEnter course hours : ";
	cin >> courseHours;
	do {
		cout << "\nEnter academic year : ";
		cin >> academicYear;
		if (academicYear != "1st" && academicYear != "2nd" && academicYear != "3rd" && academicYear != "4th")
			cout << "\nInvalid academic year .. Please try again\n";
	} while (academicYear != "1st" && academicYear != "2nd" && academicYear != "3rd" && academicYear != "4th");
	//save course in database
	string query = "insert into courses VALUES ('" + courseName + "'," + to_string(courseId) + "," + to_string(numOfStudents) + "," + to_string(courseHours) + ",'" + academicYear + "')";
	sqlite3_exec(db, query.c_str(), NULL, NULL, &err);

	course newCourse(courseName, courseId, numOfStudents, courseHours, academicYear);

	//checking if this course has prerequisites or not 
	cout << "\nDoes this course require courses? (yes/no) ";
	string require;
	cin >> require;
	if (require == "yes") {
		cout << "\nEnter number of required courses : ";
		cin >> num;
		addPreCourses(num, newCourse);
	}
	courses.push_back(newCourse);


	cout << "\n                                                     ____________________\n";
	cout << "\n                                                     Added Successfully!!\n";
	cout << "                                                     ____________________\n";


	sqlite3_close(db);
}

void admin::addPreCourses(int num, course newCourse)
{
	//open database
	sqlite3_open("project.db", &db);
	cin.ignore();

	string course;
	int index;
	for (int i = 1; i <= num; i++)
	{
		do {
			index = -1;
			cout << "\nEnter course num " << i << " :";
			getline(cin, course); //enter the name of prerequisite course
			for (int j = 0; j < courses.size(); j++) {
				if (course == courses[j].name)
				{
					index = j;
					break;
				}
			}
			if (index == -1)  //checking if the course is in courses vector
				cout << "\nCoures is not available , Please try again\n";
		} while (index == -1);


		//save a prerequisite for a specific course
		string query = "insert into prerequisites VALUES (" + to_string(newCourse.code) + " , '" + courses[index].name + "')";
		sqlite3_exec(db, query.c_str(), NULL, NULL, &err);

		//push the preCourse in the vector also
		newCourse.preCourses.push_back(courses[index]);
	}
	sqlite3_close(db);
}

void admin::viewStudents()
{
	//open database
	sqlite3_open("project.db", &db);

	int courseID;
	bool validCourse = false;

	do {
		cout << "\nEnter course id : ";
		cin >> courseID;
		for (int i = 0; i < courses.size(); i++)
		{
			if (courses[i].code == courseID) {
				validCourse = true;
				break;
			}
		}
		if (!validCourse)
			cout << "\nInvalid course ID .. Please try again\n";
	} while (!validCourse);


	//accesing study table to view all students who study a specific course 
	string select = "select course_code, stud_id from study where course_code='" + to_string(courseID) + "'";
	sqlite3_prepare_v2(db, select.c_str(), -1, &stmt, 0);

	cout << "\n";
	bool noStudents = true;
	while (sqlite3_step(stmt) != SQLITE_DONE) {

		int studID = sqlite3_column_int(stmt, 1); //accessing studID column

		for (int i = 0; i < students.size(); i++)
		{
			if (students[i].ID == studID)
			{
				cout << "Name : " << students[i].name << "\n";
				cout << "Academic year : " << students[i].academic_year << "\n\n";
				noStudents = false;
			}
		}
	}
	if (noStudents)
		cout << "There is no student that takes this course\n";
	sqlite3_close(db);
}

void admin::viewCourses()
{
	sqlite3_open("project.db", &db);
	int studID;
	int index;
	bool validID = false;

	do {
		cout << "\nEnter student id : ";
		cin >> studID;
		for (int i = 0; i < students.size(); i++)
		{
			if (students[i].ID == studID)
			{
				index = i;
				validID = true;
				break;
			}
		}
		if (!validID)
			cout << "\nInvalid ID .. Please try again\n";
	} while (!validID);
	//student s;
	//s.viewAllCourses(index);
	//accesing study table to view all courses which a specific student studies 
	string select = "select course_code, stud_id ,status from study where stud_id='" + to_string(studID) + "'";
	sqlite3_prepare_v2(db, select.c_str(), -1, &stmt, 0);


	bool noCourses = true;
	cout << "\n";
	while (sqlite3_step(stmt) != SQLITE_DONE) {

		int courseID = sqlite3_column_int(stmt, 0);//accessing courseID column
		const unsigned char* status = sqlite3_column_text(stmt, 2);
		string sStatus = ((char*)status);

		for (int i = 0; i < courses.size(); i++)
		{
			if (courses[i].code == courseID)
			{
				cout << courses[i].name << " " << courses[i].code;
				if (sStatus == "finished")
					cout << "       'finished'";
				else
					cout << "       'In-Progress'";
				cout << "\n";
				noCourses = false;
			}
		}
	}

	if (noCourses)
		cout << "\nThis student has no courses yet\n";
	sqlite3_close(db);
	

}

void admin::edit()
{
	//open database
	sqlite3_open("project.db", &db);

	string update;
	int courseID, index = -1;
	bool validCourse = false;

	do {
		cout << "\nEnter course ID : ";
		cin >> courseID;
		for (int i = 0; i < courses.size(); i++)
		{
			if (courses[i].code == courseID) {
				validCourse = true;
				break;
			}
		}
		if (!validCourse)
			cout << "\nInvalid course ID .. Please try again\n";
	} while (!validCourse);
	//loop to determine the index of the course 
	for (int i = 0; i < courses.size(); i++)
	{
		if (courses[i].code == courseID)
		{
			index = i;
			break;
		}
	}


	cout << "\nCourse name : " << courses[index].name << "\n";
	cout << "Course hours : " << courses[index].hours << "\n";
	cout << "Course capacity : " << courses[index].maxNum << "\n";

	cout << "Course prerequisites : ";

	if (courses[index].preCourses.size() == 0)
		cout << "None\n";
	else
	{
		cout << courses[index].preCourses[0].name << "\n";
		for (int i = 1; i < courses[index].preCourses.size(); i++)
		{
			cout << "                       " << courses[index].preCourses[i].name << "\n";
		}
	}
	cout << "\n1. Edit maximum number of students.\n2. Edit list of pre-required courses.\n3. Edit hours.\n\n";
	int option;
	do {
		cin >> option;

		switch (option)
		{
		case 1:

			cout << "\nEnter the new number : ";
			int newNum;
			cin >> newNum;
			//update in the database & the vector
			update = "UPDATE courses SET numOfStudents='" + to_string(newNum) + "' WHERE id= '" + to_string(courseID) + "'";
			sqlite3_exec(db, update.c_str(), NULL, NULL, &err);
			courses[index].maxNum = newNum;
			break;
		case 2:
			cout << "\nEnter number of new pre-required courses : ";
			int num;
			cin >> num;
			addPreCourses(num, courses[index]);
			break;
		case 3:
			cout << "\nEnter new number of hours : ";
			int hours;
			cin >> hours;
			//update in the database & the vector
			update = "UPDATE courses SET courseHours='" + to_string(hours) + "' WHERE id= '" + to_string(courseID) + "'";
			sqlite3_exec(db, update.c_str(), NULL, NULL, &err);
			courses[index].hours = hours;
			break;
		default:
			cout << "\nInvalid number .. Please try again : \n";
		}
	} while (option < 1 || option>3);
	cout << "                                                     _______________________\n";
	cout << "\n                                                     Changed Successfully !!\n";
	cout << "                                                     _______________________\n";
	sqlite3_close(db);
}

void admin::editStudentCourseStatus()
{
	sqlite3_open("project.db", &db);
	int studID, index = -1;

	do {
		cout << "\nEnter student ID : ";
		index = -1;
		cin >> studID;
		cout << "\n";
		for (int i = 0; i < students.size(); i++)
		{
			if (students[i].ID == studID)
				index = i;
		}
		if (index == -1)
			cout << "\nInvalid ID .. Please try again\n";
	} while (index == -1);

	if (students[index].courses_in_progress.size() == 0)
		cout << "\nThis student doesn't have courses in-progress\n";
	else {

		for (int i = 0; i < students[index].courses_in_progress.size(); i++)
		{
			cout << " " << i + 1 << ". " << students[index].courses_in_progress[i].name << "\n";
		}

		int num;
		cout << "\nHow many courses did the student finish ? \n";
		do{
		cin >> num;
		if (num<1 || num>students[index].courses_in_progress.size())
			cout << "\nInvalid number .. You can only choose " << students[index].courses_in_progress.size() << " course : ";
		} while (num<1 || num>students[index].courses_in_progress.size());
		int option;
		for (int i = 0; i < num; i++)
		{

			cout << "\nEnter course number " << i + 1 << " : ";

			do {
				cin >> option;
				if (option < 1 || option > students[index].courses_in_progress.size())
					cout << "Enter a valid number : ";
			} while (option < 1 || option > students[index].courses_in_progress.size());

			students[index].finished_courses.push_back(students[index].courses_in_progress[option - 1]);
			string status = "finished";
			string update = " UPDATE study SET status='" + status + "' WHERE course_code='" + to_string(students[index].courses_in_progress[option - 1].code) + "'";
			sqlite3_exec(db, update.c_str(), NULL, NULL, &err);

			students[index].courses_in_progress.erase(students[index].courses_in_progress.begin() + (option - 1));

			cout << "\n";

			for (int i = 0; i < students[index].courses_in_progress.size(); i++)
			{
				cout << " " << i + 1 << ". " << students[index].courses_in_progress[i].name << "\n";
			}
		}
	}
	sqlite3_close(db);
}

