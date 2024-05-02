#include"../lib/configuration.h"
#include"../lib/massages.h"

#include<fstream>
#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include <sys/stat.h>
#include <filesystem>



Configuration::Configuration(string file_name)
{
	fill_config(file_name);

	if (!is_input_file)
	{
		cout << ERROR << "Input file wasn't set. There is no default value" << endl;
		exit(1);
	}



	if (!verbose)
		return;

	if (!is_verbose)
		cout << WARNING << "Verbose wasn't set. Defaul value will be used instead" << endl;
	if (!is_output_file)
		cout << WARNING << "Output file wasn't set. Defaul value will be used instead" << endl;
	if (!is_RAM_size)
		cout << WARNING << "RAM size wasn't set. Defaul value will be used instead" << endl;

}

void Configuration::fill_config(string file_name)
{
	is_verbose = true;
	is_input_file = false;
	is_output_file = false;


	// setting default values
	verbose = true;
	output_file = "out";

	ifstream cfg (file_name);

	if (!cfg.is_open())
	{
		cout << ERROR << "Error occured while reading configuration file: no such file " << file_name << endl;
		exit(1);
	}

	string line, name, value;
	int line_num = 0;

	while (getline(cfg, line))
	{
		line_num ++;

		remove_comments (line);
		strip_white (line);

		if (line == "") continue;

		split_str (line, name, value);

		if (name == "verbose")
		{
			is_verbose = fill_bool(verbose, value, line_num);
		}
		else if (name == "input_file")
		{
			is_input_file = fill_file(input_file, value, line_num);
		}
		else if (name == "output_file")
		{
			is_output_file = fill_file(output_file, value, line_num, true);
		}
		else if (name == "RAM_size")
		{
			is_RAM_size = my_stoi(RAM_size, value, line_num);
		}
		else 
		{
			cout << WARNING << "Unknown name of parameter at line " << line_num << " of configuration file" << endl;
		}
	}

	cfg.close();
}

void Configuration::command_line(int argc, char** argv)
{
	string buffer, name, value;

	for (int i = 1; i < argc; i++)
	{
		buffer = argv[i];

		if (buffer[0] == '-')
		{
			if (buffer[1] == '-')
				buffer = buffer.substr(2);
			else
				buffer = buffer.substr(1);
		}

		split_str (buffer, name, value);


		if (name == "verbose" || name == "v")
		{
			verbose = true;
			is_verbose = true;
		}
		else if (name == "input_file")
		{
			input_file = fill_file(input_file, value, i);
		}
		else if (name == "output_file")
		{
			is_output_file = fill_file(output_file, value, i);
		}
	}
}

void Configuration::remove_comments(string& line)
{
	size_t n = line.find("#");
	line = line.substr(0, n);
}

void Configuration::strip_white(string& line)
{
	if (line == "" or line.size() == 0) return;

	size_t n = line.size();

	for (size_t i = 0; i < n; i++)
	{
		if (line[i] == ' ' or line[i] == '\t') 
		{
			line.erase(i, 1);
			i--;
			n--;
		}
	}
}

void Configuration::split_str (string& line, string& name, string& value)
{
	size_t pos = line.find('=');

	if (pos > 200) 
	{
		name = line;
		return;
	}

	name = line.substr(0, pos);
	value = line.substr(pos+1);
}

bool Configuration::my_stoi(int& parameter, string str, int line_num)
{
	try
	{
		parameter = stoi(str);
	}
	catch(std::invalid_argument const&)
	{
		if (line_num != 0)
			cout << WARNING << "Unkwnown parameter value at line " << line_num << " of configuration file" << endl;
		else
			cout << WARNING << "Unkwnown parameter value in configuration file" << endl;

		return false;
	}

	return true;
}


bool Configuration::my_stod(double& parameter, string str, int line_num)
{
	try
	{
		parameter = stod(str);
	}
	catch(std::invalid_argument const&)
	{
		if (line_num != 0)
			cout << WARNING << "Unkwnown parameter value at line " << line_num << " of configuration file" << endl;
		else
			cout << WARNING << "Unkwnown parameter value in configuration file" << endl;

		return false;
	}

	return true;
}

bool Configuration::fill_bool (bool& parameter, string value, int line_num)
{
	int v;
	my_stoi(v, value, line_num);

	if (v != 0 and v != 1)
	{
		cout << WARNING << "Unkwnown parameter value at line " << line_num << " of configuration file" << endl;
		return false;
	}

	else if (v == 1)
		parameter = true;
	else if (v == 0)
		parameter = false;

	return true;
}

bool Configuration::fill_directory (string& parameter, string value, int line_num, bool write)
{
	struct stat info;

	if(stat(value.c_str(), &info) != 0)
	{
		if (write)
		{
			filesystem::create_directory(parameter);
			return true;
		}


		cout << WARNING << "Unkwnown parameter value at line " << line_num << " of configuration file" << endl;
		return false;
	}
	else if(!(info.st_mode & S_IFDIR))
	{
		cout << WARNING << value << " is not a directory. Parameter wasn't set" << endl;
		return false;
	}
	else
	{
		parameter = value;
		return true;
	}
}

bool Configuration::fill_file (string& parameter, string value, int line_num, bool write)
{
	struct stat info;

	if (write)
	{
		parameter = value;
		return true;
	}

	if(stat(value.c_str(), &info) != 0)
	{
		cout << WARNING << "File " << value << " from line " << line_num << " doesn't exist"  << endl;
		return false;
	}
	else if(info.st_mode & S_IFDIR)
	{
		cout << WARNING << value << " is a directory. Parameter wasn't set" << endl;
		return false;
	}
	else
	{
		parameter = value;
		return true;
	}
}

