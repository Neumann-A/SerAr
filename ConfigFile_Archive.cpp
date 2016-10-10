#include "ConfigFile_Archive.h"

using namespace Archives;
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


void ConfigFile::toString::checkSyntax(std::string section, std::string key, std::string value)
{
	std::string s{ ' ' + key + " = " + value };
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