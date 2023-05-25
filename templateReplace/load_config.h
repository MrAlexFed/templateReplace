#pragma once
#include <string>

struct CfgData
{
	int num_threads = -1;
	std::string dir_path;
	std::string from_templ;
	std::string to_expr;
};

CfgData load_config();
