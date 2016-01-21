///
/// @file   Alarm.hh
///
/// @date   20 Dec 2014
/// @author Don McNeill dmcneill@me.com
///
#ifndef ALARM_HH_
#define ALARM_HH_

//  Includes
#include "Signal.hh"


//  Namespace
namespace isp {

///
/// @brief      The alarm class for the ISP client.
///
/// @details    This class contains the support methods necessary to implement
///             the ISP communication signal handler for SIGALRM.
///
class Alarm : public Signal
{
public:

    ///
    /// @brief      Set a callback on the SIGALRM.
    ///
    /// @details    Setup a callback that is invoked on a UNIX signal event.
    ///
    /// @param[in]  seconds     The number of seconds for the alarm
    ///
    /// @param[in]  callback    The function for the callback; must be
    ///                         of the type tSigFunc.
    ///
    Alarm(int seconds, tSigFunc callback);

    ///
    /// @brief      Set a callback on the SIGALRM.
    ///
    /// @details    Setup a callback that is invoked on a UNIX signal event.
    ///
    /// @param[in]  callback    The function for the callback; must be
    ///                         of the type tSigFunc.
    ///
    /// @param[in]  timeInMS    The number of milliseconds for the alarm
    ///
    Alarm(tSigFunc callback, int timeInMS);

    ///
    /// @brief      Alarm destructor
    ///
    /// @details    Destructor to restore the signal to the default
    ///             state.
    ///
    virtual ~Alarm();

private:
    ///
    /// @brief      Default constructor.
    ///
    /// @details    Force the use of the explicit constructor by not
    ///             allowing the default constructor to exist.
    ///
    Alarm() = delete;

    ///
    /// @brief      Alarm copy constructor (non-copyable)
    ///
    /// @details    Make the class non-copyable.
    ///
    /// @param[in]  alarm
    ///             Reference to an alarm instance to copy from.
    ///
    Alarm(const Alarm& alarm) = delete;

    ///
    /// @brief      Alarm assignment operator (not-assignable)
    ///
    /// @details    Make the class non-assignable.
    ///
    /// @param[in]  alarm
    ///             Reference to an alarm instance to copy from.
    ///
    /// @return     New instance for the left-hand side of the expression.
    ///
    Alarm& operator = (const Alarm& alarm) = delete;

    ///  Data members
    int m_Seconds;
    int m_Milliseconds;
};  // class

} // namespace
#endif
