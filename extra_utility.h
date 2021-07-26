#pragma once

#include <chrono>

#if defined (__MINGW32__) || (__MINGW64__)
	#include "mingw std-threads/mingw.thread.h" // For mingw with win32 threads
#else
	#include <thread>
#endif

/** The number of threads that the machine has. */
static uint_fast8_t NUM_THREADS = ((std::thread::hardware_concurrency() == 0) ? 1 : std::thread::hardware_concurrency());

/** A typedef for a time_point using high_resolution_clock. */
typedef std::chrono::time_point<std::chrono::high_resolution_clock> time_point_type;

/**
 * Returns the difference of two time_point_types in milliseconds.
 *
 * @param begin The start time as a time_point_type.
 * @param end The end time as a time_point_type.
 */
inline int64_t timer(time_point_type& begin, time_point_type& end)
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
}
