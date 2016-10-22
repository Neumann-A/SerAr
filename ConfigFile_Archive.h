#pragma once

#include <filesystem>
#include <type_traits>
#include <map>

#include <iosfwd>

#include <string>
#include <regex>

#include <complex>

#include <exception>

#include <Eigen\Core>

#include "..\Basic_Library\Headers\std_extensions.h"

#include "..\Basic_Library\Headers\BasicMacros.h"
#include "..\Basic_Library\Headers\BasicIncludes.h"

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
		template<class T, class Convert, typename ...Args>
		using member_from_string_t = decltype(std::declval<T&>().template from_string<Convert>(std::declval<std::string&>(),std::declval<Args&>()...));
		//Function type for converting values to strings
		
		template<class Convert, typename ...Args>
		using func_from_string_t = decltype(from_string(std::declval<std::string&>(), std::declval<Convert&>(),std::declval<Args&>()...));
		//Std function type for converting values to strings

		//Checks if ToTest has a load function for itself
		template<typename TypeToTest, typename TypeToConvert>
		class has_archive_member_from_string : public stdext::is_detected_exact <std::decay_t<TypeToConvert>, member_from_string_t, TypeToTest, std::decay_t<TypeToConvert> > {};
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
				Key_not_found, Section_not_found, First_line_is_not_section
			};

			Parse_error(const error_enum &err);
			

			const char * what() const noexcept override;

			void append(std::string&& str);

		private:
			Parse_error(const error_enum &err, std::string&&);
			error_enum _err;
			//std::string _msg{};

			static std::string getErrorInfoString(const error_enum &err) noexcept;
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
			static inline std::enable_if_t<std::is_same<std::decay_t<T>, std::string>::value, std::string> from_string(std::string& str)
			{
				//std::string str{ val };
				//auto pos = str.find_first_of(SpecialCharacters::stringidentifier);
				//while (pos != str.npos)
				//{
				//	str.replace(pos, SpecialCharacters::stringidentifier.size(), SpecialCharacters::escapestringidentifier); //Escaping or special characters
				//}
				//return SpecialCharacters::stringidentifier + str + SpecialCharacters::stringidentifier; //Escaping string
				std::string tmp{ std::move(str) };
				auto pos = tmp.find_first_not_of(" \r\v\t\n");
				if (!(tmp.compare(0, 1, SpecialCharacters::stringidentifier) == 0))
				{
					throw Parse_error{ Parse_error::error_enum::Missing_string_identifier };
				}
				tmp.erase(pos, SpecialCharacters::stringidentifier.size());
				pos -= (SpecialCharacters::stringidentifier.size()-1);
				while (pos != tmp.npos)
				{
					pos = tmp.find_first_of(SpecialCharacters::stringidentifier, pos+1);
					if (tmp.compare(pos-SpecialCharacters::escapestringidentifier.size()-1, SpecialCharacters::escapestringidentifier.size(), SpecialCharacters::escapestringidentifier) == 0)
					{
						tmp.replace(pos - SpecialCharacters::escapestringidentifier.size() - 1, SpecialCharacters::escapestringidentifier.size(), SpecialCharacters::stringidentifier); //Remove Escaping of special characters
					}
					else //Found the end '
					{
						tmp.erase(pos, SpecialCharacters::stringidentifier.size());
						if (tmp.find_first_not_of(" \r\v\t\n",pos) != tmp.npos)
						{
							throw Parse_error{ Parse_error::error_enum::Missing_string_identifier };
						}
						break;
					}
				}
				return tmp;
			}

			/***End from_string Selector***/

			/// <summary>	Convert string into number representation. </summary>
			template <typename T>
			static inline std::enable_if_t<std::is_arithmetic<std::decay_t<T>>::value && !std::is_same<std::decay_t<T>, bool>::value, std::decay_t<T>> from_string(std::string &str)
			{
				std::size_t pos;
				const auto num{ BasicTools::stringToNumber<std::decay_t<T>>(str, pos) };
				str.erase(0, pos);

				afterConversionStringCheck(str);
				return num;
			};

			template <typename T>
			static inline std::enable_if_t<std::is_arithmetic<std::decay_t<T>>::value && std::is_same<std::decay_t<T>, bool>::value, std::decay_t<T>> from_string(std::string &str)
			{
				std::regex rx{ "^\\s*[Tt][Rr][Uu][Ee]\\s*$" };
				return std::regex_search(str, rx);
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
			static inline std::enable_if_t<stdext::is_container<T>::value && !std::is_same<std::decay_t<T>, std::string>::value, T> from_string(std::string& str)
			{
				T resvec;
				// TODO:: String within Braces
				if (removeBraces(str))
				{
					std::size_t commapos{ findNextCommaSeperator(str) };
					while (commapos != str.npos)
					{
						std::string tmpstr{ str.substr(0, commapos - 1) };
						resvec.push_back(from_string_selector<typename T::value_type>(tmpstr));
						str.erase(0, commapos+ SpecialCharacters::seperator.size());
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
				auto tmpvec = from_string<std::vector<typename Derived::PlainObject::Scalar>>(str);
				
				typename Derived::PlainObject ret;

				if( (tmpvec.size() != (static_cast<std::size_t>(ret.cols())*static_cast<std::size_t>(ret.rows()))) )
				{
					throw std::runtime_error{"Size of request matrix does not match list of extracted values! Requested: " + 
						std::to_string(ret.cols()*ret.rows()) +" Found: " + std::to_string(tmpvec.size()) + " !" };
				}
								
				ret = Eigen::Map<decltype(ret)>(tmpvec.data(), tmpvec.size());

				afterConversionStringCheck(str);

				return ret;
			};
#endif

		private:
			static inline bool removeBraces(std::string &value)
			{
				auto startbracket{ value.find_first_of(SpecialCharacters::openbracket, 0) };
				auto endbracket{ value.find_last_of(SpecialCharacters::closebracket) };

				if (value.find_first_not_of(" \t\v", 0) != startbracket || value.find_last_not_of(" \t\v") != endbracket)
					throw Parse_error{ Parse_error::error_enum::Invalid_expression };

				if (startbracket == std::string::npos || endbracket == std::string::npos)
					throw Parse_error{ Parse_error::error_enum::Unmatched_Brackets };

				//Erase Brackets
				value.erase(endbracket, SpecialCharacters::closebracket.size());
				value.erase(0, SpecialCharacters::openbracket.size());

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

			static void checkSyntax(const std::string &section, const std::string &key, const std::string &value);
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
			static inline std::enable_if_t<std::is_arithmetic<std::decay_t<T>>::value && !std::is_same<std::decay_t<T>,bool>::value, std::string> to_string(T&& val)
			{
				return BasicTools::toStringScientific(val);
				//std::to_string(val); Does a silly rounding to 0.00000 for small doubles (1E-7) 
			};

			/// <summary>	Convert numbers into a string representation. </summary>
			template <typename T>
			static inline std::enable_if_t<std::is_arithmetic<std::decay_t<T>>::value && std::is_same<std::decay_t<T>, bool>::value, std::string> to_string(T&& val)
			{
				return (val ? std::string{ "TRUE" } : std::string{ "FALSE" });
			};

			/***End Simple Arithmetic numbers***/

			/// <summary>	Convert complex numbers into a string representation. </summary>
			template <typename T>
			static inline std::enable_if_t<std::is_same<T,std::complex<typename T::value_type>>::value, std::string> to_string(const T& val)
			{
				std::string sign{ (val.imag() < 0 ? "" : "+") };
				return to_string_selector(val.real()) + sign + to_string_selector(val.imag()) + "i";
			};

			/***End complex number***/

			/// <summary>	Convert containers into a braced string representation. </summary>
			template<typename T>
			static inline std::enable_if_t<stdext::is_container<T>::value && !std::is_same<T,std::string>::value, std::string> to_string(const T& values)
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
			template <typename T>
			static inline std::enable_if_t<std::is_same<T, std::pair<typename T::x, typename T::y>>::value, std::string> to_string(const T &val)
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
			static inline std::enable_if_t<std::is_base_of<Eigen::EigenBase<std::decay_t<Derived>>, std::decay_t<Derived>>::value, std::string> to_string(const Derived& value)
			{
				Eigen::IOFormat CommaInitFmt(Eigen::FullPrecision, Eigen::DontAlignCols, ", ", ", ", "", "", "{", "}");
				std::stringstream sstr;
				sstr << std::scientific << value.format(CommaInitFmt);
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
			inline sections& accessContents() noexcept { return _contents; };
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
					currentsection = NameStack.top();
				}
				else
				{
					currentsection.append(SectionSeperator + NameStack.top());
				}
			};


			std::stack<std::string> NameStack;
			std::string currentsection{ "" };	// Cache for the current Section //So that we do not have to build it!

			const std::string SectionSeperator{ "." };

		public:
			///-------------------------------------------------------------------------------------------------
			/// <summary>	Sets current key. </summary>
			///
			/// <param name="str">	The key string. </param>
			///-------------------------------------------------------------------------------------------------
			inline void setCurrKey(const std::string& str)
			{
				if (!NameStack.empty())
				{
					appendCurrKeyToSec();
				}
				NameStack.push(str);
			};
			/// <summary>	Resets the current key and maybe section. </summary>
			inline void resetCurrKey()
			{
				NameStack.pop();
				if (!NameStack.empty())
				{
					const auto& top{ NameStack.top() };

					if (NameStack.size() > 1)
						currentsection.erase(currentsection.length() - top.length() - SectionSeperator.length(), currentsection.length());
					else
						currentsection.clear();
				}
			};

			inline const std::string& getSection() noexcept { return currentsection; };
			inline const std::string& getKey() noexcept { return NameStack.top(); };
		};
	};

	class ConfigFile_Options
	{

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
		

		ConfigFile_OutputArchive(std::ostream& stream);
		ConfigFile_OutputArchive(const std::experimental::filesystem::path &path);
		~ConfigFile_OutputArchive();

		ALLOW_DEFAULT_MOVE_AND_ASSIGN(ConfigFile_OutputArchive)
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

		inline const ConfigFile::Storage& getStorage() const noexcept { return mStorage; };
	protected:
		ConfigFile::Logic ConfigLogic{};
	
	private:
		//std::string currentsection{}; // Cache for the current Section
		//std::string currentkey{}; //Cache for current key

		//TODO:: for unnamed values!
		//template <typename T>
		//std::size_t typecounter{ 0 };
		std::ostream &mOutputstream;
		
		bool mStreamOwner{ false };
		
		ConfigFile::Storage mStorage;

		std::ofstream& createFileStream(const std::experimental::filesystem::path &path);
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
		ConfigFile_InputArchive(std::istream& stream);
		ConfigFile_InputArchive(ConfigFile::Storage storage);
		ConfigFile_InputArchive(const std::experimental::filesystem::path &path);
		~ConfigFile_InputArchive();

		ALLOW_DEFAULT_MOVE_AND_ASSIGN(ConfigFile_InputArchive)
		DISALLOW_COPY_AND_ASSIGN(ConfigFile_InputArchive)

		auto list(const Archives::NamedValue<decltype(nullptr)>& value) -> typename ConfigFile::Storage::keyvalues;

		template<typename T>
		std::enable_if_t<std::is_same<T, typename Archives::NamedValue<T>::Type>::value, ConfigFile::Storage::keyvalues> list(const Archives::NamedValue<T>& value)
		{
			ConfigLogic.setCurrKey(value.getName());
			const auto tmp {list(value.getValue())};
			ConfigLogic.resetCurrKey();
			return tmp;
		};

		auto list(std::string value) -> typename ConfigFile::Storage::keyvalues;
		auto list() -> typename ConfigFile::Storage::sections;

		template<typename T>
		void load(Archives::NamedValue<T>& value)
		{
			ConfigLogic.setCurrKey(value.getName());
			this->operator()(value.getValue());
			ConfigLogic.resetCurrKey();
		};

		template<typename T>
		std::enable_if_t<traits::use_from_string_v<std::decay_t<T> , ConfigFile::fromString, ConfigFile_InputArchive> > load(T&& val)
		{			
			const auto& currentsection{ ConfigLogic.getSection() };
			const auto& currentkey{ ConfigLogic.getKey() };

			//const auto& nosec{ currentsection.empty() };
			//const auto& nokey{ currentkey.empty() };
			
			////TODO:: Empty Section;
			//if (nosec = currentsection.empty())
			//{
			//	currentsection = typeid(std::decay_t<T>).name() + "_" + std::to_string(typecounter<std::decay_t<T>>)
			//}

			using type = typename std::decay_t<T>;
			try
			{
				try
				{
					mStorage._contents.at(currentsection);
				}
				catch (std::out_of_range &)
				{
					throw ConfigFile::Parse_error{ ConfigFile::Parse_error::error_enum::Section_not_found };
				}
				//TODO:: Empty Key
				//if (nokey = currentkey.empty())
				//	currentkey = typeid(std::decay_t<T>).name() + "_" + std::to_string(typecounter<std::decay_t<T>>)
				std::string valstr{ (mStorage._contents.at(currentsection)).at(currentkey) };
				val = ConfigFile::fromString::from_string_selector<T>(valstr);
			}
			catch (ConfigFile::Parse_error &e)
			{
				e.append("Section: "+ currentsection +"! Key: " + currentkey + "! ");
				throw e;
			}
			catch (std::out_of_range &)
			{
				auto e = ConfigFile::Parse_error{ ConfigFile::Parse_error::error_enum::Key_not_found };
				e.append("Section: " + currentsection + "! Key: " + currentkey + "! ");
				throw e;
			}
			catch (std::runtime_error &e)
			{
				const auto str{ std::string{ e.what() }+" Section: " + currentsection + "! Key: " + currentkey + "! " };
				std::runtime_error exp{ str };
				throw exp;
			}

			//if (nokey)
			//	currentkey.clear();
			//if (nosec)
			//	currentsection.clear();
		}

		inline const ConfigFile::Storage& getStorage() const noexcept { return mStorage; };
	protected:
		ConfigFile::Logic ConfigLogic{};
	private:
		//std::string currentsection{}; // Cache for the current Section
		//std::string currentkey{}; //Cache for current key

		std::istream& mInputstream;
		bool mStreamOwner{ false };
		ConfigFile::Storage mStorage;

		std::ifstream& ConfigFile_InputArchive::createFileStream(const std::experimental::filesystem::path &path);

		void parseStream();

	};

	//TODO:: Make this the combined Input/Output Class
	class ConfigFile_Archive
	{

	};
}