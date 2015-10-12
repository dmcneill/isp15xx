///
/// @file   Utility.cc
///
/// @date   20 Dec 2014
/// @author Don McNeill dmcneill@me.com
///


// Includes
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <algorithm>
#include <ifaddrs.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include "Log.hh"
#include "Utility.hh"


// Definitions
#define DUMP_LENGTH_OUT          (16)
#define DUMP_ASCII_STRING_SIZE   (20)
#define DUMP_HEX_STRING_SIZE     (60)

//
//  @brief      Print the raw hex and ASCII representation of the data to
//              stdout.
//
void isp::Utility::hexDump( const uint8_t * psBlock,
                            size_t ulSize,
                            uint32_t offset )
{
    char        msgString   [DUMP_HEX_STRING_SIZE];
    char        asciiString [DUMP_ASCII_STRING_SIZE];
    char *      p2String;
    uint8_t *   p2Data;
    uint32_t    i, j;

    // Test for nonsense
    if( !ulSize )
        return;

    //
    //  Print the leading message text
    //
    p2Data = const_cast<uint8_t *>( psBlock );
    p2String = &msgString[0];
    *p2String = '\0';

    for( j = 0; j <= DUMP_LENGTH_OUT; j++ )
        asciiString[j] = 0;

    //
    //  Dump the block out
    //
    for( i = 0; i < ulSize; i++ )
    {
        char chData = p2Data[i];
        if( chData < ' ' || chData >= 127 )
            chData = '.';
        asciiString[i % DUMP_LENGTH_OUT] = chData;

        if( ( i % DUMP_LENGTH_OUT ) == ( DUMP_LENGTH_OUT - 1 ) )
        {
            p2String += snprintf( p2String,
                                  8,
                                  "%02X ",
                                  p2Data[i] );
            LOG( INFO ) << std::hex << std::setw(8) << std::setfill( '0' )
                        << (( i - ( DUMP_LENGTH_OUT - 1 )) + offset )
                        << " " << msgString
                        << " " << asciiString;

            p2String = &msgString[0];
            *p2String = 0;
            for( j = 0; j < DUMP_LENGTH_OUT; j++ )
                asciiString[j] = 0;
        }
        else
        {
            p2String += snprintf( p2String,
                                  8,
                                  "%02X ",
                                  p2Data[i] );
        }
    }

    if( strlen( msgString ) )
    {
        for( j = i % DUMP_LENGTH_OUT; j < DUMP_LENGTH_OUT; j++ )
            p2String += snprintf( p2String, 8, "   " );

        LOG( INFO ) << std::hex << std::setw(8) << std::setfill( '0' )
                    << ((( i / DUMP_LENGTH_OUT ) * DUMP_LENGTH_OUT) + offset)
                    << " " << msgString
                    << " " << asciiString;
    }
    return;
}


//
//  @brief      Get the integer value of a string.
//
int isp::Utility::stringToInt( std::string& str )
{
    int value = -1;

    try
    {
        value = stoi( str );
    }
    catch (...)
    {
        value = -1;
    }
    return value;
}


//
//  @brief      Convert an ASCII hex string to a byte vector.
//
void isp::Utility::stringToByte( const std::string& input,
                                 std::vector<uint8_t>& bytes )
{
    static const char * const lut = "0123456789ABCDEF";

    for (size_t i = 0; i < input.length(); i += 2)
    {
        char  a = input[i];
        const char* p = std::lower_bound(lut, lut + 16, a);
        if (*p != a)
            break; // throw std::invalid_argument("not a hex digit");

        char b = input[i + 1];
        const char* q = std::lower_bound(lut, lut + 16, b);
        if (*q != b)
            break; // throw std::invalid_argument("not a hex digit");

        uint8_t byte = (((p - lut) << 4) | (q - lut));
        bytes.push_back(byte);
    }
}


