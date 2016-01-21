///
/// @file   Log.hh
///
/// @date   20 Dec 2014
/// @author Don McNeill dmcneill@me.com
#ifndef LOG_HH_
#define LOG_HH_

//  Includes
#include <ostream>
#include <sstream>

//  Macros
#ifdef DEBUG
#define LOG(level)  if(level >= isp::Log::ReportingLevel()) \
isp::Log().Get(level) << __FILE__ << ":" << std::dec << __LINE__ << " " << __FUNCTION__ << "(): "
#else
#define LOG(level)  if(level >= isp::Log::ReportingLevel()) \
isp::Log().Get(level)
#endif

//  Type definitions
typedef enum
{
   TRACE,
   INFO,
   WARNING,
   ERROR

} tLogLevel;


//  Namespace
namespace isp {

///
/// @brief      Log class for console output.
///
/// @details    This class defines the methods for the implementation
///             of a console output logger object.
///
class Log
{
public:

    ///
    /// @brief      Log default constructor.
    ///
    /// @details    Instantiate the Log class for console output messages.
    ///
    Log();

    ///
    /// @brief      Log destructor.
    ///
    /// @details    Teardown the log instance.
    ///
    virtual ~Log();

    ///
    /// @brief      Get the output stream instance.
    ///
    /// @details    Retrieve the output stream instance that is to be used
    ///             for logging.
    ///
    /// @param[in]  level           The default logging level to use for this
    ///                             Log instance.
    ///
    /// @return     The reference to the standard output stream in use.
    ///
    std::ostream& Get(tLogLevel level = INFO);

    ///
    /// @brief      Get the reporting log level.
    ///
    /// @details    Retrieve the reporting log level for comparison use.
    ///
    /// @return     The current logging level instance in use by the system.
    ///
    static tLogLevel& ReportingLevel();

private:
    ///
    /// @brief      Log copy constructor.
    ///
    /// @details    Make the class non-copyable.
    ///
    /// @param[in]  log             Reference to the log instance to copy.
    ///
    Log(const Log& log) = delete;

    ///
    /// @brief      Log assignment operator.
    ///
    /// @details    Make the class non-assignable.
    ///
    /// @param[in]  log             Reference to the log instance to copy from.
    ///
    Log& operator = (const Log&) = delete;

    ///
    /// @brief      Get the current time string.
    ///
    /// @details    Get the current time string for use in the log messages.
    ///
    /// @return     A string object containing the formatted data and time.
    ///
    const std::string NowTime();

    ///
    /// @brief      Convert the log level to a string.
    ///
    /// @details    Convert the log level to a string representation for
    ///             the log message output.
    ///
    /// @return     A string object containing the formatted level string.
    ///
    const std::string ToString(tLogLevel level);

    //  Data members
    std::ostream *  m_stream;
    tLogLevel       m_messageLevel;
};  // class

} // namespace
#endif
