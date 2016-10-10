#pragma once

#include <filesystem>
#include <type_traits>
#include <map>

#include <sstream>
#include <istream>
#include <ostream>

#include <string>
#include <regex>

#include <complex>

#include <exception>

#include <experimental\filesystem>

#include <Eigen\Core>

#include "std_extensions.h"

#include "BasicMacros.h"
#include "BasicIncludes.h"

#include "NamedValue.h"
#include "ArchiveHelper.h"
#include "InputArchive.h"
#include "OutputArchive.h"


namespace Archives
{
	namespace traits
	{
		/******************************to_string helpers***********************************************************************/
		//Member Function type for converting values to strings
		template<class T, typename ...Args>
		using member_to_string_t = decltype(std::declval<T&>().to_string(std::declval<Args&>()...));
		//Function type for converting values to strings
		template<typename ...Args>
		using func_to_string_t = decltype(to_string(std::declval<Args&>()...));
		//Std function type for converting values to strings
		template<typename ...Args>
		using std_to_string_t = decltype(std::to_string(std::declval<Args&>()...));

		//Checks if ToTest has a save function for itself
		template<typename TypeToTest, typename TypeToConvert>
		class has_archive_member_to_string : public stdext::is_detected<member_to_string_t, TypeToTest, TypeToConvert> {};
		template<typename TypeToTest, typename TypeToConvert>
		constexpr bool has_archive_member_to_string_v = has_archive_member_to_string<TypeToTest, TypeToConvert>::value;

		//Checks if ToTest has a save function for itself
		template<typename TypeToConvert>
		class has_member_to_string : public stdext::is_detected<member_to_string_t, TypeToConvert> {};
		template<typename TypeToConvert>
		constexpr bool has_member_to_string_v = has_member_to_string<TypeToConvert>::value;

		//Checks if there is a save function for ToTest
		template<typename TypeToConvert>
		class has_func_to_string : public stdext::is_detected<func_to_string_t, TypeToConvert> {};
		template<typename TypeToConvert>
		constexpr bool has_func_to_string_v = has_func_to_string<TypeToConvert>::value;

		//Checks if the Archive has a save function for ToTest
		template<typename TypeToConvert>
		class has_std_to_string : public stdext::is_detected<std_to_string_t, TypeToConvert>{};
		template<typename TypeToConvert>
		constexpr bool has_std_to_string_v = has_std_to_string<TypeToConvert>::value;

		//There exists a function which can convert the type to a string
		template<typename TypeToConvert>
		class has_type_to_string :public std::disjunction<has_member_to_string<TypeToConvert>, has_func_to_string<TypeToConvert>, has_std_to_string<TypeToConvert>> {};
		template<typename TypeToConvert>
		constexpr bool has_type_to_string_v = has_type_to_string<TypeToConvert>::value;

		//There exists no function to convert the type to string
		template<typename TypeToTest, typename TypeToConvert>
		class has_no_to_string : public std::negation<std::disjunction<has_archive_member_to_string<TypeToTest, TypeToConvert>, has_type_to_string<TypeToConvert>>> {};
		template<typename TypeToTest, typename TypeToConvert>
		constexpr bool has_no_to_string_v = has_no_to_string<TypeToTest, TypeToConvert>::value;

		template<typename TypeToConvert, typename HelperClass, typename TextArchive>
		class use_to_string :public std::disjunction< has_archive_member_to_string<HelperClass, TypeToConvert>, std::conjunction<has_type_to_string<TypeToConvert>, no_type_save<TypeToConvert, TextArchive>>> {};
		template<typename TypeToConvert, typename HelperClass, typename TextArchive>
		constexpr bool use_to_string_v = use_to_string<TypeToConvert, HelperClass, TextArchive>::value;

		//Logic to determine which to_string function should be used
		template<typename TypeToConvert,typename HelperClass>
		class use_archive_or_func_to_string : public std::disjunction<has_archive_member_to_string<HelperClass, TypeToConvert>, has_func_to_string<TypeToConvert> > {};
		template<typename TypeToConvert, typename HelperClass>
		constexpr bool use_archive_or_func_to_string_v = use_archive_or_func_to_string<TypeToConvert, HelperClass>::value;

		template<typename TypeToConvert, typename HelperClass>
		class use_type_member_to_string :public std::conjunction<std::negation<has_archive_member_to_string<HelperClass, TypeToConvert>>, has_member_to_string<TypeToConvert>> {};
		template<typename TypeToConvert, typename HelperClass>
		constexpr bool use_type_member_to_string_v = use_type_member_to_string<TypeToConvert, HelperClass>::value;

		template<typename TypeToConvert, typename HelperClass>
		class use_std_to_string :public std::conjunction<has_std_to_string<TypeToConvert>, std::negation<use_type_member_to_string<TypeToConvert, HelperClass>>, std::negation<use_archive_or_func_to_string<TypeToConvert, HelperClass>>> {};
		template<typename TypeToConvert, typename HelperClass>
		constexpr bool use_std_to_string_v = use_std_to_string<TypeToConvert, HelperClass>::value;

