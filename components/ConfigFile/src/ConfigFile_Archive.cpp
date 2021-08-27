#include "ConfigFile_Archive.h"

#include <sstream>
#include <istream>
#include <ostream>
#include <fstream>


using namespace Archives;
///-------------------------------------------------------------------------------------------------
///ConfigFile::Exceptions
///-------------------------------------------------------------------------------------------------

ConfigFile::Parse_error::Parse_error(const error_enum& err) : std::runtime_error(getErrorInfoString(err)), _err(err) { }
ConfigFile::Parse_error::Parse_error(const error_enum& err, std::string &&str) : std::runtime_error(str), _err(err) { }

const char * ConfigFile::Parse_error::what() const noexcept
{
    return std::runtime_error::what();
}

void ConfigFile::Parse_error::append(std::string&& str)
{
    std::string msg { std::runtime_error::what() } ;
    msg.append(str);
    *this = Parse_error(std::move(_err),std::move(msg));
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
        // The If Means match.position() > 0
        if (match.position() > 0) // Thats a double check since the if above already makes sure there is a match
            line.erase(static_cast<std::size_t>(match.position()));
        //line.erase(static_cast<std::size_t>(match.position()));
    }
}

std::string ConfigFile::FileParser::trimWhitespaces(const std::string &str, const std::string& whitespace)
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
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
    return std::string{ match.str(1) };
}

void ConfigFile::FileParser::parseLine(const std::string &line, const size_t &lineNo,std::string &currentSection, ConfigFile::Storage &storage)
{
    // Comments and Whitespace lines are alread ignored / removed
    // So we should only have either a valid section line or a valid keyvalue line or an empty line
    if (line.empty())
        return;

    std::smatch match;

    if (validSectionLine(line, match))
    {
        currentSection = extractSection(match);
    }
    else if (validKeyValueLine(line, match))
    {
        const std::string sec{ currentSection };
        std::string key, value;
        extractKeyValue(match, key, value);
        (storage.accessContents()[sec])[key]= value;
    }
    else
    {
        const std::string& sec{ currentSection };
        std::stringstream str;
        str << "Configuration Archive: Line Number: ";
        str << std::to_string(lineNo);
        str << " is not valid. Section:" << sec << "; Line is: " << line << std::endl;
        std::cout << str.str();
        throw std::runtime_error{ str.str() };
        //return;
    }
}

//void ConfigFile::FileParser::loadIntoStorage(std::istream& stream, ConfigFile::Storage &storage)
//{
//	//std::ifstream file;
//	//file.open(fName, std::ios::in);
//	//if (!file)
//	//{
//	//	exitWithError("CFG: File " + _fName.string() + " couldn't be found!\n");
//	//	return;
//	//}
//
//	std::string line;
//	std::string currentSection;
//
//	size_t lineNo = 0;
//	bool FirstRun = true;
//
//	while (std::getline(stream, line))
//	{
//		++lineNo;
//
//		std::string temp{ line };
//
//		if (temp.empty())
//			continue;
//
//		removeComment(temp);
//		temp = trimWhitespaces(temp);
//
//		if (temp.empty())
//			continue;
//
//		if (FirstRun)
//		{
//			std::smatch match;
//			if (validSectionLine(temp, match))
//				currentSection = extractSection(match);
//			else
//			{
//				std::runtime_error{ "Configuration archive parse error: First non comment\\whitespace line is not a section header!" };
//				break;
//			}
//			FirstRun = false;
//		}
//		else
//		{
//			std::cout << temp << std::endl;
//			parseLine(temp, lineNo, currentSection, storage);
//		}
//	}
//}

///-------------------------------------------------------------------------------------------------
///ConfigFile::Storage
///-------------------------------------------------------------------------------------------------

void  ConfigFile::Storage::writeContentsToStream(std::ostream &stream) const
{
    if (_contents.empty())
        stream << "Nothing to write to stream" << '\n';

    for (const auto& sec : _contents)
    {
        stream << '[' << sec.first << ']' << '\n';
        for (const auto& keys : sec.second)
        {
            stream << keys.first << " = " << keys.second << '\n';
        }
        stream << /*"[EOS]" << '\n' <<*/ '\n';
    }
    stream << /*"[END]" << */'\n' << '\n';
}


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
///ConfigFile::fromString
///-------------------------------------------------------------------------------------------------

