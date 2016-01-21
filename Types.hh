///
/// @file   Types.hh
///
/// @date   20 Dec 2014
/// @author Don McNeill dmcneill@me.com
///
#ifndef TYPES_HH_
#define TYPES_HH_

//  Includes
#include <list>
#include <map>
#include <queue>
#include <string>
#include <vector>

//  Namespace
namespace isp {

///
/// @brief  Enumeration for the TR-069 client-side errors
///
typedef enum
{
    ISP_HELP_ARGUMENT = 1,
    ISP_NO_ERROR = 0,
    ISP_INVALID_ARGUMENT = -1,
    ISP_ERROR            = -2,

} tClientErrors;

///
/// @brief  Template type definitions to support TR-069 messages.
///
typedef std::vector<std::string>            tStringVector;
typedef std::map<std::string,std::string>   tStringMap;
} // namespace
#endif
