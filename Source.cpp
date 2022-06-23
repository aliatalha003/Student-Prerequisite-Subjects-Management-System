#include<iostream>
#include"student.h"
#include"admin.h"
#include"course.h"

using namespace std;
int main() {
	string x;
	system("COLOR F1");
	do {
		admin a;
		student stud;
		int option;
		cout << "1. Log in as a student\n\n2. Log in as an admin\n\n";

		do {
			cin >> option;
			switch (option)
			{
			case 1:
				stud.Login();
				break;
			case 2:
				a.logIn();
				break;
			default:
				cout << "\nInvalid number .. Please try again : ";
			}
		} while (option < 1 || option>2);
		cout << "                                                      _______________               \n\n";
		cout << "                                                        LOGGED OUT                   \n";
		cout << "                                                      _______________                 \n";
		cout << "\nDo you want to log in again? (yes/no) :";
		cin >> x;
		cout << endl;
	} while (x == "yes");
	return 0;
}