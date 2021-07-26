#include "words.h"
#include "extra_utility.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>	// stable_sort
#include <iterator>

#if defined (__MINGW32__) || (__MINGW64__)
	#include "mingw std-threads/mingw.thread.h" // For mingw with win32 threads
#else
	#include <thread>
#endif

std::mutex Words::WordCount_mutex;

void Words::CountWords(std::vector<std::string> filenames, std::string filetype)
{
	// Reset member variables
	TotalWordCount = 0;
	UniqueWordCount = 0;
	std::map<std::string, unsigned int>().swap(WordCountMap);	// Ensure clear and 0 capacity

	std::ifstream file;
	for (std::string filename : filenames)
	{
		if (std::ifstream(filename + "." + filetype))
		{
			file.open(filename + "." + filetype, std::ios::in);

			if (file.is_open())
			{
				std::cout << "File opened: " + filename << std::endl;

				std::string word;
				uint_fast64_t file_word_count = 0;
				std::map<std::string, unsigned int>::iterator it;

				while (file.good())
				{
					file >> word;
					TotalWordCount++;
					file_word_count++;

					// From https://stackoverflow.com/a/101980
					// Get potential pair position
					it = WordCountMap.lower_bound(word);

					// Check if word is in the map
					// if iterator is within allocated range and if iterator key and word match (key_comp returns false if match)
					if (it != WordCountMap.end() && !(WordCountMap.key_comp()(word, it->first)))
					{
						// Increment the count of the word
						it->second++;
					}
					else
					{
						// Add word to the map
						WordCountMap.insert(it, std::map<std::string, unsigned int>::value_type(word, 1));
					}
				}

				file.close();

				std::cout	<< "File closed: " + filename << std::endl
							<< "Number of Words in " + filename + ": " << file_word_count << std::endl;
			}
		}
	}

	// Update UniqueWordCount
	UniqueWordCount = WordCountMap.size();

	std::cout	<< "Finished counting words from files." << std::endl
				<< "Preliminary Unique Word Count: " << UniqueWordCount << std::endl
				<< "Preliminary Total Word Count: " << TotalWordCount << std::endl;
}

void Words::startCountWordsMultithread(std::vector<std::string> filenames, std::string filetype)
{
	std::vector<std::thread> threads;

	// Reset member variables
	TotalWordCount = 0;
	UniqueWordCount = 0;
	std::map<std::string, unsigned int>().swap(WordCountMap);	// Ensure clear and 0 capacity

	uint_fast8_t vect_start = 0;
	uint_fast8_t vect_end = 0;
	uint_fast8_t files_per_thread = filenames.size() / (NUM_THREADS);
	uint_fast64_t files_per_thread_rem = filenames.size() % (NUM_THREADS);

	// Create and start threads
	for (int i = 0; i < NUM_THREADS - 1; i++)
	{
		// Split the files into each thread
		vect_start = vect_end;
		if (files_per_thread_rem > 0)
		{
			vect_end += files_per_thread + 1;
			--files_per_thread_rem;
		} else
		{
			vect_end += files_per_thread;
		}		

		// Create and start thread for CountWordsMultithread with this object
		threads.push_back(std::thread(&Words::CountWordsMultithread, this, std::vector<std::string>(filenames.begin() + vect_start, filenames.begin() + vect_end), filetype));
	}

	vect_start = vect_end;
	vect_end += files_per_thread;
	threads.push_back(std::thread(&Words::CountWordsMultithread, this, std::vector<std::string>(filenames.begin() + vect_start, filenames.begin() + vect_end), filetype));

	for (auto &t : threads)
	{
		t.join();
	}

	// Update UniqueWordCount
	UniqueWordCount = WordCountMap.size();

	std::cout	<< "Finished counting words from files." << std::endl
				<< "Preliminary Unique Word Count: " << UniqueWordCount << std::endl
				<< "Preliminary Total Word Count: " << TotalWordCount << std::endl;
}