///-------------------------------------------------------------------------------------------------
///ConfigFile::Output Archive
///-------------------------------------------------------------------------------------------------

ConfigFile_OutputArchive::ConfigFile_OutputArchive(std::ostream& stream) : OutputArchive(this), mOutputstream(stream), mStorage() {}
ConfigFile_OutputArchive::ConfigFile_OutputArchive(const std::filesystem::path &path, const ConfigFile_Options& ) : OutputArchive(this), mOutputstream(createFileStream(path)), mStorage() {}
ConfigFile_OutputArchive::~ConfigFile_OutputArchive()
{
    mStorage.writeContentsToStream(mOutputstream);
    mOutputstream << std::flush;

    if (mStreamOwner) 
    {
        delete &mOutputstream; // We created the Stream object we also have to delete it!
    }
}

std::ofstream& ConfigFile_OutputArchive::createFileStream(const std::filesystem::path &path)
{
    if (!path.has_filename())
    {
        throw std::runtime_error{ "Cannot open configuration file! Filename is missing!" };
    }

    // TODO: rewrite to something which makes sense
    //std::ofstream& stream = *(new std::ofstream());
    std::unique_ptr<std::ofstream> pstr = std::make_unique<std::ofstream>();
    
    assert(pstr != nullptr);

    pstr->open(path.string().c_str(), std::fstream::trunc);
    if (!pstr->is_open())
    {
        throw std::runtime_error{ "Could not open File! Already in use?" };
    }

    mStreamOwner = true;

    return *(pstr.release());
}

///-------------------------------------------------------------------------------------------------
///ConfigFile::Input Archive
///-------------------------------------------------------------------------------------------------
ConfigFile_InputArchive::ConfigFile_InputArchive(std::istream &stream) : InputArchive(this), mInputstream(stream), mStorage()
{
    parseStream();
}

ConfigFile_InputArchive::ConfigFile_InputArchive(ConfigFile::Storage storage) : InputArchive(this), mInputstream(std::cin), mStorage(std::move(storage))
{
}

ConfigFile_InputArchive::ConfigFile_InputArchive(const std::filesystem::path &path) : InputArchive(this), mInputstream(createFileStream(path)), mStorage()
{
    parseStream();
}

ConfigFile_InputArchive::ConfigFile_InputArchive(ConfigFile_InputArchive&& CFG) : InputArchive(this), mInputstream(CFG.mInputstream) 
{
    std::swap(this->mStreamOwner, CFG.mStreamOwner);
    std::swap(this->mStorage, CFG.mStorage);
    CFG.mStreamOwner = false;
}

ConfigFile_InputArchive ConfigFile_InputArchive::operator=(ConfigFile_InputArchive&& CFG) 
{
    //delegate to move constructor
    return ConfigFile_InputArchive{ std::move(CFG) };
}

ConfigFile_InputArchive::~ConfigFile_InputArchive()
{
    if (mStreamOwner)
    {
        delete (&mInputstream); // We created the Stream object we also have to delete it!
    }
}

void ConfigFile_InputArchive::SkipBOM(std::ifstream &in)
{
    char test[3] = { 0 };
    in.read(test, 3);
    if ((unsigned char)test[0] == 0xEF &&
        (unsigned char)test[1] == 0xBB &&
        (unsigned char)test[2] == 0xBF)
    //if (static_cast<unsigned char>(test[0]) == 0xEF &&
    //	static_cast<unsigned char>(test[1]) == 0xBB &&
    //	static_cast<unsigned char>(test[2]) == 0xBF)
    {
        return;
    }
    in.seekg(0);
}

