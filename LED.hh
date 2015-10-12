///
/// @file   LED.hh
///
/// @date   06 Sep 2015
/// @author Don McNeill dmcneill@me.com
///
#ifndef LED_H
#define LED_H

// Includes
#include <stdint.h>
#include <string.h>


// Namespace
namespace isp {

///
/// @brief      LED class
///
/// @details    This class contains the support methods necessary to toggle
///             an LED.
///
class LED
{
public:

    ///
    /// @brief      Default constructor for the ISP class.
    ///
    /// @details    Implementation of the LED constructor.
    ///
    LED ();

    ///
    /// @brief      Default destructor for the LED class.
    ///
    ~LED ();

    ///
    /// @brief      Set the LED to a given state.
    ///
    /// @param[in]  value
    ///             The boolean value to use on the LED.
    ///
    void Set( bool value );

    ///
    /// @brief      Cycle the LED.
    ///
    void cycle();

private:
    ///
    /// @brief      ISP copy constructor (non-copyable)
    ///
    /// @details    Make the class non-copyable.
    ///
    /// @param[in]  ref
    ///             Reference to an ISP instance to copy from.
    ///
    LED( const LED& ref ) = delete;

    ///
    /// @brief      ISP assignment operator (not-assignable)
    ///
    /// @details    Make the class non-assignable.
    ///
    /// @param[in]  ref
    ///             Reference to an ISP instance to copy from.
    ///
    /// @return     New instance for the left-hand side of the expression.
    ///
    LED& operator = ( const LED& ref ) = delete;

    ///
    /// @brief      Open up a HW signal for access.
    ///
    /// @param[in]  signal
    ///             The signal name string to use to access the signal
    ///             through GPIO.
    ///
    /// @return     The signal's file descriptor on success or neg on on error.
    ///
    int hwSignalOpen( const char * signal );

    ///
    /// @brief      Close down a HW signal from access.
    ///
    /// @param[in]  fd
    ///             The signal's file descriptor.
    ///
    /// @param[in]  signal
    ///             The signal name string to use to access the signal
    ///             through GPIO.
    ///
    void hwSignalClose( int fd,
                        const char * signal );

    ///
    /// @brief      Set a signal to a given state.
    ///
    /// @param[in]  fd
    ///             The signal's file descriptor.
    ///
    /// @param[in]  signal
    ///             The signal name string to use to access the signal
    ///             through GPIO.
    ///
    /// @param[in]  value
    ///             The boolean value to use on the signal.
    ///
    void hwSignalSet( int fd,
                      const char * signal,
                      bool value );

    // Data members
    int             mFileDes;
    int             mCycle;
    int             mCounter;
};  // class

} // namespace
#endif

