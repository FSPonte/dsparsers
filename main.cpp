/*
	===== Data Serialization File Parser =====

	Tester code for the tool
*/

// ----- Dependencies -----
#include <iostream>
#include <dotenv.hpp>
#include <dotjson.hpp>

int main(int, char* argv[])
{
	// ----- Environment -----
	{
		std::printf("----- File: %s -----\n", argv[1]);

		dotenv env(argv[1]);
	
		std::cout
			<< "Size: " << env.size() << " elements\n\n"
			<< "API_KEY=" << env["API_KEY"] << '\n'
			<< "HOST=" << env["HOST"] << '\n'
			<< "PORT=" << env["PORT"] << '\n'
			<< "PROTOCOL=" << env["PROTOCOL"] << '\n'
			<< "BASE_URL=" << env["BASE_URL"] << '\n'
			<< std::endl;
	}

	// ----- JSON -----
	{
		std::printf("----- File: %s -----\n", argv[2]);
		dotjson json(argv[2]);
		
		std::cout
			<< "Size: " << json.size() << " elements\n\n"
			<< "app.name: " << json["app.name"] << '\n'
			<< "app.version: " << json["app.version"] << '\n'
			<< "servers[1].host: " << json["servers[1].host"] << '\n'
			<< "servers[0].port: " << json["servers[0].port"] << '\n'
			<< "database.password: " << json["database.password"] << '\n'
			<< "greeting: " << json["greeting"] << '\n'
			<< std::endl;
	}

	return 0;
}
