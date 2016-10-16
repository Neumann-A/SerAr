#include "ConfigFile_Archive.h"

#include <sstream>
#include <istream>
#include <ostream>
#include <fstream>

using namespace Archives;
///-------------------------------------------------------------------------------------------------
///ConfigFile::Exceptions
///-------------------------------------------------------------------------------------------------

ConfigFile::Parse_error::Parse_error(const error_enum &err) : std::runtime_error(getErrorInfoString(err)), _err(err) { };

const char * ConfigFile::Parse_error::what() const noexcept
{
	static const auto str{ (std::string{ std::runtime_error::what() }+_msg) };
	static const auto ret{ str.c_str() };
	return ret;
}

void ConfigFile::Parse_error::append(std::string&& str)
{
	_msg.append(str);
}
std::string ConfigFile::Parse_error::getErrorInfoString(const error_enum &err) noexcept
{
	switch (err)
	{
	case error_enum::Unmatched_Brackets:
		return "Unmatched or invalid brackets {}! ";
	case error_enum::Missing_open_bracket:
		return "Missing opening bracket ({)! ";
	case error_enum::Missing_close_bracket:
		return "Missing closing bracket (})! ";
	case error_enum::Not_a_complex_number:
		return "String does no represent a complex number! ";
	case error_enum::Empty_string:
		return "Empty string to parse! ";
	case error_enum::Invalid_expression:
		return "Invalid expression in string! ";
	case error_enum::Invalid_characters:
		return "Invalid characters in string! ";
	case error_enum::Missing_comma_seperator:
		return "Missing comma seperator in string! ";
	case error_enum::Missing_string_identifier:
		return "Missing string identifier (')! ";
	case error_enum::Key_not_found:
		return "Requested key was not found! ";
	case error_enum::Section_not_found:
		return "Requested section was not found! ";
	case error_enum::First_line_is_not_section:
		return "First line in stream is not a valid section! ";
	default:
		return "Unknown parse error! ";
	}

}
///-------------------------------------------------------------------------------------------------
///ConfigFile::Special characters
///-------------------------------------------------------------------------------------------------


const std::string ConfigFile::SpecialCharacters::seperator{ ',' };
const std::string ConfigFile::SpecialCharacters::openbracket{ '{' };
const std::string ConfigFile::SpecialCharacters::closebracket{ '}' };
const std::string ConfigFile::SpecialCharacters::stringidentifier{ "'" };
const std::string ConfigFile::SpecialCharacters::escapestringidentifier{ "\'" };

//Section string
const std::string ConfigFile::SpecialCharacters::section_string{ "(?:^\\s*\\[)([a-zA-Z0-9_]+(\\.[a-zA-Z0-9_]+)*)(?:\\][:space:]*)$" };
const std::regex ConfigFile::SpecialCharacters::section_regex{ section_string };

// Keyvalue String has the form ( key    =   value wert      ). Whitespace before and after key or value will be automatically trimmed:
const std::string ConfigFile::SpecialCharacters::keyvalue_string{ "^(?:\\s*)([a-zA-Z0-9_]+)(?:\\s*)=(?:\\s*)(.*\\S)(?:\\s*)$" };
const std::regex ConfigFile::SpecialCharacters::keyvalue_regex{ keyvalue_string };

// Comments
const std::string ConfigFile::SpecialCharacters::comments_string{ "\\s*((//)|(%)|(;)|(#))" };
const std::regex ConfigFile::SpecialCharacters::comments_regex{ comments_string };

// Whitespaces only
const std::string ConfigFile::SpecialCharacters::whites_string{ "^\\s*$" };
const std::regex ConfigFile::SpecialCharacters::whites_regex{ comments_string };

// Pair String
const std::string ConfigFile::SpecialCharacters::pair_string{ "^\\{(\\S+)((?:\\s*),(?:\\s*)(\\S+))+\\}$" };
const std::regex ConfigFile::SpecialCharacters::pair_regex{ pair_string };

// String within Braces
const std::string ConfigFile::SpecialCharacters::brackets{ "(?:\\{)(?:\\s*).*(?:\\s*)(?:\\})" };
const std::regex ConfigFile::SpecialCharacters::brackets_regex{ brackets, std::regex::nosubs };

// Comma Seperator Regex
const std::regex ConfigFile::SpecialCharacters::seperator_regex{ seperator };

