#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include<string>
#include<vector>

using namespace std;

class Configuration 
{
	public:
		Configuration(string file_name = "default.cfg");
		void command_line(int argc, char** argv);

		bool verbose;
		bool is_verbose;

		int RAM_size;
		bool is_RAM_size;


		string input_file;
		bool is_input_file;

		string output_file;
		bool is_output_file;



	private:
		void fill_config (string file_name);
		void remove_comments (string& line);
		void strip_white (string& line);
		void split_str (string& line, string& name, string& value);

		bool my_stoi (int& parameter, string, int line_num = 0);
		bool my_stod (double& parameter, string, int line_num = 0);

		bool fill_bool (bool& parameter, string value, int line_num);
		bool fill_directory (string& parameter, string value, int line_num, bool write = false);
		bool fill_file (string& parameter, string value, int line_num, bool write = false);

};

#endif
