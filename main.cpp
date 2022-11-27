// FIZZBUZZ, THREADED + PIPED
// created by Zaine Rehman
// finished 11-25-22

#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <string>
#include <vector>
#include <array>

/*
FIZZBUZZ IMPLEMENTATION (results):
[piped, 8 threads, set array]

1000 (1k) 
{
	TIME TAKEN (all)       : 0.009713
	TIME TAKEN (write)     : 0.000011
	TIME TAKEN (calculate) : 0.009701
}
10000 (10k) 
{
	TIME TAKEN (all)       : 0.008102
	TIME TAKEN (write)     : 0.000416
	TIME TAKEN (calculate) : 0.007686
}
100000 (100k) 
{
	TIME TAKEN (all)       : 0.015296
	TIME TAKEN (write)     : 0.003952
	TIME TAKEN (calculate) : 0.011344
}
1000000 (1m) 
{
	TIME TAKEN (all)       : 0.047675
	TIME TAKEN (write)     : 0.030515
	TIME TAKEN (calculate) : 0.017160
}
10000000 (10m)
{
	TIME TAKEN (all)       : 0.485799
	TIME TAKEN (write)     : 0.411582
	TIME TAKEN (calculate) : 0.074217
}
100000000 (100m)
{
	TIME TAKEN (all)       : 4.883397
	TIME TAKEN (write)     : 3.999266
	TIME TAKEN (calculate) : 0.884131
}
1000000000 (1b)
{
	TIME TAKEN (all)       : 60.792217
	TIME TAKEN (write)     : 37.654944
	TIME TAKEN (calculate) : 23.137273
}
*/

/***************************************************/

// fizzbuzz ceiling
static constexpr uint64_t FB_CEIL = 10000000;

// amount of threads going to be used
// supports 1-8 and 16
static constexpr uint8_t THREADS_TO_USE = 8;

// type of fizzbuzz implementation
// 0 = counters, 1 = set array, 2 = modulus
// 1 generally yields best results
static constexpr uint8_t FB_TYPE = 2;

/***************************************************/

typedef std::numeric_limits<double> DOUBLE_LIM;

// max number of threads
static const uint8_t MAXTHREADS = std::thread::hardware_concurrency();

// size of each section
static uint64_t part = FB_CEIL / THREADS_TO_USE;


// FB_CEIL split up into sections for each thread
static std::array<uint64_t, (THREADS_TO_USE + 1)> sections;
// each thread fills up a value for later file output
static std::array<std::string, THREADS_TO_USE> sectionStr;

void FB_inrange(
	uint64_t bottom,
	const uint64_t& top,
	const uint16_t strIndex,
	const uint8_t& type
)
{
	if (bottom == 0) bottom = 1;
	// return string
	std::string rString;

	switch (type)
	{
		// counters method
	case 0:
	{
		// counters
		uint8_t c3 = bottom % 3,
				c5 = bottom % 5;
		bool caught;
		for (uint64_t i = bottom; i < top; ++i, ++c3, ++c5)
		{
			caught = false;
			// div. by 3
			if (c3 == 3 || c3 == 0) {
				rString += "Fizz";
				caught = true;
				c3 = 0;
			}
			// div. by 5
			if (c5 == 5 || c5 == 0) {
				rString += "Buzz";
				caught = true;
				c5 = 0;
			}
			if (!caught) {
				rString += std::to_string(i);
			}
			rString += '\n';
		}
	}
	break;

	// set array
	case 1:
	{
		std::array<uint8_t, 15> setArr{ 0,0,1,0,2,1,0,0,1,2,0,1,0,0,3 };
		uint8_t arrLoc = bottom - 2 % 15;
		for (uint64_t i = bottom; i < top; ++i)
		{
			++arrLoc;
			if (arrLoc == 15) arrLoc = 0;

			switch (setArr[arrLoc]) {
			case 0:
				rString += std::to_string(i);
				break;
			case 1:
				rString += "Fizz";
				break;
			case 2:
				rString += "Buzz";
				break;
			case 3:
				rString += "FizzBuzz";
				break;
			}
			rString += '\n';
		}
	}
	break;

	// modulus
	case 2:
	{
		for (uint64_t i = bottom; i < top; ++i)
		{
			if (!(i % 15)) rString += "FizzBuzz";
			else if (!(i % 3))  rString += "Fizz";
			else if (!(i % 5))  rString += "Buzz";
			else                rString += std::to_string(i);
		}
	}
	break;
	}

	sectionStr[strIndex] = rString;
	return;
}

