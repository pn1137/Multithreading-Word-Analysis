#include "words.h"
#include "file_utility.h"
#include "extra_utility.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>


int main ()
{
	/** Delcare time_point_type variables (std::chrono::time_point<std::chrono::high_resolution_clock>) */
	time_point_type start = std::chrono::high_resolution_clock::now();
	time_point_type start_ToTxt;
	time_point_type end_ToTxt;
	time_point_type end_ToTxtMultithread;
	time_point_type start_WordCount;
	time_point_type end_WordCount;
	time_point_type end_WordCountMultithread;
	time_point_type end_SortIntoVect;
	time_point_type end_SaveVect;

	/** Declare file name containers */
	std::vector<std::string> fileList;

	/** Declare a Words instance */
	Words words;

	/** 
	 * Declare a filestream to save duration results, 
	 * initialize the file name as a string and 
	 * declare a stringstream to output duration results 
	 */
	std::ofstream duration_file;
	std::string duration_filename = "_results/timings.txt";
	std::stringstream duration_sstream;
	

	/** Populate fileList with file names */
	//File_Utility::GetFilesByNameScheme(fileList, "_data/train_", 250, "k_clean", "csv");

	/** DatafilesToTxt */
	start_ToTxt = std::chrono::high_resolution_clock::now();	
	//File_Utility::DatafilesToTxt(fileList, "csv");
	end_ToTxt = std::chrono::high_resolution_clock::now();

	/** DatafilesToTxtMultithread */
	//File_Utility::startDatafilesToTxtMultithread(fileList, "csv");
	end_ToTxtMultithread = std::chrono::high_resolution_clock::now();

	/** Populate fileList with file names */
	File_Utility::GetFilesByNameScheme(fileList, "_data/train_", 250, "k_clean", "txt");

	/** CountWords */
	start_WordCount = std::chrono::high_resolution_clock::now();
	//words.CountWords(fileList, "txt");
	end_WordCount = std::chrono::high_resolution_clock::now();

	/** CountWordsMultithread */
	words.startCountWordsMultithread(fileList, "txt");
	end_WordCountMultithread = std::chrono::high_resolution_clock::now();

	/** SortWordCountMapIntoWordCountVect */
	words.SortWordCountMapIntoWordCountVect();
	end_SortIntoVect = std::chrono::high_resolution_clock::now();

	/** SaveWordCountVect */
	words.SaveWordCountVect("_results/train_word_count_5threads", "txt");
	end_SaveVect = std::chrono::high_resolution_clock::now();

	/** Save to file and output the timing results */
	duration_file.open(duration_filename, std::ios::out);
	if (duration_file.is_open())
	{
		duration_sstream 
			<< std::setw(60) << "Time to convert and clean csv to txt: " << timer(start_ToTxt, end_ToTxt) << " ms" << std::endl
			<< std::setw(60) << "Time to convert and clean csv to txt with multithreading: " << timer(end_ToTxt, end_ToTxtMultithread) << " ms" << std::endl
			<< std::setw(60) << "Time to count words: " << timer(start_WordCount, end_WordCount) << " ms" << std::endl
			<< std::setw(60) << "Time to count words with multithreading: " << timer(end_WordCount, end_WordCountMultithread) << " ms" << std::endl
			<< std::setw(60) << "Time to sort map into vect: " << timer(end_WordCountMultithread, end_SortIntoVect) << " ms" << std::endl
			<< std::setw(60) << "Time to save vect to txt: " << timer(end_SortIntoVect, end_SaveVect) << " ms" << std::endl
			<< std::setw(60) << "Total time: " << timer(start, end_SaveVect) << " ms" << std::endl;

		std::cout << std::endl << duration_sstream.str() << std::endl;

		duration_file << duration_sstream.rdbuf();

		std::cout << "The timing results have been saved to a file: " << duration_filename << std::endl;
	}

	return 0;
}
