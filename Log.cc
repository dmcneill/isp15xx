///
/// @file   Log.cc
///
/// @date   20 Dec 2014
/// @author Don McNeill dmcneill@me.com
///

//  Includes
#include <iostream>
#include "Log.hh"


//  Static variables
static  tLogLevel   gLogLevel = INFO;


//
//  @brief      Log default constructor.
//
isp::Log::Log()
      : m_stream(&std::cout),
        m_messageLevel(WARNING)
{}


//
//  @brief      Log destructor.
//
isp::Log::~Log()
{
    *m_stream << std::endl;
}


//
//  @brief      Get the reporting log level.
//
tLogLevel& isp::Log::ReportingLevel()
{
    return gLogLevel;
}


//
//  @brief      Convert the log level to a string.
//
const std::string isp::Log::ToString(tLogLevel level)
{

    if (level >= TRACE &&
        level <= ERROR)
    {
        static const char * levelTable[] =
        {
            "[TRACE]",
            "[INFO]",
            "[WARNING]",
            "[ERROR]"
        };
        return levelTable[ level ];
    }
    else
        return "UNKNOWN";
}


//
//  @brief      Get the current time string.
//
const std::string isp::Log::NowTime()
{
    time_t     now = time(0);
    struct tm  tm;
    char       buf[80];

    tm = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tm );
    return buf;
}


//
//  @brief      Get the output stream instance.
//
std::ostream& isp::Log::Get(tLogLevel level)
{
    *m_stream << NowTime();
    *m_stream << " "  << ToString(level) << ": ";
    *m_stream << '\t';
    m_messageLevel = level;
    return *m_stream;
}

