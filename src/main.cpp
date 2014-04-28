#include <iostream>
#include <getopt.h>

#include "automata.h"

using namespace std;

int main( int argc, char** args )
{
	char c;
	while ((c = getopt (argc, args, "ha:i:o:r:w:")) != -1)
		switch (c)
		{
			case 'i': // Input file src
				freopen(optarg, "r", stdin);
		        break;
			case 'r': // Type of input automata
				cout << "This option coming soon..." << endl;
				break;
			case 'o': // Output file src
				freopen(optarg, "w", stdout);
		        break;
			case 'w': // Type of output automata
				cout << "This option coming soon..." << endl;
				break;
			case 'h':
				cout << "Usage: automata [-i input_filepath] [-o output_filepath]"<< endl;
		        cout << "-h for this help."<< endl;
		        return 0;
		        break;
			case '?':
				if ( optopt == 'i' || optopt == 'o')
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
		        return 1;
			default:
				abort ();
		}

	eNFA machine(cin);

	machine.print_table();

	cout << "Determine:" << endl;
	DFA dmachine = machine.determine();
	dmachine.print_table();

	return 0;
}