		/**************************************************from_string helpers************************************************************************/
		//Member Function type for converting values to strings
		template<class T, typename ...Args>
		using member_from_string_t = decltype(std::declval<T&>().from_string(std::declval<std::string&>(),std::declval<Args&>()...));
		//Function type for converting values to strings
		template<typename ...Args>
		using func_from_string_t = decltype(from_string(std::declval<std::string&>(),std::declval<Args&>()...));
		//Std function type for converting values to strings

		//Checks if ToTest has a load function for itself
		template<typename TypeToTest, typename TypeToConvert>
		class has_archive_member_from_string : public stdext::is_detected_exact<std::decay_t<TypeToConvert>,member_from_string_t, TypeToTest> {};
		template<typename TypeToTest, typename TypeToConvert>
		constexpr bool has_archive_member_from_string_v = has_archive_member_from_string<TypeToTest, TypeToConvert>::value;

		//Checks if ToTest has a load function for itself
		template<typename TypeToConvert>
		class has_member_from_string : public stdext::is_detected_exact<std::decay_t<TypeToConvert>,member_from_string_t> {};
		template<typename TypeToConvert>
		constexpr bool has_member_from_string_v = has_member_from_string<TypeToConvert>::value;

		//Checks if there is a load function for ToTest
		template<typename TypeToConvert>
		class has_func_from_string : public stdext::is_detected_exact<std::decay_t<TypeToConvert>, func_from_string_t> {};
		template<typename TypeToConvert>
		constexpr bool has_func_from_string_v = has_func_from_string<TypeToConvert>::value;

		//There exists a function which can convert the type to a string
		template<typename TypeToConvert>
		class has_type_from_string :public std::disjunction<has_member_from_string<TypeToConvert>, has_func_from_string<TypeToConvert>> {};
		template<typename TypeToConvert>
		constexpr bool has_type_from_string_v = has_type_from_string<TypeToConvert>::value;

		//There exists no function to convert the type to string
		template<typename TypeToTest, typename TypeToConvert>
		class has_no_from_string : public std::negation<std::disjunction<has_archive_member_from_string<TypeToTest, TypeToConvert>, has_type_from_string<TypeToConvert>>> {};
		template<typename TypeToTest, typename TypeToConvert>
		constexpr bool has_no_from_string_v = has_no_from_string<TypeToTest, TypeToConvert>::value;

		template<typename TypeToConvert, typename HelperClass, typename TextArchive>
		class use_from_string :public std::disjunction< has_archive_member_from_string<HelperClass, TypeToConvert>, std::conjunction<has_type_from_string<TypeToConvert>, no_type_load<TypeToConvert, TextArchive>>> {};
		template<typename TypeToConvert, typename HelperClass, typename TextArchive>
		constexpr bool use_from_string_v = use_from_string<TypeToConvert, HelperClass, TextArchive>::value;

		//Logic to determine which from_string function should be used
		template<typename TypeToConvert, typename HelperClass>
		class use_archive_or_func_from_string : public std::disjunction<has_archive_member_from_string<HelperClass, TypeToConvert>, has_func_from_string<TypeToConvert> > {};
		template<typename TypeToConvert, typename HelperClass>
		constexpr bool use_archive_or_func_from_string_v = use_archive_or_func_from_string<TypeToConvert, HelperClass>::value;

		template<typename TypeToConvert, typename HelperClass>
		class use_type_member_from_string :public std::conjunction<std::negation<has_archive_member_from_string<HelperClass, TypeToConvert>>, has_member_from_string<TypeToConvert>> {};
		template<typename TypeToConvert, typename HelperClass>
		constexpr bool use_type_member_from_string_v = use_type_member_from_string<TypeToConvert, HelperClass>::value;

	};

	class ConfigFile
	{
	public:
		///-------------------------------------------------------------------------------------------------
		/// <summary>	Class for Configuration Parse Error </summary>
		///
		/// <seealso cref="T:std::runtime_error"/>
		///-------------------------------------------------------------------------------------------------
		class Parse_error : public std::runtime_error
		{
		public:
			enum class error_enum {
				Unmatched_Brackets, Missing_open_bracket, Missing_close_bracket,
				Not_a_complex_number, Empty_string, Invalid_expression, Invalid_characters,
				Missing_comma_seperator, Missing_string_identifier,
				Key_not_found, Section_not_found
			};

			Parse_error(const error_enum &err) : std::runtime_error(getErrorInfoString(err)), _err(err) { };

			const char * what() const noexcept override
			{
				return (std::string{ std::runtime_error::what() }+_msg).c_str();
			}

			void append(std::string&& str)
			{
				_msg.append(str);
			}

		private:
			error_enum _err;
			std::string _msg{};

