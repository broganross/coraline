///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008-2010, Sony Pictures Imageworks Inc
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
// Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
// Neither the name of the organization Sony Pictures Imageworks nor the
// names of its contributors
// may be used to endorse or promote products derived from this software
// without specific prior written permission.
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS
// IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER
// OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///////////////////////////////////////////////////////////////////////////////

#ifndef CORAL_STRINGUTILS_H
#define CORAL_STRINGUTILS_H

#include <cstdio> 
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

//CORAL_EXPORT
#include "coralDefinitions.h"

// originally called pystring, the namespace has been modified to fit in nicely.
namespace stringUtils
{

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// This is a set of functions matching the interface and behaviors of python string methods
    /// (as of python 2.3) using std::string.
    ///
    /// Overlapping functionality ( such as index and slice/substr ) of std::string is included
    /// to match python interfaces.
    ///

    #define MAX_32BIT_INT 2147483647

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Return a copy of the string with only its first character capitalized.
    ///
    CORAL_EXPORT std::string capitalize( const std::string & str );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Return centered in a string of length width. Padding is done using spaces.
    ///
    CORAL_EXPORT std::string center( const std::string & str, int width );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Return the number of occurrences of substring sub in string S[start:end]. Optional
    /// arguments start and end are interpreted as in slice notation.
    ///
    CORAL_EXPORT int count( const std::string & str, const std::string & substr, int start = 0, int end = MAX_32BIT_INT);

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Return True if the string ends with the specified suffix, otherwise return False. With
    /// optional start, test beginning at that position. With optional end, stop comparing at that position.
    ///
    CORAL_EXPORT bool endswith( const std::string & str, const std::string & suffix, int start = 0, int end = MAX_32BIT_INT );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Return a copy of the string where all tab characters are expanded using spaces. If tabsize
    /// is not given, a tab size of 8 characters is assumed.
    ///
    CORAL_EXPORT std::string expandtabs( const std::string & str, int tabsize = 8);

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Return the lowest index in the string where substring sub is found, such that sub is
    /// contained in the range [start, end). Optional arguments start and end are interpreted as
    /// in slice notation. Return -1 if sub is not found.
    ///
    CORAL_EXPORT int find( const std::string & str, const std::string & sub, int start = 0, int end = MAX_32BIT_INT  );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Synonym of find right now. Python version throws exceptions. This one currently doesn't
    ///
    CORAL_EXPORT int index( const std::string & str, const std::string & sub, int start = 0, int end = MAX_32BIT_INT  );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Return true if all characters in the string are alphanumeric and there is at least one
    /// character, false otherwise.
    ///
    CORAL_EXPORT bool isalnum( const std::string & str );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Return true if all characters in the string are alphabetic and there is at least one
    /// character, false otherwise
    ///
    CORAL_EXPORT bool isalpha( const std::string & str );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Return true if all characters in the string are digits and there is at least one
    /// character, false otherwise.
    ///
    CORAL_EXPORT bool isdigit( const std::string & str );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Return true if all cased characters in the string are lowercase and there is at least one
    /// cased character, false otherwise.
    ///
    CORAL_EXPORT bool islower( const std::string & str );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Return true if there are only whitespace characters in the string and there is at least
    /// one character, false otherwise.
    ///
    CORAL_EXPORT bool isspace( const std::string & str );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Return true if the string is a titlecased string and there is at least one character,
    /// i.e. uppercase characters may only follow uncased characters and lowercase characters only
    /// cased ones. Return false otherwise.
    ///
    CORAL_EXPORT bool istitle( const std::string & str );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Return true if all cased characters in the string are uppercase and there is at least one
    /// cased character, false otherwise.
    ///
    CORAL_EXPORT bool isupper( const std::string & str );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Return a string which is the concatenation of the strings in the sequence seq.
    /// The separator between elements is the str argument
    ///
    CORAL_EXPORT std::string join( const std::string & str, const std::vector< std::string > & seq );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Return the string left justified in a string of length width. Padding is done using
    /// spaces. The original string is returned if width is less than str.size().
    ///
    CORAL_EXPORT std::string ljust( const std::string & str, int width );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Return a copy of the string converted to lowercase.
    ///
    CORAL_EXPORT std::string lower( const std::string & str );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Return a copy of the string with leading characters removed. If chars is omitted or None,
    /// whitespace characters are removed. If given and not "", chars must be a string; the
    /// characters in the string will be stripped from the beginning of the string this method
    /// is called on (argument "str" ).
    ///
    CORAL_EXPORT std::string lstrip( const std::string & str, const std::string & chars = "" );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Split the string around first occurance of sep.
    /// Three strings will always placed into result. If sep is found, the strings will
    /// be the text before sep, sep itself, and the remaining text. If sep is
    /// not found, the original string will be returned with two empty strings.
    ///
    CORAL_EXPORT void partition( const std::string & str, const std::string & sep, std::vector< std::string > & result );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Return a copy of the string with all occurrences of substring old replaced by new. If
    /// the optional argument count is given, only the first count occurrences are replaced.
    ///
    CORAL_EXPORT std::string replace( const std::string & str, const std::string & oldstr, const std::string & newstr, int count = -1);

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Return the highest index in the string where substring sub is found, such that sub is
    /// contained within s[start,end]. Optional arguments start and end are interpreted as in
    /// slice notation. Return -1 on failure.
    ///
    CORAL_EXPORT int rfind( const std::string & str, const std::string & sub, int start = 0, int end = MAX_32BIT_INT );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Currently a synonym of rfind. The python version raises exceptions. This one currently
    /// does not
    ///
    CORAL_EXPORT int rindex( const std::string & str, const std::string & sub, int start = 0, int end = MAX_32BIT_INT );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Return the string right justified in a string of length width. Padding is done using
    /// spaces. The original string is returned if width is less than str.size().
    ///
    CORAL_EXPORT std::string rjust( const std::string & str, int width);

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Split the string around last occurance of sep.
    /// Three strings will always placed into result. If sep is found, the strings will
    /// be the text before sep, sep itself, and the remaining text. If sep is
    /// not found, the original string will be returned with two empty strings.
    ///
    CORAL_EXPORT void rpartition( const std::string & str, const std::string & sep, std::vector< std::string > & result );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Return a copy of the string with trailing characters removed. If chars is "", whitespace
    /// characters are removed. If not "", the characters in the string will be stripped from the
    /// end of the string this method is called on.
    ///
    CORAL_EXPORT std::string rstrip( const std::string & str, const std::string & chars = "" );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Fills the "result" list with the words in the string, using sep as the delimiter string.
    /// If maxsplit is > -1, at most maxsplit splits are done. If sep is "",
    /// any whitespace string is a separator.
    ///
    CORAL_EXPORT void split( const std::string & str, std::vector< std::string > & result, const std::string & sep = "", int maxsplit = -1);

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Fills the "result" list with the words in the string, using sep as the delimiter string.
    /// Does a number of splits starting at the end of the string, the result still has the
    /// split strings in their original order.
    /// If maxsplit is > -1, at most maxsplit splits are done. If sep is "",
    /// any whitespace string is a separator.
    ///
    CORAL_EXPORT void rsplit( const std::string & str, std::vector< std::string > & result, const std::string & sep = "", int maxsplit = -1);

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Return a list of the lines in the string, breaking at line boundaries. Line breaks
    /// are not included in the resulting list unless keepends is given and true.
    ///
    CORAL_EXPORT void splitlines(  const std::string & str, std::vector< std::string > & result, bool keepends = false );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Return True if string starts with the prefix, otherwise return False. With optional start,
    /// test string beginning at that position. With optional end, stop comparing string at that
    /// position
    ///
    CORAL_EXPORT bool startswith( const std::string & str, const std::string & prefix, int start = 0, int end = MAX_32BIT_INT );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Return a copy of the string with leading and trailing characters removed. If chars is "",
    /// whitespace characters are removed. If given not "",  the characters in the string will be
    /// stripped from the both ends of the string this method is called on.
    ///
    CORAL_EXPORT std::string strip( const std::string & str, const std::string & chars = "" );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Return a copy of the string with uppercase characters converted to lowercase and vice versa.
    ///
    CORAL_EXPORT std::string swapcase( const std::string & str );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Return a titlecased version of the string: words start with uppercase characters,
    /// all remaining cased characters are lowercase.
    ///
    CORAL_EXPORT std::string title( const std::string & str );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Return a copy of the string where all characters occurring in the optional argument
    /// deletechars are removed, and the remaining characters have been mapped through the given
    /// translation table, which must be a string of length 256.
    ///
    CORAL_EXPORT std::string translate( const std::string & str, const std::string & table, const std::string & deletechars = "");

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Return a copy of the string converted to uppercase.
    ///
    CORAL_EXPORT std::string upper( const std::string & str );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Return the numeric string left filled with zeros in a string of length width. The original
    /// string is returned if width is less than str.size().
    ///
    CORAL_EXPORT std::string zfill( const std::string & str, int width );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief function matching python's slice functionality.
    ///
    CORAL_EXPORT std::string slice( const std::string & str, int start = 0, int end = MAX_32BIT_INT);
	
	CORAL_EXPORT float parseFloat( const std::string & str );
	CORAL_EXPORT int parseInt( const std::string & str );
	CORAL_EXPORT std::string boolToString(bool value);
	CORAL_EXPORT std::string intToString(int value);
	CORAL_EXPORT std::string floatToString(float value);
	
	template <class T>
	std::string vectorToString(const std::vector<T> &vec){
		std::ostringstream stream;
		stream << "[";
		
		int vecSize = vec.size();
		for(int i = 0; i < vecSize; ++i){
			stream << "'" << vec[i] << "'";
			
			if(i < vecSize - 1){
				stream << ",";
			}
		}
		stream << "]";
		
		return stream.str();
	}
}

#endif