///-------------------------------------------------------------------------------------------------
///ConfigFile::FileParser
///-------------------------------------------------------------------------------------------------
void ConfigFile::FileParser::removeComment(std::string &line)
{
	std::smatch match;
	if (std::regex_search(line, match, SpecialCharacters::comments_regex))
	{
		line.erase(match.position());
	}
}

bool ConfigFile::FileParser::onlyWhitespace(const std::string &line)
{
	return (std::regex_search(line, SpecialCharacters::whites_regex) || line.empty());
}

bool ConfigFile::FileParser::validKeyValueLine(const std::string &line, std::smatch &match)
{
	return (std::regex_match(line, match, SpecialCharacters::keyvalue_regex));
}

bool ConfigFile::FileParser::validKeyValueLine(const std::string &line)
{
	return (std::regex_match(line, SpecialCharacters::keyvalue_regex));
}

bool ConfigFile::FileParser::validSectionLine(const std::string &line, std::smatch &match)
{
	return (std::regex_match(line, match, SpecialCharacters::section_regex));
}

bool ConfigFile::FileParser::validSectionLine(const std::string &line)
{
	return (std::regex_match(line, SpecialCharacters::section_regex));
}

void ConfigFile::FileParser::extractKeyValue(const std::smatch &match, std::string &key, std::string &value)
{
	key = match.str(1);
	value = match.str(2);
}

std::string ConfigFile::FileParser::extractSection(const std::smatch &match)
{
	return match.str(1);
}

void ConfigFile::FileParser::parseLine(const std::string &line, size_t const lineNo, std::string &currentSection, ConfigFile::Storage &storage)
{
	// Comments and Whitespace lines are alread ignored / removed
	// So we should only have either a valid section line or a valid keyvalue line
	std::smatch match;

	if (validKeyValueLine(line, match))
	{
		std::string key, value;
		extractKeyValue(match, key, value);
		(storage.accessContents())[currentSection][key] = value;
		return;
	}

	if (validSectionLine(line, match))
	{
		currentSection = extractSection(match);
		return;
	}

	throw std::runtime_error{ "Configuration Archive: Line Number: " + std::to_string(lineNo) + " is not valid. Line is: " + line + "\n" };
};

void ConfigFile::FileParser::loadIntoStorage(std::istream& stream, ConfigFile::Storage &storage)
{
	//std::ifstream file;
	//file.open(fName, std::ios::in);
	//if (!file)
	//{
	//	exitWithError("CFG: File " + _fName.string() + " couldn't be found!\n");
	//	return;
	//}

	std::string line;
	std::string currentSection;

	size_t lineNo = 0;
	bool FirstRun = true;

	while (std::getline(stream, line))
	{
		++lineNo;

		std::string temp{ line };

		if (temp.empty())
			continue;

		removeComment(temp);
		if (onlyWhitespace(temp))
			continue;

		if (FirstRun)
		{
			std::smatch match;
			if (validSectionLine(temp, match))
				currentSection = extractSection(match);
			else
			{
				std::runtime_error{ "Configuration archive parse error: First non comment\\whitespace line is not a section header!" };
				break;
			}
			FirstRun = false;
		}
		else
		{
			parseLine(temp, lineNo, currentSection, storage);
		}
	}
}

///-------------------------------------------------------------------------------------------------
///ConfigFile::Storage
///-------------------------------------------------------------------------------------------------

void  ConfigFile::Storage::writeContentsToStream(std::ostream &stream) const
{
	if (_contents.empty())
		stream << "Nothing to write to stream" << '\n';

	for (const auto& sections : _contents)
	{
		stream << '[' << sections.first << ']' << '\n';
		for (const auto& keys : sections.second)
		{
			stream << keys.first << " = " << keys.second << '\n';
		}
		stream << /*"[EOS]" << '\n' <<*/ '\n';
	}
	stream << /*"[END]" << */'\n' << '\n';
};


///-------------------------------------------------------------------------------------------------
///ConfigFile::toString
///-------------------------------------------------------------------------------------------------

void ConfigFile::toString::checkSyntax(const std::string& section, const std::string& key, const std::string& value)
{
	const std::string s{ ' ' + key + " = " + value };
	if (!FileParser::validKeyValueLine(s))
	{
		throw std::runtime_error{ std::string{ "Key and\\or Value does not fullfill requirements for Configuration Archive. Key: " + key + " Value:" + value } };
	}
	if (!FileParser::validSectionLine('[' + section + ']'))
	{
		throw std::runtime_error{ std::string{ "Section does not fullfill requirements for Configuration Archive. Section: " + section } };
	}
	return;
}

