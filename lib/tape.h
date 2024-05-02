#ifndef TAPE_H
#define TAPE_H

#include<vector>
#include<string>

using namespace std;

template <class T> class Tape
{
	public:
		Tape(string input_file, size_t RAM_size_in, bool read = false);
		Tape(string path, vector<T> numbers, size_t RAM_size_in);

		vector<T> numbers;


		int sort(string output_file);
		vector<T> load(size_t begin, size_t end = 0);

	private:
		size_t RAM_size;
		size_t size;
		string path;

		void split();
};

#endif
