#include"../lib/tape.h"

#include<iostream>
#include<fstream>
#include <algorithm>
#include <numeric>
#include <sys/stat.h>
#include <filesystem>





using namespace std;

template <class T> Tape<T>::Tape(string input_file, size_t RAM_size_in, bool read)
{	
	RAM_size = RAM_size_in;
	path = input_file;

	// coubt the number of lines
	ifstream file(input_file);
	if(!file)
		throw "The file " + input_file + "doesn't exist";

	size_t number_of_lines = 0;
	string line;

	while (getline(file, line))
		if (line != "")
			++number_of_lines;


	file.clear();
	file.seekg(0);

	size = number_of_lines;
	
	if (read)
	{
		numbers = vector<T>(size);

		for (size_t i = 0; i < size; ++i)
		{
			getline(file, line);
			numbers.at(i) = stod(line);
		}

		file.close();
	}
	else
		numbers = vector<T>(0);
}

template <class T> Tape<T>::Tape(string path_in, vector<T> numbers_in, size_t RAM_size_in)
{	
	numbers = vector<T> (numbers_in.size());
	numbers = numbers_in;

	size = numbers_in.size();
	RAM_size = RAM_size_in;

	path = path_in;

	ofstream file (path);
	for (size_t k = 0; k < size; ++k)
		file << numbers.at(k) << endl;
	file.close();
}

// unsafe function!!!
// be aware of using it
template<class T> void Tape<T>::split()
{
	size_t n_files = size/(RAM_size/3);
	vector<T> temp_vec (RAM_size/3);

	size_t pos = 0;

	for (size_t i = 0; i < n_files; ++i)
	{

		temp_vec = load(pos, pos+temp_vec.size());
		Tape<T> a ("tmp/" + to_string(i), temp_vec, RAM_size);

		pos += a.size;
	}

	if (size % temp_vec.size() > 0)
	{
		
		temp_vec = load(pos, size);
		Tape<T> a ("tmp/" + to_string(n_files), temp_vec, RAM_size);
	}
}

template<class T> int Tape<T>::sort(string output_file)
// sort given array with restrition on RAM usage
{
	size_t n_files = size/(RAM_size/3);
	vector<T> temp_vec (2 * (RAM_size/3));
	size_t n = temp_vec.size();
	size_t residual = size % (RAM_size/3);

	Tape<T> *tape;

	if (size % temp_vec.size() > 0)
		n_files += 1;

	// create directory tmp, if it doesnt exist
	struct stat info;
	if(stat("tmp", &info ) != 0)
	       std::filesystem::create_directory("tmp");

	// split big tape into a set of small temporal tapes
	split();

	vector<bool> is_sorted_btw(n_files-1);
	bool flag = false;


	while (!flag)
	{
		for (size_t k = 0; k < n_files-2; ++k)
		{
			// load two temporal tapes into RAM (temp_vec)
			tape = new Tape<T> ("tmp/" + to_string(k), RAM_size, true);
			for (size_t i = 0; i < n/2; ++i)
				temp_vec.at(i) = tape->numbers.at(i);

			tape = new Tape<T> ("tmp/" + to_string(k+1), RAM_size, true);
			for (size_t i = 0; i < n/2; ++i)
				temp_vec.at(i+n/2) = tape->numbers.at(i);

			// check if the two tapes sorted
			// sort and save them
			is_sorted_btw.at(k) = is_sorted(temp_vec.begin(), temp_vec.end());

			if (!is_sorted_btw.at(k))
			{
				std::sort(temp_vec.begin(), temp_vec.begin()+n);

				tape = new Tape<T> ("tmp/" + to_string(k), 
						vector<T> (temp_vec.begin(), temp_vec.begin()+n/2), RAM_size);
				tape = new Tape<T> ("tmp/" + to_string(k+1), 
						vector<T> (temp_vec.begin()+n/2, temp_vec.begin()+n), RAM_size);
			}

		}
		
		// same procedure but for the last tape wich are smaller
		tape = new Tape<T> ("tmp/" + to_string(n_files-2), RAM_size, true);
		for (size_t i = 0; i < n/2; ++i)
			temp_vec.at(i) = tape->numbers.at(i);

		tape = new Tape<T> ("tmp/" + to_string(n_files-1), RAM_size, true);
		for (size_t i = 0; i < residual; ++i)
			temp_vec.at(i+n/2) = tape->numbers.at(i);


		is_sorted_btw.at(n_files-2) = is_sorted(temp_vec.begin(), temp_vec.begin() + n/2 + residual);

		if (!is_sorted_btw[n_files-1])
		{
			std::sort(temp_vec.begin(), temp_vec.begin()+n/2+residual);

			tape = new Tape<T> ("tmp/" + to_string(n_files-2), 
					vector<T> (temp_vec.begin(), temp_vec.begin()+n/2), RAM_size);
			tape = new Tape<T> ("tmp/" + to_string(n_files-1), 
					vector<T> (temp_vec.begin()+n/2, temp_vec.begin()+n/2+residual), RAM_size);
		}


		// repeat the process until all files are mutually sorted
		flag = accumulate(is_sorted_btw.begin(), is_sorted_btw.end(), true, multiplies<bool>());
	}

	
	// read all temporal files and save them to a big file
	ofstream file (output_file);

	for (size_t k = 0; k < n_files-1; ++k)
	{
		tape = new Tape<T> ("tmp/" + to_string(k), RAM_size, true);

		for (auto w: tape->numbers)
			file << w << endl;
	}

	tape = new Tape<T> ("tmp/" + to_string(n_files-1), RAM_size, true);

	for (auto w: tape->numbers)
		file << w << endl;


	file.close();
	delete tape;

	return 0;
}






template <class T> vector<T> Tape<T>::load(size_t begin, size_t end)
// load the range of positions of the tape to RAM
{
	if (end < begin)
		end = begin;

	if ((end - begin) > RAM_size)
		throw "Can't load tape from " +
		       	to_string(begin) + " to " + to_string(end) + ". Size grater than RAM";

	vector<T> result(end - begin);

	ifstream file(path);
	string line;

	for (size_t i = 0; i < begin; ++i)
		file.ignore(1000, '\n');

	for (size_t i = 0; i < result.size(); ++i)
	{
		getline(file, line);
		result.at(i) = stod(line);
	}
	
	file.close();

	return result;
}

// possible data types
template class Tape<signed char>;
template class Tape<unsigned char>;
template class Tape<short int>;
template class Tape<unsigned short int>;
template class Tape<int>;
template class Tape<unsigned int>;
template class Tape<long int>;
template class Tape<unsigned long int>;
template class Tape<long long int>;
template class Tape<unsigned long long int>;
template class Tape<float>;
template class Tape<double>;
template class Tape<long double>;
