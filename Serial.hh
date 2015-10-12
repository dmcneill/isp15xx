///
/// @file   Serial.hh
///
/// @date   20 Dec 2014
/// @author Don McNeill dmcneill@me.com
///
#ifndef SERIAL_H
#define SERIAL_H

// Includes
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include "Signal.hh"

// Namespace
namespace isp {

///
/// @brief      Serial class for the ISP client.
///
/// @details    This class contains the support methods necessary to implement
///             a serial port interface for ISP client.
///
class Serial
{
public:
    static const size_t RxBufferSize  = 2048;

    ///
    /// @brief      Explicit constructor for the Serial class.
    ///
    /// @details    Create a channel to the serial device.
    ///
    /// @param[in]  p2Device
    ///             A character string for the device name to open.
    ///
    /// @param[in]  controlFlags
    ///             The tty control flags for baud rate, data and flow control.
    ///
    /// @param[in]  inputFlags
    ///             Optional parity checking input flags.
    ///
    Serial (const char * p2Device,
            int controlFlags = B115200,
            int inputFlags = 0 );

    ///
    /// @brief      Default destructor for the Serial class.
    ///
    ~Serial ();

    ///
    /// @brief      Determine if the serial port is open or not.
    ///
    /// @return     Boolean true if open, false otherwise.
    ///
    bool isOpen() { return mIsOpen; }

    ///
    /// @brief      Read an input string from the Serial port.
    ///
    /// @param[out] str
    ///             A reference to the string to write to.
    ///
    /// @param[in]  timeoutInMS
    ///             The time in milliseconds for the reply timeout.
    ///
    /// @param[out] readTime
    ///             The time in milliseconds when reading starts.
    ///
    /// @param[in]  isVerbose
    ///             Boolean flag for the debug verbosity.
    ///
    /// @return     The number of bytes read into the buffer.  Set to a
    ///             negative number on error.
    ///
    ssize_t read( std::string& str,
                  unsigned timeoutInMS,
                  unsigned& readTime,
                  bool isVerbose = false );

    ///
    /// @brief      Read an input vector of bytes from the Serial port.
    ///
    /// @param[out] bVector
    ///             A reference to the byte vector to write to.
    ///
    /// @param[in]  timeoutInMS
    ///             The time in milliseconds for the reply timeout.
    ///
    /// @param[out] readTime
    ///             The time in milliseconds when reading starts.
    ///
    /// @param[in]  isVerbose
    ///             Boolean flag for the debug verbosity.
    ///
    /// @return     The number of bytes read into the vector.  Set to a
    ///             negative number on error.
    ///
    ssize_t read( std::vector<uint8_t>& bVector,
                  unsigned timeoutInMS,
                  unsigned& readTime,
                  bool isVerbose = false );

    ///
    /// @brief      Write an output buffer to the Serial port.
    ///
    /// @param[out] pBuffer
    ///             A pointer to the user buffer to read from.
    ///
    /// @param[in]  size
    ///             The number of bytes to write.
    ///
    /// @return     The number of bytes written. Set to a negative number on error.
    ///
    ssize_t write (const char * pBuffer, size_t size);

    ///
    /// @brief      Write an output string to the Serial port.
    ///
    /// @param[in]  str
    ///             A reference to the string to read from.
    ///
    /// @return     The number of bytes read from the string.  Set to a
    ///             negative number on error.
    ///
    ssize_t write( const std::string& str );

    ///
    /// @brief      Write an output byte vector to the Serial port.
    ///
    /// @param[in]  vec
    ///             A reference to the byte-vector to read from.
    ///
    /// @return     The number of bytes read from the vector.  Set to a
    ///             negative number on error.
    ///
    ssize_t write( const std::vector<uint8_t>& vec );

    ///
    /// @brief      Get the current error state.
    ///
    /// @return     The error number as an integer.
    ///
    int getError () { return mError; }

private:
    ///
    /// @brief      Default constructor for the Serial class.
    ///
    /// @details    Force the use of the explicit constructor by not
    ///             allowing the default constructor to exist.
    ///
    Serial() = delete;

    ///
    /// @brief      Copy constructor
    ///
    /// @details    Make the class non-copyable.
    ///
    /// @param[in]  serial      Reference to the Serial object
    ///                         to be copied.
    ///
    Serial( const Serial& serial ) = delete;

    ///
    /// @brief      Assignment operator
    ///
    /// @details    Make the class non-assignable.
    ///
    /// @param[in]  serial      Reference to the Serial object
    ///                         to be copied.
    ///
    Serial& operator = ( const Serial& serial ) = delete;

    ///
    /// @brief      Get the file descriptor.
    ///
    /// @return     The file descriptor as an integer.
    ///
    int getFileDes () { return mFileDes; }


    ///
    /// @brief      Read an input buffer from the Serial port.
    ///
    /// @param[out] pBuffer
    ///             A pointer to the user buffer to write to.
    ///
    /// @param[in]  size
    ///             The maximum number of bytes available in the buffer.
    ///
    /// @param[in]  timeInMS
    ///             The timeout value in milliseconds for the read.
    ///
    /// @param[out] readTime
    ///             The time in milliseconds when reading starts.
    ///
    /// @return     The number of bytes read into the buffer.  Set to a
    ///             negative number on error.
    ///
    ssize_t read( char * pBuffer,
                  size_t size,
                  unsigned timeInMS,
                  unsigned& readTime );

    // Data Members
    int             mError;
    bool            mIsOpen;
    int             mFileDes;
    struct termios  mOldSettings;
    struct termios  mNewSettings;
};
}
#endif
