#include "file_utility.h"
#include "extra_utility.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

#if defined (__MINGW32__) || (__MINGW64__)
	#include "mingw std-threads/mingw.thread.h" // For mingw with win32 threads
#else
	#include <thread>
#endif

namespace File_Utility
{
	void GetFilesByNameScheme(std::vector<std::string>& out_filenames_vector, 
		std::string prefix, int numbering, std::string postfix, std::string filetype)
	{
		std::ifstream file;
		std::string filename;
		int num = numbering;

		if (!out_filenames_vector.empty())
		{
			out_filenames_vector.clear();
		}

		while (true)
		{
			filename = prefix + std::to_string(num) + postfix;
			if (!std::ifstream(filename + "." + filetype))
			{
				break;
			}
			
			out_filenames_vector.push_back(filename);
			num += numbering;
		}
	}

	bool isAscii(char c)
	{
		return (c >= 1 && c <= 255);
	}

	bool isApostrophe(char c)
	{
		return (c == '\'');
	}

	void removeHangingApostrophes(std::string& str)
	{
		uint_fast64_t pos = std::string::npos;

		while (true)
		{
			if (pos == std::string::npos)
			{
				pos = str.find("\'");
			}
			else
			{
				pos = str.find("\'", pos + 1);
			}
			
			if (pos == std::string::npos) { break; }

			if (pos > 0)
			{
				if (std::isalpha(str[pos - 1]) && std::isalpha(str[pos + 1]))
				{
					continue;
				}
			}

			str.replace(pos, 1, " ");
		}
	}

	void DatafilesToTxt(std::vector<std::string> filenames, std::string filetype)
	{
		std::fstream file;
		std::stringstream ss;
		std::string str;

		for (std::string filename : filenames)
		{
			file.open(filename + "." + filetype, std::ios::in);

			if (file.is_open())
			{
				std::cout << "Begining conversion of: " << filename << std::endl;
				while (std::getline(file, str))
				{
					// From http://www.cplusplus.com/forum/general/76900/#msg413335
					// If the character is not ascii or is ascii and not apostrophe and not alpha and not space, return true to replace.
					std::replace_if(str.begin(), str.end(), [](char c) { return !isAscii(c) || (isAscii(c) && !isApostrophe(c) && !std::isalpha(c) && !std::isspace(c)); }, ' ');
					std::transform(str.begin(), str.end(), str.begin(), ::tolower );
					removeHangingApostrophes(str);
					ss << str << std::endl;
				}

				file.close();
			}
			else
			{
				std::cout << "File not found: " << filename << std::endl;
				continue;
			}

			// Delete existing text file to rewrite it
			if (std::ifstream(filename + ".txt"))
			{
				std::cout << "File exists, overwriting..." << std::endl;
				remove((filename + ".txt").c_str());
			}

			// Create text file
			file.open(filename + ".txt", std::ios::app);

			// Save stringstream buffer to file
			if (file.is_open())
			{
				file << ss.rdbuf();
				file.close();
				std::cout << "Finished converting: " << filename << std::endl << std::endl;
			}
			else
			{
				std::cout << "Cannot save to file: " << filename << std::endl << std::endl;
			}
		}
	}

	void startDatafilesToTxtMultithread(std::vector<std::string> filenames, std::string filetype)
	{
		std::vector<std::thread> threads;

		uint_fast8_t vect_start = 0;
		uint_fast8_t vect_end = 0;
		uint_fast8_t files_per_thread = filenames.size() / (NUM_THREADS);

		// Create and start threads
		for (int i = 0; i < NUM_THREADS - 1; i++)
		{
			// Split the files into each thread
			vect_start = vect_end;
			vect_end += files_per_thread;

			// Create and start thread for DatafilesToTxtMultithread
			threads.push_back(std::thread(&DatafilesToTxtMultithread, std::vector<std::string>(filenames.begin() + vect_start, filenames.begin() + vect_end), filetype));
		}

		vect_start = vect_end;
		vect_end += files_per_thread;
		threads.push_back(std::thread(&DatafilesToTxtMultithread, std::vector<std::string>(filenames.begin() + vect_start, filenames.begin() + vect_end), filetype));

		for (auto &t : threads)
		{
			t.join();
		}
	}

	void DatafilesToTxtMultithread(std::vector<std::string> filenames, std::string filetype)
	{
		std::fstream file;
		std::stringstream ss;
		std::string str;

		for (std::string filename : filenames)
		{
			file.open(filename + "." + filetype, std::ios::in);

			if (file.is_open())
			{
				// Lock for std::cout
				DatafilesToTxt_mutex.lock();
				std::cout << "Begining conversion of: " << filename << std::endl;
				DatafilesToTxt_mutex.unlock();

				while (std::getline(file, str))
				{
					// From http://www.cplusplus.com/forum/general/76900/#msg413335
					// If the character is not ascii or is ascii and not apostrophe and not alpha and not space, return true to replace.
					std::replace_if(str.begin(), str.end(), [](char c) { return !isAscii(c) || (isAscii(c) && !isApostrophe(c) && !std::isalpha(c) && !std::isspace(c)); }, ' ');
					std::transform(str.begin(), str.end(), str.begin(), ::tolower);
					removeHangingApostrophes(str);
					ss << str << std::endl;
				}

				file.close();
			}
			else
			{
				// Lock for std::cout
				DatafilesToTxt_mutex.lock();
				std::cout << "File not found: " << filename << std::endl;
				DatafilesToTxt_mutex.unlock();

				continue;
			}

			// Delete existing text file to rewrite it
			if (std::ifstream(filename + ".txt"))
			{
				// Lock for std::cout
				DatafilesToTxt_mutex.lock();
				std::cout << "File exists, overwriting..." << std::endl;
				DatafilesToTxt_mutex.unlock();

				remove((filename + ".txt").c_str());
			}

			// Create text file
			file.open(filename + ".txt", std::ios::app);

			// Save stringstream buffer to file
			if (file.is_open())
			{
				file << ss.rdbuf();
				file.close();

				// Lock for std::cout
				DatafilesToTxt_mutex.lock();
				std::cout << "Finished converting: " << filename << std::endl << std::endl;
				DatafilesToTxt_mutex.unlock();
			}
			else
			{
				// Lock for std::cout
				DatafilesToTxt_mutex.lock();
				std::cout << "Cannot save to file: " << filename << std::endl << std::endl;
				DatafilesToTxt_mutex.unlock();
			}
		}
	}
}