			std::string getErrorInfoString(const error_enum &err)
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
				default:
					return "Unknown parse error! ";
				}

			}
		};

		/// <summary>	Class which has all Special Characters used by the logic for Configuration Files  </summary>
		class SpecialCharacters
		{
		public:
			static const std::string seperator;
			static const std::string openbracket;
			static const std::string closebracket;
			static const std::string stringidentifier;
			static const std::string escapestringidentifier;

			// Section String has the form: [parta.partb.partx] with an arbitary number of whitespace before or after. Section string does not contain numbers!
			static const std::string section_string;// { "(?:^\\s*\\[)([a-zA-Z0-9_]+(\\.[a-zA-Z0-9_]+)*)(?:\\][:space:]*)$" };
			static const std::regex section_regex; //= std::regex{section_string};// { section_string };

			// Keyvalue String has the form ( key    =   value wert      ). Whitespace before and after key or value will be automatically trimmed:
			static const std::string keyvalue_string;// { "^(?:\\s*)([a-zA-Z0-9_]+)(?:\\s*)=(?:\\s*)(.*\\S)(?:\\s*)$" };
			static const std::regex keyvalue_regex;// { keyvalue_string };

											 // Comments
			static const std::string comments_string;// { "\\s*((//)|(%)|(;)|(#))" };
			static const std::regex comments_regex;// { comments_string };

											 // Whitespaces only
			static const std::string whites_string;// { "^\\s*$" };
			static const std::regex whites_regex;// { comments_string };

										   // Pair String
			static const std::string pair_string;// {"^\\{(\\S+)((?:\\s*),(?:\\s*)(\\S+))+\\}$"};
			static const std::regex pair_regex;// { pair_string };

										 // String within Braces
			static const std::string brackets;// { "(?:\\{)(?:\\s*).*(?:\\s*)(?:\\})" };
			static const std::regex brackets_regex;// { braces, std::regex::nosubs };
			
			static const std::regex seperator_regex;// { comma };

		};

		/// <summary>	Class which has all the from_string Logic for Configuration Files  </summary>
		class fromString
		{

		public:
			///-------------------------------------------------------------------------------------------------
			/// <summary>	Selects the correct from_string implementation for the given type of value. </summary>
			///
			/// <typeparam name="T">	Generic type parameter. </typeparam>
			/// <param name="val">	[in,out] The value. </param>
			///
			/// <returns>	value as a string </returns>
			///-------------------------------------------------------------------------------------------------
			template<typename T>
			static inline std::enable_if_t<traits::use_archive_or_func_from_string_v<std::decay_t<T>, fromString>, std::decay_t<T>> from_string_selector(std::string& str)
			{
				return from_string<std::decay_t<T>>(str);
			}

			template<typename T>
			static inline std::enable_if_t<traits::use_type_member_from_string_v<std::decay_t<T>, fromString>, std::decay_t<T>> from_string_selector(std::string& str)
			{
				return (std::decay_t<T>)::from_string<std::decay_t<T>>(str);
			}

			template<typename T>
			static inline std::enable_if_t<std::is_same<std::decay_t<T>, std::string>::value, std::decay_t<T>> from_string_selector(std::string& str)
			{
				std::string tmp{ std::move(str) };
				auto pos = tmp.find_first_of(SpecialCharacters::escapestringidentifier);
				while (pos != tmp.npos)
				{
					tmp.replace(pos, SpecialCharacters::escapestringidentifier.size(), stringidentifier); //Escaping or special characters
				}
				return tmp;
			}

			/***End from_string Selector***/

			/// <summary>	Convert string into number representation. </summary>
			template <typename T>
			static inline std::enable_if_t<std::is_arithmetic<std::decay_t<T>>::value, std::decay_t<T>> from_string(std::string &str)
			{
				std::size_t pos;
				auto num = BasicTools::stringToNumber<std::decay_t<T>>(str, pos);
				str.erase(0, pos);

				afterConversionStringCheck(str);
			};

			/// <summary>	Convert a string into a complex number. </summary>
			template <typename T>
			static inline std::enable_if_t<std::is_same<T, std::complex<typename T::value_type>>::value, T> from_string(std::string &str)
			{
				using type = typename T::value_type;

				if (str.empty())
					throw Parse_error{ Parse_error::error_enum::Empty_string };

				auto ipos = str.find_first_of("ijIJ");

				if (ipos != str.npos)
				{ // Check if there is more than one iiIJ -> Invalid Expression
					auto ipos2 = str.find_last_of("ijIJ");

					if (ipos2 != ipos)
						throw Parse_error{ Parse_error::error_enum::Invalid_expression };
				}
				else // String has to be a real number or is invalid; 
				{
					type real = from_string<type>(str);
					return std::complex<type>{real, static_cast<type>(0.0)};
				}

				// we have only one "i" and have to find the correct position to seperate the maybe two numbers
				// Case1: 2.323(E+03)i, i2.323(E+03) ,  2.323(E+03)i + x, i2.323(E+03) + x , x + 2.323(E+03)i, x + i2.323(E+03)

				type tmp1{ 0 };
				std::size_t currentparse = 0;
				bool firstexception = false;

				try
				{
					tmp1 = BasicTools::stringToNumber<type>(str, currentparse);
				}
				catch (std::invalid_argument&)
				{
					if (str.find_first_of("0123456789.E+-iIjJ", ipos) != ipos) // Number before I which was not parsed!
						throw Parse_error{ Parse_error::error_enum::Invalid_expression };

					firstexception = true;
				}

				type real{ 0 };
				type complex{ 0 };

				if (!firstexception)
				{
					bool complexfirst = false;
					str.erase(0, currentparse); //Move pos one further
					ipos = str.find_first_of("ijIJ"); // find knew position of the i

					if (ipos == 0) // found the complex number
					{
						complex = tmp1;
						str.erase(0, 1);//delete the i
						complexfirst = true;
					}
					else
					{
						real = tmp1;
					}

					str.erase(ipos, 1);
					if (str.compare(0, 1, "+") == 0) //remove trailing + for conversion
						str.erase(0, 1);

					try
					{
						if (complexfirst && str.length() > 0)
						{
							real = BasicTools::stringToNumber<type>(str, currentparse);
							str.erase(0, currentparse);
						}
						else if (!complexfirst && str.length() > 0)
						{
							if (str.length() == 1)
							{
								if (str.compare(0, 1, "-") == 0)
								{
									complex = -1.0;
									str.erase(0, 1);
								}
							}
							else
							{
								complex = BasicTools::stringToNumber<type>(str, currentparse);
								str.erase(0, currentparse);
							}
						}
						else if (str.length() == 0)
						{
							complex = 1.0;
							str.clear();
						}

					}
					catch (std::invalid_argument&)
					{
						throw Parse_error{ Parse_error::error_enum::Invalid_expression };
					}

					//if (!str.empty())
					//{
					//	throw Parse_error{ Parse_error::error_enum::Invalid_expression };
					//}
				}
				else // tmp1 = 0; firstexception thrown
				{
					std::size_t del = (ipos < 1) ? 0 : (ipos - 1);
					str.erase(0, del); // we know that the first one will be a complex number so we can delete it to the i
					auto signpos = str.find_first_of("+-");

					bool complexfound{ false };

					if (signpos == 0 || (ipos == 0 && signpos == 1)) //Single standing i+-
					{
						if (str.compare(0, 1, "-") == 0)
						{
							complex = -1.0;  // complex number will be at least 1;
							str.erase(0, 2); //remove -i
						}
						else
						{
							complex = 1.0;
							str.erase(0, 1); //remove i
						}

						complexfound = true;
					}
					else // extract complex number
					{
						try
						{
							if (complexfound)
							{
								if (str.compare(0, 1, "+") == 0)
									str.erase(0, 1); //remove unneccessary +

								real = BasicTools::stringToNumber<type>(str, currentparse);
							}
							else
							{
								complex = BasicTools::stringToNumber<type>(str, currentparse);
							}

							str.erase(0, currentparse);

							auto val = str.compare(0, 1, "+");
							if (str.compare(0, 1, "+") == 0)
							{
								str.erase(0, 1);
							}
						}
						catch (std::invalid_argument&)
						{
							throw Parse_error{ Parse_error::error_enum::Invalid_expression };
						}
					}

					if (str.empty()) // nothing left -> real = 0
					{
						real = 0;
						return T{ real, complex };
					}
					else
					{
						try
						{
							real = BasicTools::stringToNumber<type>(str, currentparse);
						}
						catch (std::invalid_argument&)
						{
							throw Parse_error{ Parse_error::error_enum::Invalid_expression };
						}
						str.erase(0, currentparse);
						//if (str.empty())
						//	return T{ real, complex };
						//else
						//	throw Parse_error{ Parse_error::error_enum::Invalid_expression };
					}
				}
				return T{ real, complex };
			};

			/// <summary>	Convert containers into a braced string representation. </summary>
			template<typename T>
			static inline std::enable_if_t<stdext::is_container<T>::value, T> from_string(std::string& str)
			{
				T resvec;
				// TODO:: String within Braces
				if (removeBraces(str))
				{
					std::size_t commapos{ findNextCommaSeperator(str) };
					while (commapos != str.npos)
					{
						resvec.push_back(from_string_selector<typename T::value_type>(str.substr(0, commapos - 1)));
						str.erase(0, commapos);
						commapos = findNextCommaSeperator(str);
					};
					resvec.push_back(from_string_selector<typename T::value_type>(str));
				}

				return resvec;
			};

			/// <summary>	Convert string into a pair </summary>
			template <typename T>
			static inline std::enable_if_t<std::is_same<T, std::pair<typename T::first_type, typename T::second_type>>::value, T> from_string(std::string &str)
			{
				removeBraces(str);
				auto pos = findNextCommaSeperator(str);

				auto first = from_string_selector<typename T::first_type>(str);
				str.erase(0, pos);
				auto second = from_string_selector<typename T::second_type>(str);

				afterConversionStringCheck(str);

				return T{ first,second };
			};

			/// <summary>	Convert tuples into a braced string representation. </summary>
			template<typename T>
			static inline std::enable_if_t<(std::tuple_size<T>::value > 0), T> from_string(std::string &str)
			{
				removeBraces(str);
				return buildtupletype<0, T>(str);
			};

#ifdef EIGEN_CORE_H
			template <typename Derived>
			static inline std::enable_if_t<std::is_base_of<Eigen::EigenBase<Derived>, Derived>::value, typename Derived::PlainObject> from_string(std::string& str)
			{
				try
				{
					removeBraces(str);
				}
				catch (Parse_error &e)
				{
					e.append("Cannot create EigenBase<Derived>. ");
					throw e;
				}

				typename Derived::PlainObject ret;

				auto todel = str.find_first_not_of("+-0.123456789E,iI ");
				auto substring = str.substr(0, todel);

				ret << substring;

				str.erase(0, todel);
				afterConversionStringCheck(str);

				return ret;
			};
#endif

		private:
			bool removeBraces(std::string &value)
			{
				auto startbracket{ value.find_first_of(SpecialCharacters::openbracket, 0) };
				auto endbracket{ value.find_last_of(SpecialCharacters::closebracket) };

				if (value.find_first_not_of(" ", 0) != startbracket || value.find_last_not_of(" ") != endbracket)
					throw Parse_error{ Parse_error::error_enum::Invalid_expression };

				if (startbracket == std::string::npos || endbracket == std::string::npos)
					throw Parse_error{ Parse_error::error_enum::Unmatched_Brackets };

				//Erase Brackets
				value.erase(endbracket);
				value.erase(0, startbracket);

				return true;
			}

			static inline void afterConversionStringCheck(std::string &str)
			{
				if (!str.empty())
				{
					if (str.find_first_not_of(" \t\n\v\f\r", 0, 1) != str.npos)
					{
						throw std::invalid_argument{ "String not empty after conversion! " };
					}
					else
					{
						str.clear();
					}
				}

			}

			static inline std::size_t findNextCommaSeperator(std::string &str)
			{
				//TODO: Make this better and faster? Will work but will parse recursive types very inefficient due to multiple parse passes! 
				//	    Could have a temp. cache for faster access

				auto seperatorpos{ str.find_first_of(SpecialCharacters::seperator, 0) };

				std::size_t searchpos{ 0 };
				std::int32_t bracelvl{ 0 };

				while (seperatorpos != str.npos) // do until found
				{
					searchpos = str.find_first_of(SpecialCharacters::openbracket + SpecialCharacters::stringidentifier + SpecialCharacters::closebracket, searchpos);

					if (seperatorpos < searchpos && bracelvl == 0)
						return seperatorpos; //found it !

					if (str.compare(searchpos, SpecialCharacters::stringidentifier.size(), SpecialCharacters::stringidentifier) == 0) //String Identifier Case
					{
						++searchpos;
						while (str.compare(searchpos - (SpecialCharacters::escapestringidentifier.size() - 1), SpecialCharacters::escapestringidentifier.size(), SpecialCharacters::SpecialCharacters::escapestringidentifier) == 0) //Escaped; Find next!
						{
							searchpos = str.find_first_of(SpecialCharacters::stringidentifier, searchpos + 1);

							if (searchpos == str.npos) //Missing string identifier
								throw Parse_error{ Parse_error::error_enum::Missing_string_identifier }; //Nothing found kills the loop!
						};

						if (searchpos > seperatorpos) //wrong comma get next after closing string pos
						{
							++searchpos;
							seperatorpos = str.find_first_of(SpecialCharacters::seperator, searchpos);
							continue;	//Return to start
						}

					}
					else if (str.compare(searchpos, 1, SpecialCharacters::openbracket) == 0)//
					{
						++bracelvl;
						++searchpos; // Find next
						continue; //Return to start
					}
					else if (str.compare(searchpos, 1, SpecialCharacters::closebracket) == 0) //valid closing bracket!
					{
						--bracelvl;
						if (bracelvl == 0) // Thats the intresting case!
						{
							searchpos += 1;
							if (seperatorpos < searchpos) // wrong comma....
								seperatorpos = str.find_first_of(SpecialCharacters::seperator, searchpos);
							continue;
						}
						else if (bracelvl < 0) //too many closing brackets
						{
							throw Parse_error{ Parse_error::error_enum::Missing_open_bracket };
						}
						else
						{
							++searchpos;
							seperatorpos = str.find_first_of(SpecialCharacters::seperator, searchpos);
							continue;
							//Return to start
						}
					}
					else
					{
						std::string err{ "Reached logical not reachable code in " };
						throw std::logic_error{ err + __func__ + " on line " + std::to_string(__LINE__) + " in file " + __FILE__ };
					}
				}

				if (bracelvl > 0) // Too many opening brackets
					throw Parse_error{ Parse_error::error_enum::Missing_close_bracket };

				return seperatorpos;
			}
			/// <summary>	Helper to convert string into tuple types </summary>
			template <std::size_t N, typename Tuple>
			static inline std::enable_if_t< (N != std::tuple_size<Tuple>::value - 2), std::tuple<>> buildtupletype(std::string &str)
			{
				auto seperator{ findNextCommaSeperator(str) };
				using type = std::decay_t<std::tuple_element_t<N, Tuple>>;
				auto head{ from_string_selector<type>(str.substr(0,str - 1)) };
				str.erase(0, str);
				auto tail{ buildtupletype<N,Tuple>(str) };
				return std::tuple_cat(head, tail);
			};
			/// <summary>	Helper to convert string into tuple types </summary>
			template <std::size_t N, typename Tuple>
			static inline std::enable_if_t< (N == std::tuple_size<Tuple>::value - 2), std::tuple<>> buildtupletype(std::string &str)
			{
				auto seperator{ findNextCommaSeperator(str) };
				using type = std::decay_t<std::tuple_element_t<N, Tuple>>;
				using type2 = std::decay_t<std::tuple_element_t<N + 1, Tuple>>;
				auto head{ from_string_selector<type>(str.substr(0,str - 1)) };
				str.erase(0, str);
				auto tail{ from_string_selector<type2>(str) };
				afterConversionStringCheck(str);
				return std::tuple_cat(head, tail);
			};
		};
		
		/// <summary>	Class which has all the to_string Logic for Configuration Files </summary>
		class toString
		{
		public:

			static void checkSyntax(std::string section, std::string key, std::string value);
			///-------------------------------------------------------------------------------------------------
			/// <summary>	Selects the correct to_string implementation for the given type of value. </summary>
			///
			/// <typeparam name="T">	Generic type parameter. </typeparam>
			/// <param name="val">	[in,out] The value. </param>
			///
			/// <returns>	value as a string </returns>
			///-------------------------------------------------------------------------------------------------
			template<typename T>
			static inline std::enable_if_t<traits::use_archive_or_func_to_string_v<std::decay_t<T>, toString>, std::string> to_string_selector(T&& val)
			{
				return to_string(val);
			}

			template<typename T>
			static inline std::enable_if_t<traits::use_type_member_to_string_v<std::decay_t<T>, toString>, std::string> to_string_selector(T&& val)
			{
				return val.to_string();
			}

			template<typename T>
			static inline std::enable_if_t<traits::use_std_to_string_v<std::decay_t<T>, toString>, std::string> to_string_selector(T&& val)
			{
				return std::to_string(val);
			}

			/// <summary>	Special case if parameter is already a string </summary>
			template <typename T>
			static inline std::enable_if_t<std::is_same<std::decay_t<T>, std::string>::value, std::string> to_string(T&& val)
			{
				std::string str{ val };
				auto pos = str.find_first_of(SpecialCharacters::stringidentifier);
				while (pos != str.npos)
				{
					str.replace(pos, SpecialCharacters::stringidentifier.size(), SpecialCharacters::escapestringidentifier); //Escaping or special characters
				}
				return SpecialCharacters::stringidentifier + str + SpecialCharacters::stringidentifier; //Escaping string
			};

			/***End to_string Selector***/

			/// <summary>	Convert numbers into a string representation. </summary>
			template <typename T>
			static inline std::enable_if_t<std::is_arithmetic<std::decay_t<T>>::value, std::string> to_string(T&& val)
			{
				return BasicTools::toStringScientific(val);
				//std::to_string(val); Does a silly rounding to 0.00000 for small doubles (1E-7) 
			};

			/***End Simple Arithmetic numbers***/

			/// <summary>	Convert complex numbers into a string representation. </summary>
			template <typename T>
			static inline std::string to_string(const std::complex<T> &val)
			{
				std::string sign{ (val.imag() < 0 ? "" : "+") };
				return to_string_selector(val.real()) + sign + to_string_selector(val.imag()) + "i";
			};

			/***End complex number***/

			/// <summary>	Convert containers into a braced string representation. </summary>
			template<typename T>
			static inline std::enable_if_t<stdext::is_container<T>::value, std::string> to_string(T&& values)
			{
				std::stringstream sstr;
				sstr << SpecialCharacters::openbracket;
				bool first = true;
				for (const auto& iter : values)
				{
					if (first)
					{
						sstr << to_string_selector(iter);
						first = false;
					}
					else
					{
						sstr << SpecialCharacters::seperator << " " << to_string_selector(iter);
					}

				}
				sstr << SpecialCharacters::closebracket;
				return sstr.str();
			};

			/// <summary>	Convert pairs into a string representation. </summary>
			template <typename T1, typename T2>
			static inline std::string to_string(const std::pair<T1, T2> &val)
			{
				std::stringstream sstr;
				sstr << SpecialCharacters::openbracket << to_string_selector(val.first) << " " << SpecialCharacters::seperator << " " << to_string_selector(val.second) << SpecialCharacters::closebracket;
				return sstr.str();
			};

			/// <summary>	Convert tuples into a braced string representation. </summary>
			template<typename T>
			static inline std::enable_if_t<(std::tuple_size<T>::value > 0), std::string> to_string(const T &val)
			{
				std::stringstream sstr;
				sstr << SpecialCharacters::openbracket;
				sstr << buildtuplestring<std::tuple_size_v<T>-1, T>(val);
				sstr << SpecialCharacters::closebracket;
				return sstr.str();
			};

#ifdef EIGEN_CORE_H
			template <typename Derived>
			static inline std::enable_if_t<std::is_base_of<Eigen::EigenBase<Derived>, Derived>::value, std::string> to_string(Derived&& value)
			{
				Eigen::IOFormat CommaInitFmt(Eigen::FullPrecision, Eigen::DontAlignCols, ", ", ", ", "", "", "{", "}");
				std::stringstream sstr;
				sstr << std::scientific << Position.format(CommaInitFmt);
				return sstr.str();
			};
#endif
		private:
			/// <summary>	Helper to convert tuples into a string. </summary>
			template <std::size_t N, typename Tuple>
			static inline std::enable_if_t< (N > 0), std::string> buildtuplestring(const Tuple &val)
			{
				return buildtuplestring<N - 1, Tuple>(val) + SpecialCharacters::seperator + " " + to_string_selector(std::get<N>(val));
			}
			/// <summary>	Helper to convert tuples into a string. End of Recursion </summary>
			template <std::size_t N, typename Tuple>
			static inline std::enable_if_t< (N == 0), std::string> buildtuplestring(const Tuple &val)
			{
				return to_string_selector(std::get<0>(val));
			};

		};

		/// <summary>	Storage class for Configration File Archives. </summary>
		class Storage
		{
			friend class ConfigFile_OutputArchive;
			friend class ConfigFile_InputArchive;
		protected:
			typedef std::map<std::string, std::string> keyvalues;
			typedef std::map<std::string, keyvalues> sections;

			sections _contents;	//Contents of the CFG
		public:
			void writeContentsToStream(std::ostream &stream) const;
			sections& accessContents() noexcept { return _contents; };
		};

		/// <summary>	Used to Parse the Input Stream </summary>
		class FileParser
		{
		public:
			// Removes commented Linies
			static void removeComment(std::string &line);

			// Checks whether a line contains only whitespaces
			static bool onlyWhitespace(const std::string &line);

			//Checks if a Line is a valid Key Value line (key = value)
			static bool validKeyValueLine(const std::string &line, std::smatch &match);
			static bool validKeyValueLine(const std::string &line);

			//Checks if a Line is a valid Section line ([Sectionparta.partb.partc.partd])
			static bool validSectionLine(const std::string &line, std::smatch &match);
			static bool validSectionLine(const std::string &line);

			// Extracts the Key and Value string from the found match
			static void extractKeyValue(const std::smatch &match, std::string &key, std::string &value);

			// Returns the section string of the found section match
			static std::string extractSection(const std::smatch &match);

			// lineNo = the current line number in the file.
			// line = the current line, with comments removed.
			static void parseLine(const std::string &line, size_t const lineNo, std::string &currentSection, Storage &storage);

			//loads contents from file
			static void loadIntoStorage(std::istream& stream, Storage &storage);
		};

		/// <summary>	Has the logic for the configuration file. </summary>
		class Logic
		{
		private:
			/// <summary>	Appends the curr key to the current section </summary>
			inline void appendCurrKeyToSec()
			{
				if (currentsection.empty())
				{
					currentsection = std::move(currentkey);
				}
				else
				{
					currentsection.append("." + std::move(currentkey));
				}
			};
		protected:
			std::string currentsection{}; // Cache for the current Section
			std::string currentkey{}; //Cache for current key
		public:
			///-------------------------------------------------------------------------------------------------
			/// <summary>	Sets current key. </summary>
			///
			/// <param name="str">	The key string. </param>
			///-------------------------------------------------------------------------------------------------
			void inline setCurrKey(const std::string& str)
			{
				if (!currentkey.empty())
				{
					appendCurrKeyToSec();
				}
				currentkey = str;
			};
			/// <summary>	Resets the current key and maybe section. </summary>
			void inline resetCurrKey()
			{
				if (currentkey.empty())
				{
					currentsection.erase(currentsection.length() - currentkey.length() - 1, currentkey.length());
				}
				else
				{
					currentkey.clear();
				}
			};

			inline std::string& getSection() noexcept { return currentsection; };
			inline std::string& getKey() noexcept { return currentkey; };
		};
	};

	///-------------------------------------------------------------------------------------------------
	/// <summary>	Configuration file output archive. </summary>
	///
	/// <seealso cref="T:OutputArchive{ConfigFile_Out}"/>
	/// <seealso cref="T:ConfigFile_Base"/>
	///-------------------------------------------------------------------------------------------------
	class ConfigFile_OutputArchive : public OutputArchive<ConfigFile_OutputArchive> 
	{
	public:
		ConfigFile_OutputArchive(std::ostream& stream) : OutputArchive(this), mOutputstream(stream) {};
		~ConfigFile_OutputArchive() 
		{ 
			mStorage.writeContentsToStream(mOutputstream);
			mOutputstream << std::flush;
		}

		DISALLOW_COPY_AND_ASSIGN(ConfigFile_OutputArchive)

		template<typename T>
		inline void save(const Archives::NamedValue<T>& value)
		{
			ConfigLogic.setCurrKey(value.getName());
			this->operator()(value.getValue());
			ConfigLogic.resetCurrKey();
		};
		//Saves the data if it is known how to convert the given type to a string. 
		//If the type has a seperate save operation than that one should be most likely used instead of the to_string operation
		template<typename T> 
		inline std::enable_if_t<traits::use_to_string_v<T, ConfigFile::toString, ConfigFile_OutputArchive > > save(const T& val)
		{
			const std::string valstr{ ConfigFile::toString::to_string_selector(val) };
			ConfigFile::toString::checkSyntax(ConfigLogic.getSection(), ConfigLogic.getKey(), valstr);
			mStorage._contents[ConfigLogic.getSection()][ConfigLogic.getKey()] = valstr;
		}

		const ConfigFile::Storage& getStorage() { return mStorage; };
	protected:
		ConfigFile::Logic ConfigLogic{};
	
	private:
		std::string currentsection{}; // Cache for the current Section
		std::string currentkey{}; //Cache for current key

		//template <typename T>
		//std::size_t typecounter{ 0 };
		std::ostream &mOutputstream;
		ConfigFile::Storage mStorage;

	};

	
	///-------------------------------------------------------------------------------------------------
	/// <summary>	Configuration file output archive. </summary>
	///
	/// <seealso cref="T:OutputArchive{ConfigFile_Out}"/>
	/// <seealso cref="T:ConfigFile_Base"/>
	///-------------------------------------------------------------------------------------------------
	class ConfigFile_InputArchive : public InputArchive<ConfigFile_InputArchive>
	{
		
	public:
		ConfigFile_InputArchive(std::istream& stream) : InputArchive(this) ,mInputstream(stream)
		{

		};
		
		ConfigFile_InputArchive(ConfigFile::Storage storage) : InputArchive(this), mInputstream(std::cin), mStorage(std::move(storage))
		{
		};

		DISALLOW_COPY_AND_ASSIGN(ConfigFile_InputArchive)

		template<typename T>
		void load(Archives::NamedValue<T>& value)
		{
			ConfigLogic.setCurrKey(value.getName());
			this->operator()(value.getValue());
			ConfigLogic.resetCurrKey();
		};

		template<typename T>
		std::enable_if_t<traits::use_from_string_v<std::decay_t<T>, ConfigFile::fromString, ConfigFile_OutputArchive>> load(T&& val)
		{
			bool nokey{ false };
			bool nosec{ false };
			if (nosec = currentsection.empty())
			{
				currentsection = typeid(std::decay_t<T>).name() + "_" + std::to_string(typecounter<std::decay_t<T>>)
			}

			using type = typename std::decay_t<T>;
			try
			{
				try
				{
					auto&& sections = mStorage._contents.at(currentsection);
				}
				catch (std::out_of_range &e)
				{
					throw ConfigFile::Parse_error{ ConfigFile::Parse_error::error_enum::Section_not_found };
				}
				if (nokey = currentkey.empty())
					currentkey = typeid(std::decay_t<T>).name() + "_" + std::to_string(typecounter<std::decay_t<T>>)
				T = ConfigFile::fromString::from_string_selector((mStorage._contents.at(currentsection)).at(currentkey));
			}
			catch (ConfigFile::Parse_error &e)
			{
				e.append("Section: "+ currentsection +"! Key: " + currentkey + "! ");
				throw e;
			}
			catch (std::out_of_range &exp)
			{
				auto e = ConfigFile::Parse_error{ ConfigFile::Parse_error::error_enum::Key_not_found }
				e.append("Section: " + currentsection + "! Key: " + currentkey + ". ");
				throw e;
			}

			if (nokey)
				currentkey.clear();
			if(nosec)
				currentsection.clear()
		}

		const ConfigFile::Storage& getStorage() { return mStorage; };
	protected:
		ConfigFile::Logic ConfigLogic{};
	private:
		std::string currentsection{}; // Cache for the current Section
		std::string currentkey{}; //Cache for current key

		std::istream& mInputstream;

		ConfigFile::Storage mStorage;
	};

	//TODO:: Make this the combined Input/Output Class
	class ConfigFile_Archive
	{

	};
}