//
//  @brief      Get a random number.
//
unsigned isp::Utility::getUnsignedRandom()
{
    unsigned seed = time( NULL );
    int random = rand_r( &seed );

    if( 0 > random )
        random = -random;

    if( 10000 < random )
        random %= 10000;

    return static_cast<unsigned>( random );
}


//
//  @brief      Cut the last character from the string.
//
void isp::Utility::cutLast(
                std::string& string )
{
    if( string.empty() )
        return;

    string.erase( string.size() - 1, 1 );
}


//
//  @brief      Trim the spaces from the left side.
//
std::string& isp::Utility::leftTrim( std::string& str)
{
    str.erase( str.begin(),
               std::find_if( str.begin(),
                             str.end(),
                             std::not1( std::ptr_fun<int, int>( std::isspace ))));
    return str;
}


//
//  @brief      Trim the spaces from the right side.
//
std::string& isp::Utility::rightTrim( std::string& str )
{
    str.erase( std::find_if( str.rbegin(),
                             str.rend(),
                             std::not1( std::ptr_fun<int, int>(std::isspace))).base(),
               str.end());
    return str;
}


//
//  @brief      Trim the spaces on both sides.
//
std::string& isp::Utility::trim( std::string& str)
{
    return isp::Utility::leftTrim(isp::Utility::rightTrim( str ));
}


//
//  @brief      Split a string into a vector by delimiter.
//
void isp::Utility::split( const std::string& str,
                          const char * delimiter,
                          std::vector<std::string>& vec)
{
    std::string::size_type i = 0;
    std::string::size_type j = str.find(delimiter);
    size_t siz = strlen( delimiter );

    while (j != std::string::npos)
    {
        vec.push_back( str.substr(i, j - i) );

        i = j + siz;
        j = str.find( delimiter, i );

        if (j == std::string::npos)
        {
            std::string tmp = str.substr(i, str.length());

            vec.push_back( tmp );
        }
    }
}


//
//  @brief      Get the substring starting just beyond the matching
//              pattern.
//
bool isp::Utility::cutString(
                std::string& string,
                const char * pattern )
{
    bool  result = false;

    do
    {
        if( ( NULL == pattern )
         || ( string.length() < ( strlen( pattern ))) )
           break;

        size_t   found  = string.find( pattern );
        size_t   length = strlen( pattern );

        if( found != std::string::npos )
        {
            std::string tmp = string.substr(
                            found + length,
                            string.length() - ( found + length ));

            string = tmp;
            result = true;
        }

    } while( false );

    return result;
}


//
// @brief       Get the substring starting just beyond the matching
//              pattern and just before the end pattern.
//
bool isp::Utility::cutString(
                std::string& string,
                const char * startPattern,
                const char * endPattern )
{
    bool  result = false;

    do
    {
        if( ( NULL == startPattern )  ||
            ( NULL == endPattern )    ||
            ( string.length() <
                        ( strlen( startPattern ) + strlen( endPattern ))) )
           break;

        std::string   tmp;
        size_t        found = string.find( startPattern );
        size_t        startLen = strlen( startPattern );

        if( found != std::string::npos )
        {
            tmp = string.substr( found + startLen,
                        string.length() - ( found + startLen ));
            result = true;
        }

        if( true == result )
        {
            found = tmp.find( endPattern );
            if( found != std::string::npos )
            {
                string = tmp.substr( 0, found );
                result = true;
            }
        }
    } while( false );

    return result;
}