void Words::CountWordsMultithread(std::vector<std::string> filenames, std::string filetype)
{
	std::map<std::string, unsigned int> temp_word_count_map;
	uint_fast64_t temp_total_word_count = 0;
	std::ifstream file;

	for (std::string filename : filenames)
	{
		if (std::ifstream(filename + "." + filetype))
		{
			file.open(filename + "." + filetype, std::ios::in);

			if (file.is_open())
			{
				// Lock for std::cout
				WordCount_mutex.lock();
				std::cout << "File opened: " + filename << std::endl;
				WordCount_mutex.unlock();

				std::string word;
				uint_fast64_t file_word_count = 0;
				std::map<std::string, unsigned int>::iterator it;

				while (file.good())
				{
					file >> word;
					temp_total_word_count++;
					file_word_count++;

					// From https://stackoverflow.com/a/101980
					// Get potential pair position
					it = temp_word_count_map.lower_bound(word);

					// Check if word is in the map
					// if iterator is within allocated range and if iterator key and word match (key_comp returns false if match)
					if (it != temp_word_count_map.end() && !(temp_word_count_map.key_comp()(word, it->first)))
					{
						// Increment the count of the word
						it->second++;
					}
					else
					{
						// Add word to the map
						temp_word_count_map.insert(it, std::map<std::string, unsigned int>::value_type(word, 1));
					}
				}

				file.close();

				// Lock for std::cout
				WordCount_mutex.lock();
				std::cout	<< "File closed: " + filename << std::endl
							<< "Number of Words in " + filename + ": " << file_word_count << std::endl;
				WordCount_mutex.unlock();
			}
		}
	}

	// Lock the remainder of the function
	std::lock_guard<std::mutex> lock_count(WordCount_mutex);

	if (WordCountMap.empty())
	{
		WordCountMap = std::move(temp_word_count_map);
	}
	else
	{
		std::map<std::string, unsigned int>::iterator it;

		// Merge the temporary map with WordCountMap
		for (auto word_count : temp_word_count_map)
		{
			// From https://stackoverflow.com/a/101980
			// Get potential pair position
			it = WordCountMap.lower_bound(word_count.first);

			
			// Check if iterator is within allocated range and if iterator key and pair key match
			if (it != WordCountMap.end() && !(WordCountMap.key_comp()(word_count.first, it->first)))
			{
				it->second += word_count.second;
			}
			else
			{
				// Insert pair if key is not found
				WordCountMap.insert(it, word_count);
			}
		}
	}

	// Update TotalWordCount
	TotalWordCount += temp_total_word_count;
}

bool Words::CompareCountThenWord(std::pair<std::string, unsigned int> const &l, std::pair<std::string, unsigned int> const &r)
{
	// Sort counts in descending order
	if (r.second != l.second)
	{
		return r.second < l.second;
	}

	// Sort words of equal count in ascending order
	return l.first < r.first;
}

void Words::SortWordCountMapIntoWordCountVect()
{
	std::copy(WordCountMap.begin(), WordCountMap.end(), std::back_inserter<std::vector<std::pair<std::string, unsigned int>>>(WordCountVect));
	std::stable_sort(WordCountVect.begin(), WordCountVect.end(), CompareCountThenWord);
}

void Words::SaveWordCountVect(std::string outputFileName, std::string filetype)
{
	std::ofstream file;

	// Rewrite existing file
	if (std::ifstream(outputFileName + "." + filetype))
	{
		remove((outputFileName + "." + filetype).c_str());
	}

	// Save word count to a temporary file
	file.open(outputFileName + "_temp" + "." + filetype, std::ios::app);

	if (file.is_open())
	{
		for (auto word_count : WordCountVect)
		{
			// Remove words from being written if they aren't common enough
			if (word_count.second < COUNT_THRESHHOLD)
			{
				// Update statistics
				TotalWordCount -= word_count.second;
				UniqueWordCount--;

				continue;
			}

			// Count followed by word saved to temporary file
			file << std::fixed << std::setw(10) << word_count.second << "\t" << word_count.first << std::endl;
		}

		file.close();
	}

	// Include updated statistics at the top of the file and append with temporary word count file
	file.open(outputFileName + "." + filetype, std::ios::app);

	if (file.is_open())
	{
		std::ifstream temp_file;
		temp_file.open(outputFileName + "_temp" + "." + filetype, std::ios::in);

		if (temp_file.is_open())
		{
			file << "[Unique Word Count: " << UniqueWordCount << "]" << std::endl;
			file << "[Total Word Count: " << TotalWordCount << "]" << std::endl;

			file << temp_file.rdbuf();

			temp_file.close();
		}

		// Delete temporary word count file
		if (std::ifstream(outputFileName + "_temp" + "." + filetype))
		{
			remove((outputFileName + "_temp" + "." + filetype).c_str());
		}

		file.close();
	}
}
