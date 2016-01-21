///
/// @file   CmdLine.cc
///
/// @date   08 Sep 2015
/// @author Don McNeill dmcneill@me.com
///

//  Includes
#include <algorithm>
#include <vector>
#include <string>
#include "CmdLine.hh"


///
/// @brief      CmdLine explicit constructor.
///
isp::CmdLine::CmdLine(int argc, char * const * argv)
{
    for (int ii = 0; ii < argc; ++ii)
    {
        std::string tmpStr = argv[ii];
        list.push_back(tmpStr);
    }
}


///
/// @brief      CmdLine destructor.
///
isp::CmdLine::~CmdLine()
{
    list.clear();
}


///
/// @brief      Determine if a specific option exists.
///
bool isp::CmdLine::find(const char * input, size_t& index)
{
    for (unsigned ii = 0; ii < list.size(); ++ii)
    {
        if (list[ii] == input)
        {
            index = ii;
            return true;
        }
    }
    return false;
}


///
/// @brief      Get the nth string from the command line option list.
///
bool isp::CmdLine::get(size_t index, std::string& option)
{
    if (index < list.size())
    {
        option = list[index];
        return true;
    }
    return false;
}
