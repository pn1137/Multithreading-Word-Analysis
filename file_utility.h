#pragma once

#include <vector>
#include <string>

#if defined (__MINGW32__) || (__MINGW64__)
	#include "mingw std-threads/mingw.mutex.h" // For mingw with win32 threads
#else
	#include <mutex>
#endif

namespace File_Utility
{
	/** A mutex to be used in DatafilesToTxtMultithread. */
	static std::mutex DatafilesToTxt_mutex;

	/**
	 * Modifies a vector of strings to include all file names that follow an incremental numbering scheme.
	 *
	 * @note Example: prefix = "test_", numbering = 100, postfix = "_k", filetype = "csv"
	 * @note results in test_100_k.csv, test_200_k.csv, etc.
	 * @param out_filenames_vector A vector of strings to modify with file names.
	 * @param prefix A string representing the beginning of a file naming scheme.
	 * @param numbering The number to increment by.
	 * @param postfix A string representing the end of a file naming scheme.
	 * @param filetype A string representing the file extension.
	 */
	void GetFilesByNameScheme(std::vector<std::string>& out_filenames_vector,
		std::string prefix, int numbering, std::string postfix, std::string filetype);

	/**
	 * Returns true if the input character is an ASCII character. Otherwise returns false.
	 *
	 * @param c The character to check for being an ASCII character.
	 */
	bool isAscii(char c);

	/**
	 * Returns true if the input character is an ASCII apostrophe. Otherwise returns false.
	 *
	 * @param c The character to check for being an apostrophe.
	 */
	bool isApostrophe(char c);

	/**
	 * Remove apostrophes in a string that are proceeded or followed by a space.
	 *
	 * @param str The string to search for and remove hanging apostrophes.
	 */
	void removeHangingApostrophes(std::string& str);

	/**
	 * Converts and cleans a data file into a text file with only alphabetic words (including apostrophes).
	 *
	 * @note The words are not checked for spelling errors.
	 * @param filenames A vector of file name strings.
	 * @param filetype A string representing the file extension. (ex. "csv")
	 */
	void DatafilesToTxt(std::vector<std::string> filenames, std::string filetype);

	/**
	 * Creates threads, divides the files between the threads and assigns a call to DatafilesToTxtMultithread to each thread.
	 *
	 * @param filenames A vector of file name strings.
	 * @param filetype A string representing the file extension. (ex. "txt")
	 */
	void startDatafilesToTxtMultithread(std::vector<std::string> filenames, std::string filetype);

	/**
	 * Converts and cleans a data file into a text file with only alphabetic words (including apostrophes), multithreaded.
	 *
	 * @note The words are not checked for spelling errors.
	 * @param filenames A vector of file name strings.
	 * @param filetype A string representing the file extension. (ex. "txt")
	 */
	void DatafilesToTxtMultithread(std::vector<std::string> filenames, std::string filetype);
}
