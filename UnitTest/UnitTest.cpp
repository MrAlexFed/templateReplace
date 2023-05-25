#include "pch.h"
#include "CppUnitTest.h"
#include "..\\templateReplace\\load_config.h"
#include "..\\templateReplace\\replace.h"
#include <filesystem>
#include <fstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace fs = std::filesystem;

namespace UnitTest
{
	TEST_CLASS(UnitTest)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			try
			{
				create_config_file();
				create_test_files_dir();
				auto cfg_data = load_config();
				make_replacement(cfg_data);
				make_checking();
				remove_files();
			}
			catch(const std::exception& err)
			{
				std::wstringstream stream;
				stream << "Exception was thrown: " << err.what();
				Assert::Fail(stream.str().data());
			}
		}

	private:
		void create_config_file();
		void create_test_files_dir();
		void make_checking();
		void remove_files();
	};

	void UnitTest::create_config_file()
	{
		std::string cfg_path = fs::current_path().string() + "\\cfg.config";
		std::ofstream cfg_file(cfg_path);
		if(!cfg_file.is_open())
			throw std::runtime_error(("Can't create cfg.config file by path: " + cfg_path).data());

		cfg_file << "NUM_THREADS 4\n";
		cfg_file << "DIR_PATH testFilesDir\n";
		cfg_file << "TEMPLATE <%COLOR%>\n";
		cfg_file << "TO_EXPRESION BLACK\n";
		cfg_file.close();
	}

	void UnitTest::create_test_files_dir()
	{
		const auto build_path = fs::current_path();
		fs::create_directory(build_path.string() + "\\testFilesDir" );
		fs::create_directory(build_path.string() + "\\testFilesDir\\folder1");
		fs::create_directory(build_path.string() + "\\testFilesDir\\folder1\\folder2");

		fs::path file_path = build_path / fs::path("testFilesDir\\file1.txt");
		std::ofstream file1(file_path.string());
		if(!file1.is_open())
			throw std::runtime_error(("Can't create cfg.config file by path: " + file_path.string()));
		file1 << "I like <%COLOR%> color.\n";
		file1 << "<%COLOR%>, <%COLOR%> ...\n";
		file1.close();

		file_path = file_path.parent_path() / fs::path("file2.txt");
		std::ofstream file2(file_path.string());
		if(!file2.is_open())
			throw std::runtime_error(("Can't create cfg.config file by path: " + file_path.string()).data());
		file2 << "I bought a car last week.\n";
		file2 << "The car is <%COLOR%>.\n";
		file2.close();

		file_path = file_path.parent_path() / fs::path("folder1\\file3.txt");
		std::ofstream file3(file_path.string());
		if(!file3.is_open())
			throw std::runtime_error(("Can't create cfg.config file by path: " + file_path.string()).data());
		file3 << "I want to paint a wall in <%COLOR%> color.\n";
		file3.close();

		file_path = file_path.parent_path() / fs::path("folder2\\file4.txt");
		std::ofstream file4(file_path.string());
		if(!file4.is_open())
			throw std::runtime_error(("Can't create cfg.config file by path: " + file_path.string()).data());
		file4 << "<%COLOR%>, <%COLOR%>, <%COLOR%>\n";
		file4 << "<%COLOR%>, <%COLOR%>, <%COLOR%>\n";
		file4 << "<%COLOR%>, <%COLOR%>, <%COLOR%>\n";
		file4.close();
	}

	void UnitTest::make_checking()
	{
		const auto build_path = fs::current_path();
		std::string line;
		fs::path file_path = build_path / fs::path("testFilesDir\\file1.txt");
		std::ifstream file1(file_path.string());
		if(!file1.is_open())
			throw std::runtime_error(("Can't create cfg.config file by path: " + file_path.string()));
		std::getline(file1, line);
		Assert::AreEqual("I like BLACK color.", line.data());
		std::getline(file1, line);
		Assert::AreEqual("BLACK, BLACK ...", line.data());
		file1.close();

		file_path = file_path.parent_path() / fs::path("file2.txt");
		std::ifstream file2(file_path.string());
		if(!file2.is_open())
			throw std::runtime_error(("Can't create cfg.config file by path: " + file_path.string()).data());
		std::getline(file2, line);
		Assert::AreEqual("I bought a car last week.", line.data());
		std::getline(file2, line);
		Assert::AreEqual("The car is BLACK.", line.data());
		file2.close();
		 
		file_path = file_path.parent_path() / fs::path("folder1\\file3.txt");
		std::ifstream file3(file_path.string());
		if(!file3.is_open())
			throw std::runtime_error(("Can't create cfg.config file by path: " + file_path.string()).data());
		std::getline(file3, line);
		Assert::AreEqual("I want to paint a wall in BLACK color.", line.data());
		file3.close();

		file_path = file_path.parent_path() / fs::path("folder2\\file4.txt");
		std::ifstream file4(file_path.string());
		if(!file4.is_open())
			throw std::runtime_error(("Can't create cfg.config file by path: " + file_path.string()).data());
		std::getline(file4, line);
		Assert::AreEqual("BLACK, BLACK, BLACK", line.data());
		std::getline(file4, line);
		Assert::AreEqual("BLACK, BLACK, BLACK", line.data());
		std::getline(file4, line);
		Assert::AreEqual("BLACK, BLACK, BLACK", line.data());
		file4.close();
	}

	void UnitTest::remove_files()
	{
		fs::remove_all(fs::current_path() / "testFilesDir");
	}
}
