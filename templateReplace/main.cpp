#include <iostream>
#include "load_config.h"
#include "replace.h"

int main()
{
	try
	{
		auto cfg_data = load_config();
		make_replacement(cfg_data);
	}
	catch(const std::runtime_error& err)
	{
		std::cout << "Error: " << err.what() << "\n";
		return 1;
	}

	std::cout << "The work have been done!\n";
	return 0;
}