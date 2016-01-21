///
/// @file   Utility.hh
///
/// @date   20 Dec 2014
/// @author Don McNeill dmcneill@me.com
///
#ifndef UTILITY_HH_
#define UTILITY_HH_

//  Includes
#include <vector>
#include <string>
#include <stdint.h>
#include <vector>
#include "Types.hh"     // convenience datatypes


//  Namespace
namespace isp {

///
/// @brief      Utility class for the ISP client.
///
/// @details    This class contains the support methods necessary to implement
///              the ISP client.
///
class Utility
{
public:

    ///
    /// @brief      Print the raw hex and ASCII representation of the data to
    ///             stdout.
    ///
    /// @param[in]  psBlock
    ///             A byte pointer to the block to dump.
    ///
    /// @param[in]  ulSize
    ///             The number of bytes to dump.
    ///
    /// @param[in]  offset
    ///             The optional offset to be added to the printout for the
    ///             address.
    ///
    static void hexDump(const uint8_t * psBlock,
                        size_t ulSize,
                        uint32_t offset = 0U);


    ///
    /// @brief      Get the integer value of a string.
    ///
    /// @param[in]  str
    ///             The string holding the value to convert.
    ///
    /// @return     Integer value for the string; set to neg one
    ///             on error.
    ///
    static int stringToInt(std::string& str);

    ///
    /// @brief      Convert an ASCII hex string to a byte vector.
    ///
    /// @param[in]  input
    ///             The input ASCII hex string to examine.
    ///
    /// @param[in]  bytes
    ///             The byte vector to fill in with the raw data.
    ///
    static void stringToByte(const std::string& input,
                             std::vector<uint8_t>& bytes);
    ///
    /// @brief      Get a random number.
    ///
    /// @details    Generate a random number in a set range.
    ///
    /// @return     The unsigned random number to use.
    ///
    static unsigned getUnsignedRandom();

    ///
    /// @brief      Cut the last character from the string.
    ///
    /// @details    Erase the last character in the given string.
    ///
    /// @param[in,out]  string  The string to cut.
    ///
    static void cutLast(
                    std::string& string );

    ///
    /// @brief      Trim the spaces from the left side.
    ///
    /// @param[in,out]  str
    ///                 The string to trim.
    ///
    /// @return     A reference to the trimmed string.
    ///
    static std::string& leftTrim(std::string& str);

    ///
    /// @brief      Trim the spaces from the right side.
    ///
    /// @param[in,out]  str
    ///                 The string to trim.
    ///
    /// @return     A reference to the trimmed string.
    ///
    static std::string& rightTrim(std::string& str);

    ///
    /// @brief      Trim the spaces on both sides.
    ///
    /// @param[in,out]  str
    ///                 The string to trim.
    ///
    /// @return     A reference to the trimmed string.
    ///
    static std::string& trim(std::string& str);


    ///
    /// @brief      Split a string into a vector by delimiter.
    ///
    /// @param[in]  str
    ///             The input string to split.
    ///
    /// @param[in]  delimiter
    ///             The delimiter string to use.
    ///
    /// @param[out] vec
    ///             The resultant string vector.
    ///
    static void split(const std::string& str,
                      const char * delimiter,
                      std::vector<std::string>& vec);

    ///
    /// @brief      Get the substring starting just beyond the matching
    ///             pattern.
    ///
    /// @details    Check for the presence of the pattern in the given
    ///             string and remove the preceding string data up to
    ///             and including the pattern.
    ///
    /// @param[out] string   A reference to the string to be examined and cut.
    /// @param[in]  pattern  The constant string pointer to the pattern to look
    ///             for in the given string.
    ///
    /// @retval     true    Success - returned string has been cut.
    /// @retval     false   Failure - pattern not found - no action taken.
    ///
    static bool cutString(
                    std::string& string,
                    const char * pattern );
    ///
    /// @brief      Get the substring starting just beyond the matching
    ///             pattern and just before the end pattern.
    ///
    /// @details    Check for the presence of the patterns in the given
    ///             string and remove the preceding string data up to
    ///             and including the start pattern and remove the end
    ///             pattern and any data beyond the end pattern in the
    ///             given string.  Note that BOTH patterns should be
    ///             present to allow the input string to be changed.
    ///
    /// @param[out] string          A reference to the string to be examined
    ///                             and cut.
    /// @param[in]  startPattern    The constant string pointer to the starting
    ///                             pattern to look for in the given string.
    /// @param[in]  endPattern      The constant string pointer to the ending
    ///                             pattern to look for in the given string.
    ///
    /// @retval     true    Success - returned string has been cut.
    /// @retval     false   Failure - pattern not found - no action taken.
    ///
    static bool cutString(
                    std::string& string,
                    const char * startPattern,
                    const char * endPattern );

