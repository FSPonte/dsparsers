/*
	===== Data Serialization File Parser =====

	Tester code for the tool
*/

// ----- Dependencies -----
#include <iostream>
#include "dotenv.hpp"

int main(int argc, char* argv[])
{
	// ----- Program arguments -----
	{
		std::printf("----- Passed arguments -----\n");

		for (int i = 1; i < argc; ++i)
			std::cout << argv[i] << '\n';
	
		std::printf("-----------------------------\n\n");

		if (argc == 1)
			return 0;
	}

	dotenv env(argv[1]);
	
	std::cout
		<< "Size: " << env.size() << " elements\n\n"
		<< "API_KEY=" << env["API_KEY"] << '\n'
		<< "HOST=" << env["HOST"] << '\n'
		<< "PORT=" << env["PORT"] << '\n'
		<< "PROTOCOL=" << env["PROTOCOL"] << '\n'
		<< "BASE_URL=" << env["BASE_URL"] << '\n'
		<< std::flush;

	return 0;
}