std::ifstream& ConfigFile_InputArchive::createFileStream(const std::filesystem::path &path)
{
    if (!path.has_filename())
    {
        throw std::runtime_error{ "Cannot open configuration file! Filename is missing!" };
    }

    std::unique_ptr<std::ifstream> pstr = std::make_unique<std::ifstream>();

    assert(pstr != nullptr);

    pstr->open(path.string().c_str());
    if (!pstr->is_open())
    {
        std::string err{ "Unable to open: " };
        err += path.string();
        err += "(File already opened?)";
        throw std::runtime_error{ err.c_str() };
    }

    SkipBOM(*pstr);

    mStreamOwner = true;

    return *(pstr.release());
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
        if (temp.empty())
            continue; //Empty Line
        //temp.shrink_to_fit();
        temp = ConfigFile::FileParser::trimWhitespaces(temp);
        if (temp.empty())
            continue; //Empty Line
    
        if (ConfigFile::FileParser::onlyWhitespace(temp) || temp.empty()) 
            continue; //Only Whitespaces or Comments

        temp.shrink_to_fit();

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
            ConfigFile::FileParser::parseLine(temp, lineNo, currentSection, mStorage);
        }
    }
}

auto ConfigFile_InputArchive::list(std::string value) -> typename ConfigFile::Storage::keyvalues
{
    using type = decltype(mStorage.accessContents().cbegin()->second);
    type data;
    ConfigLogic.setCurrKey(std::move(value));
    ConfigLogic.setCurrKey("Dummy");

    if (mStorage.accessContents().find(ConfigLogic.getSection()) != mStorage.accessContents().end())
    {
        data = mStorage.accessContents().at(ConfigLogic.getSection());
    }
    else
    {
        data = type{};
    }

    ConfigLogic.resetCurrKey();
    ConfigLogic.resetCurrKey();
    return data;
}

auto ConfigFile_InputArchive::list(const Archives::NamedValue<decltype(nullptr)>& value) -> typename ConfigFile::Storage::keyvalues
{
    return list(value.getName());
}

auto ConfigFile_InputArchive::list() -> typename ConfigFile::Storage::sections
{
    const auto& tmp = mStorage.accessContents();
    return tmp;
}

template void ConfigFile_InputArchive::load<bool>(Archives::NamedValue<bool>& value);
template void ConfigFile_InputArchive::load<short>(Archives::NamedValue<short>& value);
template void ConfigFile_InputArchive::load<int>(Archives::NamedValue<int>& value);
template void ConfigFile_InputArchive::load<long>(Archives::NamedValue<long>& value);
template void ConfigFile_InputArchive::load<long long>(Archives::NamedValue<long long>& value);
template void ConfigFile_InputArchive::load<unsigned int>(Archives::NamedValue<unsigned int>& value);
template void ConfigFile_InputArchive::load<unsigned long>(Archives::NamedValue<unsigned long>& value);
template void ConfigFile_InputArchive::load<unsigned long long>(Archives::NamedValue<unsigned long long>& value);
template void ConfigFile_InputArchive::load<float>(Archives::NamedValue<float>& value);
template void ConfigFile_InputArchive::load<double>(Archives::NamedValue<double>& value);
template void ConfigFile_InputArchive::load<long double>(Archives::NamedValue<long double>& value);
template void ConfigFile_InputArchive::load<std::string>(Archives::NamedValue<std::string>& value);
template void ConfigFile_InputArchive::load<bool&>(Archives::NamedValue<bool&>& value);
template void ConfigFile_InputArchive::load<short&>(Archives::NamedValue<short&>& value);
template void ConfigFile_InputArchive::load<int&>(Archives::NamedValue<int&>& value);
template void ConfigFile_InputArchive::load<long&>(Archives::NamedValue<long&>& value);
template void ConfigFile_InputArchive::load<long long&>(Archives::NamedValue<long long&>& value);
template void ConfigFile_InputArchive::load<unsigned int&>(Archives::NamedValue<unsigned int&>& value);
template void ConfigFile_InputArchive::load<unsigned long&>(Archives::NamedValue<unsigned long&>& value);
template void ConfigFile_InputArchive::load<unsigned long long&>(Archives::NamedValue<unsigned long long&>& value);
template void ConfigFile_InputArchive::load<float&>(Archives::NamedValue<float&>& value);
template void ConfigFile_InputArchive::load<double&>(Archives::NamedValue<double&>& value);
template void ConfigFile_InputArchive::load<long double&>(Archives::NamedValue<long double&>& value);
template void ConfigFile_InputArchive::load<std::string&>(Archives::NamedValue<std::string&>& value);