    ///
    /// @brief      Get the substring starting just beyond the matching
    ///             pattern and just before the end pattern repeatedly,
    ///             saving the substring to the given string vector.
    ///
    /// @details    Check for the presence of the patterns in the given
    ///             string and remove the preceding string data up to
    ///             and including the start pattern and remove the end
    ///             pattern and any data beyond the end pattern in the
    ///             given string.  Note that BOTH patterns should be
    ///             present for an entry to be made to the string
    ///             vector.
    ///
    /// @param[in]  string          A copy of the string to be examined.
    /// @param[in]  startPattern    The constant string pointer to the starting
    ///                             pattern to look for in the given string.
    /// @param[in]  endPattern      The constant string pointer to the ending
    ///                             pattern to look for in the given string.
    /// @param[out] stringVector    The string vector reference to be filled
    ///                             with the substrings.
    ///
    /// @return     The number of substrings added to the string
    ///             vector.
    ///
    static int cutToVector(
                    std::string string,
                    const char * startPattern,
                    const char * endPattern,
                    tStringVector& stringVector );

    ///
    /// @brief      Get the substring starting at the matching pattern
    ///             and just after the end pattern.
    ///
    /// @details    Check for the presence of the patterns in the given
    ///             string and remove the string data just before the
    ///             start pattern and remove the data after the end
    ///             pattern in the given string.  Note that BOTH
    ///             patterns should be present to allow the input
    ///             string to be changed.
    ///
    /// @param[out] string          A reference to the string to be examined
    ///                             and trimmed.
    /// @param[in]  startPattern    The constant string pointer to the starting
    ///                             pattern to look for in the given string.
    /// @param[in]  endPattern      The constant string pointer to the ending
    ///                             pattern to look for in the given string.
    ///
    /// @retval     true    Success - returned string has been trimmed.
    /// @retval     false   Failure - pattern not found - no action taken.
    ///
    static bool trimString(
                    std::string& string,
                    const char * startPattern,
                    const char * endPattern );

    ///
    /// @brief      Translate an unsigned number to a string with
    ///             leading zeroes.
    ///
    /// @details    Translate an unsigned integer value to a string
    ///             representation with a given size.  If the number
    ///             width is less than the given width, the leading
    ///             portion is padded with zeroes.
    ///
    /// @param[in]  value           The unsigned value to use.
    ///
    /// @param[in]  width           The width of the field to use for
    ///                             the unsigned value in the string.
    ///
    /// @return     The string object with the leading digits zero
    ///             filled.
    ///
    static std::string  UnsignedToString(
                    unsigned value,
                    unsigned width = 10 );

    ///
    /// @brief      Get the current time string in GMT.
    ///
    /// @details    Provide a time string of the form:
    ///             "day, dd mmm yyyy hh:mm:ss TZ", where the day
    ///             is the current day of the week and TZ is the
    ///             timezone abbreviation.
    ///
    /// @param[out] string          The string is filled in with
    ///                             the properly formatted time
    ///                             string.
    ///
    /// @return     Boolean true on success and false on error.
    ///
    static bool getGMTTime(
                    std::string& string );

    ///
    /// @brief      Get the current local time.
    ///
    /// @details    Get the current time in time_t format for
    ///             the inform header.
    ///
    /// @return     The current time in time_t format.
    ///
    static time_t getLocalTime();

    ///
    /// @brief  Extract the directory path from the string.
    ///
    /// @param[in]  path        The path string to examine.
    /// @param[in]  delimiter   The single character delimiter for separation.
    ///
    /// @return     Returns everything, including the trailing path separator,
    ///             except the filename part of the path.  Example:
    ///             "/foo/bar/baz.txt" --> "/foo/bar/"
    ///
    static std::string ExtractDirectory(
                    const std::string& path,
                    char delimiter = '/' );

    ///
    /// @brief  Extract the filename string from the path.
    ///
    /// @param[in]  path        The path string to examine.
    /// @param[in]  delimiter   The single character delimiter for separation.
    ///
    /// @return     Returns only the filename portion of the path.
    ///             Example: "/foo/bar/baz.txt" --> "baz.txt".
    ///
    static std::string ExtractFilename(
                    const std::string& path,
                    char delimiter = '/' );

    ///
    /// @brief  Extract the filename's extension string from the path.
    ///
    /// @param[in]  path        The path string to examine.
    /// @param[in]  delimiter   The single character delimiter for separation.
    ///
    /// @return     Returns only the extension of the filename portion of the path.
    ///             Example: "/foo/bar/baz.txt" --> ".txt".
    ///             Example: "/foo/bar/baz" --> "".
    ///
    static std::string ExtractFileExtension(
                    const std::string& path,
                    char delimiter = '/' );
private:
    ///
    /// @brief      Utility class constructor.
    ///
    /// @details    Force the use of the explicit constructor by not
    ///             allowing the default constructor to exist.
    ///
    Utility() {}

    ///
    /// @brief      Copy constructor
    ///
    /// @details    Make the class non-copyable.
    ///
    /// @param[in]  utility     Reference to the Utility object
    ///                         to be copied.
    ///
    Utility(const Utility& utility) = delete;

    ///
    /// @brief      Assignment operator
    ///
    /// @details    Make the class non-assignable.
    ///
    /// @param[in]  utility     Reference to the Utility object
    ///                         to be copied.
    ///
    Utility& operator = (const Utility& utility) = delete;

    ///
    /// @brief      Utility class destructor.
    ///
    /// @details    NOT USED
    ///
    virtual ~Utility() {}
};  // class

} // namespace
#endif
