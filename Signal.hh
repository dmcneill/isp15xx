///
/// @file   Signal.hh
///
/// @date   20 Dec 2014
/// @author Don McNeill dmcneill@me.com
///
#ifndef SIGNAL_HH_
#define SIGNAL_HH_

//  Includes
#include <signal.h>
#include "Types.hh"


//  Namespace
namespace isp {

///
/// @brief      The POSIX signals class for the TR-069 client.
///
/// @details    This class contains the support methods necessary to implement
///             the ISP client UNIX signal handlers.
///
class Signal
{
public:
    /// The callback function signature
    typedef void (* tSigFunc)(int);

    ///
    /// @brief      Set a callback on a UNIX signal.
    ///
    /// @details    Setup a callback that is invoked on a UNIX signal event.
    ///
    /// @param[in]  number      The signal number to use.
    ///
    /// @param[in]  callback    The function for the callback; must be
    ///                         of the type tSigFunc.
    ///
    Signal(int number, tSigFunc callback);

    ///
    /// @brief      Set the default to handle the UNIX signal.
    ///
    /// @details    Setup the default to handle a UNIX signal event.
    ///
    /// @param[in]  number      The signal number to use.
    ///
    Signal(int number);

    ///
    /// @brief      Signal destructor
    ///
    /// @details    Destructor to restore the signal to the default
    ///             state.
    ///
    virtual ~Signal();

private:
    ///
    /// @brief      Default Signal constructor
    ///
    /// @details    Force the use of the explicit constructor by not
    ///             allowing the default constructor to exist.
    ///
    Signal() = delete;

    ///
    /// @brief      Copy constructor
    ///
    /// @details    Make the class non-copyable.
    ///
    /// @param[in]  signal      Reference to the Signal object
    ///                         to be copied.
    ///
    Signal(const Signal& signal) = delete;

    ///
    /// @brief      Assignment operator
    ///
    /// @details    Make the class non-assignable.
    ///
    /// @param[in]  signal      Reference to the Signal object
    ///                         to be copied.
    ///
    Signal& operator = (const Signal& signal) = delete;

    //  Data members
    int m_number;
    void (* m_Func )(int);
    struct sigaction m_action;
};  // class

} // namespace
#endif
