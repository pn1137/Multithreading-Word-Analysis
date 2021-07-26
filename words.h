#pragma once

#include <map>
#include <set>
#include <vector>
#include <string>

#if defined (__MINGW32__) || (__MINGW64__)
	#include "mingw std-threads/mingw.mutex.h" // For mingw with win32 threads
#else
	#include <mutex>
#endif

class Words
{
public:
	/** The threshhold for removing a word for having too few occurrences. */
	static constexpr uint_fast8_t COUNT_THRESHHOLD = 5;

	/** A mutex to be used in CountWordsMultithread. */
	static std::mutex WordCount_mutex;

public:
	/**
	 * Counts the number of occurrences of words from files and stores the words and counts into the member map WordCountMap.
	 *
	 * @param filenames A vector of file name strings.
	 * @param filetype A string representing the file extension. (ex. "txt")
	 */
	void CountWords(std::vector<std::string> filenames, std::string filetype);

	/**
	 * Creates threads, divides the files between the threads and assigns a call to CountWordsMultithread to each thread.
	 *
	 * @param filenames A vector of file name strings.
	 * @param filetype A string representing the file extension. (ex. "txt")
	 */
	void startCountWordsMultithread(std::vector<std::string> filenames, std::string filetype);

	/**
	 * Counts the number of occurrences of words from files and stores the words and counts into the member map WordCountMap, multithreaded.
	 *
	 * @param filenames A vector of strings that represent file names.
	 * @param filetype A string representing the file extension. (ex. "txt")
	 */
	void CountWordsMultithread(std::vector<std::string> filenames, std::string filetype);


	// Sort WordCountMap by .second primarily and then by .first
	/**
	 * Compares two pairs of string and unsigned int. The unsigned int is compared first and if equal the string is then compared.
	 * Returns true if the left pair should be sorted before the right pair. Otherwise returns false.
	 *
	 * @note The unsigned int is compared for descending order and the string is compared for ascending order.
	 * @param l The left pair of the comparison.
	 * @param r The right pair of the comparison.
	 */
	static bool CompareCountThenWord(std::pair<std::string, unsigned int> const &l, std::pair<std::string, unsigned int> const &r);

	/**
	 * Places WordCountMap into WordCountVect and then sorts WordCountVect using CompareCountThenWord for comparisons.
	 */
	void SortWordCountMapIntoWordCountVect();

	/**
	 * Writes WordCountVect to file.
	 *
	 * @param outputFileName The name of the file to write to.
	 * @param filetype A string representing the file extension. (ex. "txt")
	 */
	void SaveWordCountVect(std::string outputFileName, std::string filetype);

public:
	/** A map to hold words and their number of occurrences. */
	std::map<std::string, unsigned int> WordCountMap;

	/** A vector used to hold and sort data from WordCountMap. */
	std::vector<std::pair<std::string, unsigned int>> WordCountVect;

	/** The total amount of words. */
	uint_fast64_t TotalWordCount;

	/** The amount of unique words. */
	uint_fast64_t UniqueWordCount;
};