//
// @brief       Get the substring starting just beyond the matching
//              pattern and just before the end pattern and fill in
//              a vector.
//
int isp::Utility::cutToVector(
                std::string string,
                const char * startPattern,
                const char * endPattern,
                tStringVector& stringVector )
{
    bool  result = false;

    do
    {
        if( ( NULL == startPattern )  ||
            ( NULL == endPattern )    ||
            ( string.length() <
                        ( strlen( startPattern ) + strlen( endPattern ))) )
           break;

        std::string   tmp;
        size_t        startLen = strlen( startPattern );
        size_t        endLen = strlen( endPattern );
        size_t        found = 0;

        // Clear out the string vector.
        stringVector.clear();

        do
        {
            found = string.find( startPattern );

            if( found != std::string::npos )
            {
                tmp = string.substr(
                            found + startLen,
                            string.length() - ( found + startLen ));
                result = true;
            }
            else
            {
                break;
            }

            if( true == result )
            {
                found = tmp.find( endPattern );
                if( found != std::string::npos )
                {
                    std::string vstr = tmp.substr( 0, found );
                    stringVector.push_back( vstr );

                    string = tmp.substr(
                                found + endLen,
                                tmp.length() - ( found + endLen ));
                    result = true;
                }
            }
        }
        while( found != std::string::npos );

    } while( false );

    return stringVector.size();
}


//
// @brief       Get the substring starting at the matching pattern
//              and just after the end pattern.
//
bool isp::Utility::trimString(
                std::string& string,
                const char * startPattern,
                const char * endPattern )
{
    bool  result = false;

    do
    {
        if( ( NULL == startPattern )  ||
            ( NULL == endPattern )    ||
            ( string.length() <
                        ( strlen( startPattern ) + strlen( endPattern ))) )
           break;

        std::string   tmp;
        size_t        found = string.find( startPattern );

        if( found != std::string::npos )
        {
            tmp = string.substr(
                        found,
                        string.length() - found );
            result = true;
        }

        if( true == result )
        {
            found = tmp.find( endPattern );
            if( found != std::string::npos )
            {
                size_t  size = strlen( endPattern ) + found;

                string = tmp.substr( 0, size );
                result = true;
            }
        }
    } while( false );

    return result;
}


//
//  @brief      Translate an unsigned number to a string with
//              leading zeroes.
//
std::string isp::Utility::UnsignedToString(
                unsigned value,
                unsigned width )
{
    std::stringstream ss;

    ss << std::setw( width )
       << std::setfill('0')
       << value;
    return ss.str();
}


//
//  @brief      Get the current time string in GMT.
//
bool isp::Utility::getGMTTime(
                std::string& string )
{
    char        buffer[ 80 + 1] = { 0 };
    struct tm   tmResult;
    time_t      currentTime = time( NULL );
    bool        result = false;

    if( NULL != gmtime_r( &currentTime, &tmResult ))
    {
        strftime( buffer, 80, "%a %d %b %Y %T %Z", &tmResult );
        string = buffer;
        result = true;
    }
    return result;
}


//
//  @brief      Get the current time string in local time.
//
time_t isp::Utility::getLocalTime()
{
    struct tm   tmResult;
    time_t      currentTime = time( NULL );

    if( NULL != localtime_r( &currentTime, &tmResult ))
    {
        currentTime = mktime( &tmResult );
    }
    return currentTime;
}


///
/// @brief  Extract the directory path from the string.
///
std::string isp::Utility::ExtractDirectory(
                const std::string& path,
                char delimiter)
{
    size_t found = path.find_last_of( delimiter );
    if (found != std::string::npos )
    {
        return path.substr( 0, found + 1 );
    }
    else
    {
        return std::string();
    }
}


///
/// @brief  Extract the filename string from the path.
///
std::string isp::Utility::ExtractFilename(
                const std::string& path,
                char delimiter)
{
    size_t found = path.find_last_of( delimiter );

    if (found != std::string::npos )
    {
        return path.substr( found + 1 );
    }
    else
    {
        return std::string();
    }      
}


///
/// @brief  Extract the filename's extension string from the path.
///
std::string isp::Utility::ExtractFileExtension(
                const std::string& path,
                char delimiter)
{
    std::string filename = ExtractFilename( path, delimiter );
    std::string::size_type n = filename.find_last_of( '.' );
    LOG( INFO ) << "filename is " << filename << "  n is " << n;

    if (n != std::string::npos)
        return filename.substr( n );
    return std::string();
}

