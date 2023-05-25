#include "replace.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <exception>
#include <thread>

namespace fs = std::filesystem;

// class that implemet thread pool pattern for replace symbol templates in files
class ReplThreadPool
{
public:
	ReplThreadPool(const CfgData& cfg_data, const std::vector<fs::path>& file_paths) :
		_cfg_data(cfg_data), _file_paths(file_paths) {}
	// the class can't be copied
	ReplThreadPool(const ReplThreadPool& other) = delete;
	ReplThreadPool(ReplThreadPool&& other) = delete;
	ReplThreadPool& operator=(const ReplThreadPool& other) = delete;
	ReplThreadPool& operator=(ReplThreadPool&& other) = delete;
	~ReplThreadPool();

	void init();

private:
	void run();
	void make_repl_for_one_file(const fs::path& file_path);

private:
	const CfgData& _cfg_data;
	std::vector<fs::path> _file_paths;
	std::vector<std::thread> _threads;
	std::mutex _queue_mtx;
};

ReplThreadPool::~ReplThreadPool()
{
	// wait until all threads finish
	for(auto& thr : _threads)
		thr.join();
}

void ReplThreadPool::init()
{
	_threads.reserve(_cfg_data.num_threads);
	for (int i = 0; i < _cfg_data.num_threads; ++i) {
		_threads.emplace_back(&ReplThreadPool::run, this);
	}
}

void ReplThreadPool::run()
{
	// run proceeds until all files are processed
	while(true)
	{
		std::unique_lock<std::mutex> lock(_queue_mtx);
		if(_file_paths.empty())
			break;

		fs::path fpath = _file_paths.back();
		_file_paths.pop_back();
		lock.unlock();

		make_repl_for_one_file(fpath);
	}

}

void ReplThreadPool::make_repl_for_one_file(const fs::path& file_path)
{
	const std::string& templ = _cfg_data.from_templ;
	const std::string& to_expr = _cfg_data.to_expr;
	std::ifstream file_i(file_path.string());
	if (!file_i.is_open())
	{
		auto err = "Can't open file for template replacement by path: "  + file_path.string();
		throw std::runtime_error(err);
	}

	// get all processed lines in vector
	std::vector<std::string> lines_of_file;
	std::string line;
	while(std::getline(file_i, line))
	{
		size_t pos = 0;
		while ((pos = line.find(templ, pos)) != std::string::npos) 
		{
			line.replace(pos, templ.length(),to_expr);
			pos += to_expr.length();
		}
		lines_of_file.push_back(std::move(line));
	}
	file_i.close();

	std::ofstream file_o(file_path.string());
	if (!file_o.is_open())
	{
		auto err = "Can't open file for template replacement by path: "  + file_path.string();
		throw std::runtime_error(err);
	}

	// write all processed lines back to file
	for(auto& ln : lines_of_file)
		file_o << ln << "\n";

	file_o.close();
}

void make_replacement(const CfgData& cfg_data)
{
	std::vector<fs::path> file_paths;
	fs::recursive_directory_iterator begin(cfg_data.dir_path);
	fs::recursive_directory_iterator end;
	std::copy_if(begin, end, std::back_inserter(file_paths), [](const fs::path& path) {
		return fs::is_regular_file(path);
		});

	ReplThreadPool pool_thr(cfg_data, file_paths);
	pool_thr.init();
}