int main() 
{
	if (MAXTHREADS < THREADS_TO_USE) {
		std::cout 
			<< "U_ERROR: \nmain() -> \n" 
			<< std::to_string(__LINE__) 
			<< " -> \n [MAXTHREADS < THREADS_TO_USE] number of threads requested is greater than amount of supported threads -> \nending program\n";
		return -1;
	}
	// set cout decimal precision
	std::setprecision(DOUBLE_LIM::max_digits10);
	// supposedly makes stdout faster
	std::ios_base::sync_with_stdio(false);

	// timepoint 1, all
	auto TP1 = std::chrono::high_resolution_clock::now();
	
	std::ofstream out("output.txt");
	// save output buffer
	std::streambuf* coutbuf = std::cout.rdbuf();
	// redirect cout to file
	std::cout.rdbuf(out.rdbuf());
	
	// fill up sections
	sections[0] = 1;
	for (uint8_t i = 1; i < sections.size() - 1; ++i) sections[i] = i * part;
	sections[sections.size() - 1] = FB_CEIL + 1;

	std::vector<std::thread> activeThreads;

	// make the threads do stuff
	// dear lord i am sorry for what i am about to write
	if (THREADS_TO_USE == 1) {
		std::thread FB_T1(FB_inrange, sections[0], sections[1], 0, FB_TYPE);
		FB_T1.join();
	}
	if (THREADS_TO_USE == 2) {
		std::thread FB_T1(FB_inrange, sections[0], sections[1], 0, FB_TYPE);
		std::thread FB_T2(FB_inrange, sections[1], sections[2], 1, FB_TYPE);
		FB_T1.join(); FB_T2.join();
	}
	if (THREADS_TO_USE == 3) {
		std::thread FB_T1(FB_inrange, sections[0], sections[1], 0, FB_TYPE);
		std::thread FB_T2(FB_inrange, sections[1], sections[2], 1, FB_TYPE);
		std::thread FB_T3(FB_inrange, sections[2], sections[3], 2, FB_TYPE);
		FB_T1.join(); FB_T2.join(); FB_T3.join();
	}
	if (THREADS_TO_USE == 4) {
		std::thread FB_T1(FB_inrange, sections[0], sections[1], 0, FB_TYPE);
		std::thread FB_T2(FB_inrange, sections[1], sections[2], 1, FB_TYPE);
		std::thread FB_T3(FB_inrange, sections[2], sections[3], 2, FB_TYPE);
		std::thread FB_T4(FB_inrange, sections[3], sections[4], 3, FB_TYPE);
		FB_T1.join(); FB_T2.join(); FB_T3.join(); FB_T4.join();
	}
	if (THREADS_TO_USE == 5) {
		std::thread FB_T1(FB_inrange, sections[0], sections[1], 0, FB_TYPE);
		std::thread FB_T2(FB_inrange, sections[1], sections[2], 1, FB_TYPE);
		std::thread FB_T3(FB_inrange, sections[2], sections[3], 2, FB_TYPE);
		std::thread FB_T4(FB_inrange, sections[3], sections[4], 3, FB_TYPE);
		std::thread FB_T5(FB_inrange, sections[4], sections[5], 4, FB_TYPE);
		FB_T1.join(); FB_T2.join(); FB_T3.join(); FB_T4.join(); FB_T5.join();
	}
	if (THREADS_TO_USE == 6) {
		std::thread FB_T1(FB_inrange, sections[0], sections[1], 0, FB_TYPE);
		std::thread FB_T2(FB_inrange, sections[1], sections[2], 1, FB_TYPE);
		std::thread FB_T3(FB_inrange, sections[2], sections[3], 2, FB_TYPE);
		std::thread FB_T4(FB_inrange, sections[3], sections[4], 3, FB_TYPE);
		std::thread FB_T5(FB_inrange, sections[4], sections[5], 4, FB_TYPE);
		std::thread FB_T6(FB_inrange, sections[5], sections[6], 5, FB_TYPE);
		FB_T1.join(); FB_T2.join(); FB_T3.join(); FB_T4.join(); FB_T5.join(); FB_T6.join();
	}
	if (THREADS_TO_USE == 7) {
		std::thread FB_T1(FB_inrange, sections[0], sections[1], 0, FB_TYPE);
		std::thread FB_T2(FB_inrange, sections[1], sections[2], 1, FB_TYPE);
		std::thread FB_T3(FB_inrange, sections[2], sections[3], 2, FB_TYPE);
		std::thread FB_T4(FB_inrange, sections[3], sections[4], 3, FB_TYPE);
		std::thread FB_T5(FB_inrange, sections[4], sections[5], 4, FB_TYPE);
		std::thread FB_T6(FB_inrange, sections[5], sections[6], 5, FB_TYPE);
		std::thread FB_T7(FB_inrange, sections[6], sections[7], 6, FB_TYPE);
		FB_T1.join(); FB_T2.join(); FB_T3.join(); FB_T4.join(); FB_T5.join(); FB_T6.join(); FB_T7.join();
	}
	if (THREADS_TO_USE == 8) {
		std::thread FB_T1(FB_inrange, sections[0], sections[1], 0, FB_TYPE);
		std::thread FB_T2(FB_inrange, sections[1], sections[2], 1, FB_TYPE);
		std::thread FB_T3(FB_inrange, sections[2], sections[3], 2, FB_TYPE);
		std::thread FB_T4(FB_inrange, sections[3], sections[4], 3, FB_TYPE);
		std::thread FB_T5(FB_inrange, sections[4], sections[5], 4, FB_TYPE);
		std::thread FB_T6(FB_inrange, sections[5], sections[6], 5, FB_TYPE);
		std::thread FB_T7(FB_inrange, sections[6], sections[7], 6, FB_TYPE);
		std::thread FB_T8(FB_inrange, sections[7], sections[8], 7, FB_TYPE);
		FB_T1.join(); FB_T2.join(); FB_T3.join(); FB_T4.join(); FB_T5.join(); FB_T6.join(); FB_T7.join(); FB_T8.join();
	}
	if (THREADS_TO_USE == 16) {
		std::thread FB_T1(FB_inrange, sections[0], sections[1], 0, FB_TYPE);
		std::thread FB_T2(FB_inrange, sections[1], sections[2], 1, FB_TYPE);
		std::thread FB_T3(FB_inrange, sections[2], sections[3], 2, FB_TYPE);
		std::thread FB_T4(FB_inrange, sections[3], sections[4], 3, FB_TYPE);
		std::thread FB_T5(FB_inrange, sections[4], sections[5], 4, FB_TYPE);
		std::thread FB_T6(FB_inrange, sections[5], sections[6], 5, FB_TYPE);
		std::thread FB_T7(FB_inrange, sections[6], sections[7], 6, FB_TYPE);
		std::thread FB_T8(FB_inrange, sections[7], sections[8], 7, FB_TYPE);
		std::thread FB_T1(FB_inrange, sections[8], sections[9], 8, FB_TYPE);
		std::thread FB_T2(FB_inrange, sections[9], sections[10], 9, FB_TYPE);
		std::thread FB_T3(FB_inrange, sections[10], sections[11], 10, FB_TYPE);
		std::thread FB_T4(FB_inrange, sections[11], sections[12], 11, FB_TYPE);
		std::thread FB_T5(FB_inrange, sections[12], sections[13], 12, FB_TYPE);
		std::thread FB_T6(FB_inrange, sections[13], sections[14], 13, FB_TYPE);
		std::thread FB_T7(FB_inrange, sections[14], sections[15], 14, FB_TYPE);
		std::thread FB_T8(FB_inrange, sections[15], sections[16], 15, FB_TYPE);
		FB_T1.join(); FB_T2.join(); FB_T3.join(); FB_T4.join(); FB_T5.join(); FB_T6.join(); FB_T7.join(); FB_T8.join();
	}

	// FB_inrange(sections[0], sections[1], 0, FB_TYPE);

	// write to file
	// timepoint 1, write
	auto TPW1 = std::chrono::high_resolution_clock::now();
	for (std::string i : sectionStr) {
		std::cout << i;
	}
	// timepoint 2, write
	auto TPW2 = std::chrono::high_resolution_clock::now();

	// timepoint 2, all
	auto TP2 = std::chrono::high_resolution_clock::now();

	// time between tp 1 and 2
	auto timeTaken      = std::chrono::duration_cast<std::chrono::duration<double>> (TP2  - TP1 );
	auto timeTakenWrite = std::chrono::duration_cast<std::chrono::duration<double>> (TPW2 - TPW1);

	std::cout << "TIME TAKEN (all)       : " << std::to_string(timeTaken     .count())					   << '\n';
	std::cout << "TIME TAKEN (write)     : " << std::to_string(timeTakenWrite.count())					   << '\n';
	std::cout << "TIME TAKEN (calculate) : " << std::to_string(timeTaken.count() - timeTakenWrite.count()) << '\n';

	// reset cout
	std::cout.rdbuf(coutbuf);

	std::cout << "TIME TAKEN (all)       : " << std::to_string(timeTaken	 .count())					   << '\n';
	std::cout << "TIME TAKEN (write)     : " << std::to_string(timeTakenWrite.count())					   << '\n';
	std::cout << "TIME TAKEN (calculate) : " << std::to_string(timeTaken.count() - timeTakenWrite.count()) << '\n';
	return 0;
}