///-------------------------------------------------------------------------------------------------
///ConfigFile::Output Archive
///-------------------------------------------------------------------------------------------------

ConfigFile_OutputArchive::ConfigFile_OutputArchive(std::ostream& stream) : OutputArchive(this), mOutputstream(stream) {};
ConfigFile_OutputArchive::ConfigFile_OutputArchive(const std::experimental::filesystem::path &path) : OutputArchive(this), mOutputstream(createFileStream(path)) {};
ConfigFile_OutputArchive::~ConfigFile_OutputArchive()
{
	mStorage.writeContentsToStream(mOutputstream);
	mOutputstream << std::flush;

	if (mStreamOwner) 
	{
		dynamic_cast<std::fstream*>(&mOutputstream)->close();
		delete &mOutputstream; // We created the Stream object we also have to delete it!
	}
}

std::ofstream& ConfigFile_OutputArchive::createFileStream(const std::experimental::filesystem::path &path)
{
	if (!path.has_filename())
	{
		throw std::runtime_error{ "Cannot open configuration file! Filename is missing!" };
	}

	std::ofstream& stream = *(new std::ofstream());
	stream.open(path.string().c_str(), std::fstream::trunc);
	if (!stream.is_open())
	{
		delete &stream; //Delete created Stream before we throw; If we throw destructor of the class will not be called!
		throw std::runtime_error{ "Could not open File! Already in use?" };
	}

	mStreamOwner = true;

	assert(&stream != nullptr);

	return stream;
}

///-------------------------------------------------------------------------------------------------
///ConfigFile::Input Archive
///-------------------------------------------------------------------------------------------------
ConfigFile_InputArchive::ConfigFile_InputArchive(std::istream& stream) : InputArchive(this), mInputstream(stream)
{
	parseStream();
};

ConfigFile_InputArchive::ConfigFile_InputArchive(ConfigFile::Storage storage) : InputArchive(this), mInputstream(std::cin), mStorage(std::move(storage))
{
};

ConfigFile_InputArchive::ConfigFile_InputArchive(const std::experimental::filesystem::path &path) : InputArchive(this), mInputstream(createFileStream(path))
{
	parseStream();
};

ConfigFile_InputArchive::~ConfigFile_InputArchive() 
{
	if (mStreamOwner)
	{
		dynamic_cast<std::fstream*>(&mInputstream)->close();
		delete &mInputstream; // We created the Stream object we also have to delete it!
	}
};

std::ifstream& ConfigFile_InputArchive::createFileStream(const std::experimental::filesystem::path &path)
{
	if (!path.has_filename())
	{
		throw std::runtime_error{ "Cannot open configuration file! Filename is missing!" };
	}

	std::ifstream& stream = *(new std::ifstream());
	stream.open(path.string().c_str());
	if (!stream.is_open())
	{
		delete &stream; //Delete created Stream before we throw; If we throw destructor of the class will not be called!
		throw std::runtime_error{ "Could not open File! Already in use?" };
	}

	mStreamOwner = true;

	assert(&stream != nullptr);

	return stream;
}

void ConfigFile_InputArchive::parseStream()
{
	if (!mInputstream)
	{
		throw std::runtime_error{ "Cannot read from Stream. Stream not ready! " };
	}
	std::string line;
	std::string currentSection;

	size_t lineNo = 0;
	bool FirstRun{ true };

	while (std::getline(mInputstream, line))
	{
		++lineNo;

		auto& temp{ line };

		if (temp.empty()) 
			continue; //Empty Line

		ConfigFile::FileParser::removeComment(temp);
		if (ConfigFile::FileParser::onlyWhitespace(temp) || temp.empty()) 
			continue; //Only Whitespaces or Comments

		if (FirstRun)
		{
			std::smatch match;
			if (ConfigFile::FileParser::validSectionLine(temp, match))
			{
				currentSection = ConfigFile::FileParser::extractSection(match);
			}
			else
			{
				throw ConfigFile::Parse_error{ ConfigFile::Parse_error::error_enum::First_line_is_not_section };
			}
			FirstRun = false;
		}
		else
		{
			ConfigFile::FileParser::parseLine(temp, lineNo, currentsection, mStorage);
		}
	}
};