#include<iostream>
#include<fstream>
#include<string>

#include"../lib/configuration.h"
#include"../lib/massages.h"
#include"../lib/tape.h"


using namespace std;

Configuration* cfg;

void read_command_line(int argc, char **argv)
{

	// section for reading configuration file name from CL
	int i;

	// is exists custom configuration file?
	for (i = 1; i < argc; i++)
	{
		if (string(argv[i]) == "-c")
		{
			i ++;
			if(i < argc)
				cfg = new Configuration(argv[i]);

			else
			{
				cout << WARNING << "Custom configuration file wasn't set. Default will be used istead" << endl;
				cfg = new Configuration();
			}

		}
		else if (string(argv[i]) == "-h")
		{
			i ++;
			cout << "Help message" << endl;
		}
		else
		{
			cout << "Unknown command line argument: " << argv[i] << endl;
		}
	}
}

int main (int argc, char *argv[])
{
	read_command_line(argc, argv);

	// there aren't custom configuration file
	if (argc == 1)
		cfg = new Configuration();

	cfg->command_line(argc, argv);


	
	Tape<long long unsigned> a (cfg->input_file, cfg->RAM_size);
	a.sort(cfg->output_file);	

	return 0;

}
