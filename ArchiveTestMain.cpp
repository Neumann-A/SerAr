#include <cstdlib>
#include <iostream>
#include <complex>
#include <array>
#include <vector>
#include <stdio.h>


#include "NamedValue.h"
#include "ConfigFile_Archive.h"
#include "Matlab_Archive.h"

//Wrapper Archives
#include "XMLArchiveWrapper.h"
#include "JSONArchiveWrapper.h"
#include "BinaryArchiveWrapper.h"
#include "PortableBinaryArchiveWrapper.h"

class InsideTestClass
{
public:
	std::string d{ "what the fuck" };
	std::vector<double> arr{ { 5.3E0,-9.6E3,1.6E-7} };
public:
	template <typename T>
	void save(Archives::OutputArchive<T>& ar) const
	{
		ar(Archives::createNamedValue("string", d), Archives::createNamedValue("vector", arr));
	}
};

class ConfigTestClass
{
public:
	double d{ 0.1023E-8 };
	int i{ 23 };
	InsideTestClass test;
	//std::complex<double> comp{ d,i };
	//std::tuple<int, double> tuple{ 23,0.2 };
public:
	template <typename T>
	void save(Archives::OutputArchive<T>& ar) const
	{
		ar(Archives::createNamedValue("doubleone", d), Archives::createNamedValue("inttwo", i), Archives::createNamedValue("InsideTest", test));
	}
};


class NoArchiveClass
{
private:
	double d;
	int i;
};


int main(int, char**)
{
	std::cout << "Build date: " << __DATE__ << " Time: " << __TIME__ << " CPP: " << __cplusplus << std::endl ;
	std::cout << "Last Source Changes: " << __TIMESTAMP__ << std::endl << std::endl;

	std::experimental::filesystem::path mypath{ "Test.mat" };
	using namespace Archives;
	//typedef ConfigFile_OutputArchive ArchiveType;
	typedef MatlabOutputArchive ArchiveType;
	//typedef ConfigFile_OutputArchive ArchiveType;
	//typedef ConfigFile_InputArchive InArchiveType;
	{
		ArchiveType CFG{ mypath, Archives::MatlabOptions::write_v73 };
		ConfigTestClass CFGTEST{};
		//NoArchiveClass NoAr{};

		//using TestClass = std::complex<double>;
		//static_assert(Archives::traits::use_archive_member_save_v<TestClass, ArchiveType>, "Archive cannot save the Type");
		//static_assert(Archives::traits::has_archive_member_to_string_v<ConfigFile::toString, TestClass>, "Helper does not have to string");
		//static_assert(!Archives::traits::has_type_to_string_v<TestClass>, "Type does not have to_string");
		//static_assert(Archives::traits::use_to_string_v<TestClass, ConfigFile::toString, ArchiveType>, "Archive does not use to string");

		//static_assert(Archives::traits::no_type_save_v<TestClass, ArchiveType>, "Archive does have type save2");
		//
		//static_assert(!Archives::traits::has_member_to_string_v<TestClass>, "Has member to string");
		//static_assert(!Archives::traits::has_member_save_v<TestClass, ArchiveType>, "Has Member Save");
		//static_assert(!Archives::traits::has_func_save_v<ConfigTestClass, ArchiveType>, "Has Function Save");
		//static_assert(!Archives::traits::has_member_serialize_v<TestClass, ArchiveType>, "Has Member Serialize");
		//static_assert(!Archives::traits::has_func_serialize_v<ArchiveType, TestClass>, "Has Function Serialize");
		//static_assert(Archives::traits::no_type_save_v<TestClass, ArchiveType>, "Type has some save function");
		//static_assert(!Archives::traits::not_any_save_v<TestClass, ArchiveType>, "Nobody has a clue");

		std::cout << "Archive Test Begin" << std::endl;
		CFG(Archives::createNamedValue("FirstSection", CFGTEST));

		std::pair<double, double> Pairtest{ 0.215,0.31 };

		using TestType = decltype(Archives::createNamedValue("FirstSection", CFGTEST));
	}

	std::string s1{ "test" };
	std::string s2{ std::move(s1) };
	std::cout << "string s1 is empty? " << s1.empty() << std::endl;
	////ArchiveType is ConfigFile_Out
	////TestType should be NamedValue<ConfigTestClass &>
	//static_assert(Archives::traits::use_archive_member_save_v<TestType, ArchiveType>, "No Archive Save");
	//static_assert(!Archives::traits::has_member_save_v<TestType, ArchiveType>, "Has Member Save");
	//static_assert(!Archives::traits::has_func_save_v<TestType, ArchiveType>, "Has Function Save");
	//static_assert(!Archives::traits::has_member_serialize_v<TestType, ArchiveType>, "Has Member Serialize");
	//static_assert(!Archives::traits::has_func_serialize_v<ArchiveType, TestType>, "Has Function Serialize");
	//static_assert(Archives::traits::no_type_save_v<TestType, ArchiveType>, "Type does know how to save");
	//static_assert(!Archives::traits::not_any_save_v<TestType, ArchiveType>, "Nobody has a clue");
	////TestType::Type should be ConfigTestClass &
	//static_assert(!Archives::traits::use_archive_member_save_v<TestType::Type, ArchiveType>, "Uses Archive Save");
	//static_assert(!Archives::traits::use_to_string_v<TestType::Type, ArchiveType, ConfigFile::toString>, "Has to string");
	//static_assert(!Archives::traits::has_member_to_string_v<TestType::Type>, "Has member to string");
	//static_assert(Archives::traits::has_member_save_v<TestType::Type, ArchiveType>, "No Member Save");
	//static_assert(!Archives::traits::has_func_save_v<ConfigTestClass, ArchiveType>, "Has Function Save");
	//static_assert(!Archives::traits::has_member_serialize_v<TestType::Type, ArchiveType>, "Has Member Serialize");
	//static_assert(!Archives::traits::has_func_serialize_v<ArchiveType, TestType::Type>, "Has Function Serialize");
	//static_assert(!Archives::traits::no_type_save_v<TestType::Type, ArchiveType>, "Type does not have a clue to save");
	//static_assert(!Archives::traits::not_any_save_v<TestType::Type, ArchiveType>, "Nobody has a clue");

	//CFG.getStorage().writeContentsToStream(std::cout);
	//
	//std::cout << "Input Archive Test Begin" << std::endl;
	//InArchiveType InCFG{ CFG.getStorage() };
	//InCFG.getStorage().writeContentsToStream(std::cout);

	system("pause");
	return EXIT_SUCCESS;
}