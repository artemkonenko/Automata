#include<iostream>
#include "automata.h"

using namespace std;

int main()
{
	eNFA machine("example.txt");
	machine.print_table();

	cout << "Determine:" << endl;
	DFA dmachine = machine.determine();
	dmachine.print_table();


	system("pause");

	return 0;
}