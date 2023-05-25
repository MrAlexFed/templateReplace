#include "load_config.h"
#include <fstream>
#include <exception>
#include <optional>
#include <filesystem>

namespace fs = std::filesystem;

static CfgData read_config_file(std::ifstream& config_file)
{
	CfgData cfg_data;
	while(!config_file.eof())
	{
		std::string key;
		std::string value;
		config_file >> key;
		config_file >> value;

		if(key == "NUM_THREADS")
			cfg_data.num_threads = std::stoi(value);
		else if(key == "DIR_PATH")
		{
			fs::path dir(value);
			if(dir.is_relative())
				dir = fs::current_path() / dir;
			cfg_data.dir_path = dir.string();
		}
		else if(key == "TEMPLATE")
			cfg_data.from_templ = value;
		else if(key == "TO_EXPRESION")
			cfg_data.to_expr = value;
	}
	return cfg_data;
}

static std::optional<std::string> validate_config_data(const CfgData& cfg_data)
{
	std::optional<std::string> err;
	if(cfg_data.num_threads <= 0)
		err = "Config error: NUM_THREADS value invalid";
	else if(cfg_data.dir_path == "")
		err = "Config error: DIR_PATH value empty";
	else if(cfg_data.from_templ == "")
		err = "Config error: TEMPLATE value empty";
	else if(cfg_data.to_expr == "")
		err = "Config error: TO_EXPRESION value empty";

	return err;
}

CfgData load_config()
{
	// config file should be in build directory
	std::ifstream config_file(".//cfg.config");
	if(!config_file.is_open())
		throw std::runtime_error("Can't open cfg.config file");
	
	auto cfg_data = read_config_file(config_file);
	config_file.close();

	if(auto err = validate_config_data(cfg_data); err.has_value())
		throw std::runtime_error(err.value().data());
	
	return cfg_data;
}