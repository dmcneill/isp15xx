///
/// @file   ISP.hh
///
/// @date   28 Dec 2014
/// @author Don McNeill dmcneill@me.com
///
#ifndef ISP_H
#define ISP_H

// Includes
#include <stdint.h>
#include <string.h>
#include "Serial.hh"


// Namespace
namespace isp {

///
/// @brief      ISP class for the ISP client.
///
/// @details    This class contains the support methods necessary to implement
///             the client command / response functions.
///
class ISP
{
public:
    typedef enum {
        ERR_ISP_TIMEOUT = -1,
        ERR_ISP_NO_ERROR = 0,
        ERR_ISP_INVALID_COMMAND,
        ERR_ISP_SRC_ADDR_ERROR,
        ERR_ISP_DST_ADDR_ERROR,
        ERR_ISP_SRC_ADDR_NOT_MAPPED,
        ERR_ISP_DST_ADDR_NOT_MAPPED,
        ERR_ISP_COUNT_ERROR,
        ERR_ISP_INVALID_SECTOR,
        ERR_ISP_SECTOR_NOT_BLANK,
        ERR_ISP_SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION,
        ERR_ISP_COMPARE_ERROR,
        ERR_ISP_BUSY,                       // Flash interface is busy
        ERR_ISP_PARAM_ERROR,                // Insufficient number of parameters
        ERR_ISP_ADDR_ERROR,                 // Address not on word boundary
        ERR_ISP_ADDR_NOT_MAPPED,
        ERR_ISP_CMD_LOCKED,                 // Command is locked
        ERR_ISP_INVALID_CODE,               // Unlock code is invalid
        ERR_ISP_INVALID_BAUD_RATE,
        ERR_ISP_INVALID_STOP_BIT,
        ERR_ISP_CODE_READ_PROTECTION_ENABLED,
        ERR_ISP_INVALID_FLASH_UNIT,         // reserved
        ERR_ISP_USER_CODE_CHECKSUM,         // reserved
        ERR_ISP_SETTING_ACTIVE_PARTITION,   // reserved
        ERR_ISP_IRC_NO_POWER,
        ERR_ISP_FLASH_NO_POWER,
        ERR_ISP_EEPROM_NO_POWER,
        ERR_ISP_EEPROM_NO_CLOCK,
        ERR_ISP_FLASH_NO_CLOCK,
        ERR_ISP_REINVOKE_ISP_CONFIG
    } Error;

    static const unsigned MINIMAL_TIMEOUT = 10;
    static const unsigned SHORT_TIMEOUT   = 20;
    static const unsigned MEDIUM_TIMEOUT  = 40;
    static const unsigned LONG_TIMEOUT    = 80;

    ///
    /// @brief      Explicit constructor for the ISP class.
    ///
    /// @details    Implementation of the ISP constructor.
    ///
    /// @param[in]  serial
    ///             Reference to the serial class to use.
    ///
    /// @param[in]  isActiveLowReset
    ///             The boolean flag for reset polarity.
    ///
    /// @param[in]  isVerbose
    ///             The boolean flag for the debug verbosity.
    ///
    ISP( isp::Serial&   serial,
         bool           isActiveLowReset = true,
         bool           isVerbose = false );

    ///
    /// @brief      Default destructor for the ISP class.
    ///
    ~ISP () {}

    ///
    /// @brief      Enter ISP mode on the target.
    ///
    void programMode();

    ///
    /// @brief      Enter Application mode on the target.
    ///
    void applicationMode();

    ///
    /// @brief      Synchronize the serial port to the target.
    ///
    /// @param[in]  isVerbose
    ///             The flag for the verbosity level.
    ///
    /// @return     The error code for the operation where zero is success and
    ///             any other value is an error.
    ///
    Error synchronize( bool isVerbose = false );


    ///
    /// @brief      Set the baud rate and number of stop bits
    ///             for the target.
    ///
    /// @param[in]  baud
    ///             The baud rate to be set.
    ///
    /// @param[in]  stopBits
    ///             The number of stop bits to use. Default is one.
    ///
    /// @param[in]  timeoutInMS
    ///             The timeout value in milliseconds for the reply.
    ///
    /// @param[in]  isVerbose
    ///             The flag for the verbosity level.
    ///
    Error setBaudRate( unsigned baud,
                       unsigned stopBits = 1,
                       unsigned timeoutInMS = SHORT_TIMEOUT,
                       bool isVerbose = false );

    ///
    /// @brief      Query the chip identifier for the target.
    ///
    /// @param[out] chipId
    ///             The identifier for the target chip to be filled in.
    ///
    /// @param[in]  timeoutInMS
    ///             The timeout value in milliseconds for the reply.
    ///
    /// @param[in]  isVerbose
    ///             The flag for the verbosity level.
    ///
    /// @return     The error code for the operation where zero is success and
    ///             any other value is an error.
    ///
    Error queryId( uint32_t& chipId,
                   unsigned timeoutInMS = MINIMAL_TIMEOUT,
                   bool isVerbose = false );

    ///
    /// @brief      Get the unique identifier for the target chip.
    ///
    /// @param[out] vec
    ///             A reference to the string vector to use for output.
    ///
    /// @param[in]  timeoutInMS
    ///             The timeout value in milliseconds for the reply.
    ///
    /// @param[in]  isVerbose
    ///             The flag for the verbosity level.
    ///
    /// @return     The error code for the operation where zero is success and
    ///             any other value is an error
    ///
    Error queryUID( std::vector<std::string>& vec,
                    unsigned timeoutInMS = MINIMAL_TIMEOUT,
                    bool isVerbose = false );

    ///
    /// @brief      Get the bootloader version for the target chip.
    ///
    /// @param[out] vec
    ///             A reference to the string vector to use for output.
    ///
    /// @param[in]  timeoutInMS
    ///             The timeout value in milliseconds for the reply.
    ///
    /// @param[in]  isVerbose
    ///             The flag for the verbosity level.
    ///
    /// @return     The error code for the operation where zero is success and
    ///             any other value is an error
    ///
    Error queryBootloaderVersion( std::vector<std::string>& vec,
                                  unsigned timeoutInMS = MINIMAL_TIMEOUT,
                                  bool isVerbose = false );

    ///
    /// @brief      Read the CRC checksum starting at the given address for the
    ///             given number of bytes.
    ///
    /// @param[in]  address
    ///             The address to start the CRC calculation from.
    ///
    /// @param[in]  size
    ///             The number of bytes to checksum.  Must be a multiple of 4.
    ///
    /// @param[out] crc
    ///             The resultant CRC.
    ///
    /// @param[in]  timeoutInMS
    ///             The timeout value in milliseconds for the reply.
    ///
    /// @param[in]  isVerbose
    ///             The flag for the verbosity level.
    ///
    /// @return     The error code for the operation where zero is success and
    ///             any other value is an error
    ///
    Error queryCRC( uint32_t address,
                    size_t size,
                    uint32_t& crc,
                    unsigned timeoutInMS = SHORT_TIMEOUT,
                    bool isVerbose = false );

    ///
    /// @brief      Unlock flash for the target chip.
    ///
    /// @param[in]  timeoutInMS
    ///             The timeout value in milliseconds for the reply.
    ///
    /// @param[in]  isVerbose
    ///             The flag for the verbosity level.
    ///
    /// @return     The error code for the operation where zero is success and
    ///             any other value is an error
    ///
    Error unlockFlash( unsigned timeoutInMS = MEDIUM_TIMEOUT,
                       bool isVerbose = false );

    ///
    /// @brief      Prepare flash sectors for write operations.
    ///
    /// @param[in]  start
    ///             The starting sector number to prepare.
    ///
    /// @param[in]  end
    ///             The ending sector number to prepare.
    ///
    /// @param[in]  timeoutInMS
    ///             The timeout value in milliseconds for the reply.
    ///
    /// @param[in]  isVerbose
    ///             The flag for the verbosity level.
    ///
    /// @return     The error code for the operation where zero is success and
    ///             any other value is an error
    ///
    Error prepareSectors( unsigned start,
                          unsigned end,
                          unsigned timeoutInMS = MEDIUM_TIMEOUT,
                          bool isVerbose = false );

    ///
    /// @brief      Erase flash sectors.
    ///
    /// @param[in]  start
    ///             The starting sector number to erase.
    ///
    /// @param[in]  end
    ///             The ending sector number to erase.
    ///
    /// @param[in]  timeoutInMS
    ///             The timeout value in milliseconds for the reply.
    ///
    /// @param[in]  isVerbose
    ///             The flag for the verbosity level.
    ///
    /// @return     The error code for the operation where zero is success and
    ///             any other value is an error
    ///
    Error eraseSectors( unsigned start,
                        unsigned end,
                        unsigned timeoutInMS = LONG_TIMEOUT,
                        bool isVerbose = false );

    ///
    /// @brief      Blank check flash sectors.
    ///
    /// @param[in]  sector
    ///             The sector number to check.
    ///
    /// @param[out] sectorMap
    ///             Reference to the boolean sector map for blanking state.
    ///
    /// @param[in]  timeoutInMS
    ///             The timeout value in milliseconds for the reply.
    ///
    /// @param[in]  isVerbose
    ///             The flag for the verbosity level.
    ///
    /// @return     The error code for the operation where zero is success and
    ///             any other value is an error
    ///
    Error blankCheckSector( unsigned sector,
                            std::vector<bool>& sectorMap,
                            unsigned timeoutInMS = SHORT_TIMEOUT,
                            bool isVerbose = false );

    ///
    /// @brief      Read memory from the target device.
    ///
    /// @param[in]  address
    ///             The address at which to start reading from.
    ///
    /// @param[in]  size
    ///             The number of bytes to read.
    ///
    /// @param[out] vec
    ///             Reference to the uint8_t vector to use for output.
    ///
    /// @param[in]  timeoutInMS
    ///             The timeout value in milliseconds for the reply.
    ///
    /// @param[in]  isVerbose
    ///             The flag for the verbosity level.
    ///
    /// @return     The error code for the operation where zero is success and
    ///             any other value is an error
    ///
    Error readMemory( uint32_t address,
                      size_t size,
                      std::vector<uint8_t>& vec,
                      unsigned timeoutInMS = LONG_TIMEOUT,
                      bool isVerbose = false );

    ///
    /// @brief      Enable / disable command echoing from the target.
    ///
    /// @param[in]  enable
    ///             The flag for the echo enable (true) or disable (false).
    ///
    /// @param[in]  timeoutInMS
    ///             The timeout value in milliseconds for the reply.
    ///
    /// @param[in]  isVerbose
    ///             The flag for the verbosity level.
    ///
    /// @return     The error code for the operation where zero is success and
    ///             any other value is an error.
    ///
    Error echo( bool enable,
                unsigned timeoutInMS = MEDIUM_TIMEOUT,
                bool isVerbose = false );

    ///
    /// @brief      Copy RAM to flash memory (program flash)
    ///
    /// @param[in]  flash
    ///             The flash address at which to start copying to.
    ///
    /// @param[in]  address
    ///             The RAM address at which to start copying from.
    ///
    /// @param[in]  size
    ///             The number of bytes to read.
    ///
    /// @param[in]  timeoutInMS
    ///             The timeout value in milliseconds for the reply.
    ///
    /// @param[in]  isVerbose
    ///             The flag for the verbosity level.
    ///
    Error copyToFlash( uint32_t flash,
                       uint32_t address,
                       size_t size,
                       unsigned timeoutInMS = LONG_TIMEOUT,
                       bool isVerbose = false );

    ///
    /// @brief      Execute starting at a given address.
    ///
    /// @param[in]  address
    ///             The address at which to start writing to.
    ///
    /// @param[in]  timeoutInMS
    ///             The timeout value in milliseconds for the reply.
    ///
    /// @param[in]  isVerbose
    ///             The flag for the verbosity level.
    ///
    /// @return     The error code for the operation where zero is success and
    ///             any other value is an error.
    ///
    Error execute( uint32_t address,
                   unsigned timeoutInMS = MEDIUM_TIMEOUT,
                   bool isVerbose = false );

    ///
    /// @brief      Write memory to the target device.
    ///
    /// @param[in]  address
    ///             The address at which to start writing to.
    ///
    /// @param[in]  size
    ///             The number of bytes to write.
    ///
    /// @param[out] vec
    ///             Reference to the uint8_t vector to use for input.
    ///
    /// @param[in]  timeoutInMS
    ///             The timeout value in milliseconds for the reply.
    ///
    /// @param[in]  isVerbose
    ///             The flag for the verbosity level.
    ///
    /// @return     The error code for the operation where zero is success and
    ///             any other value is an error
    ///
    Error writeMemory( uint32_t address,
                       size_t size,
                       std::vector<uint8_t>& vec,
                       unsigned timeoutInMS = MEDIUM_TIMEOUT,
                       bool isVerbose = false );

protected:
    ///
    /// @brief      Send a command to the serial interface and get a response.
    ///
    /// @details    Send a command to the serial interface, wait to get the
    ///             appropriate response or a timeout.
    ///
    /// @param[in]  command
    ///             The command string to be sent.
    ///
    /// @param[out] response
    ///             The string to fill in for the response.
    ///
    /// @param[in]  timeoutInMS
    ///             The timeout value in milliseconds for the reply.
    ///
    /// @param[in]  isVerbose
    ///             The flag for the verbosity level.
    ///
    /// @param[in]  retryCount
    ///             The number of attempts to make before aborting.
    ///
    /// @retval     > 0:    The number of characters in the response.
    ///             -1:     Error encountered.
    ///             -2:     Timeout
    ///
    ssize_t send( const std::string& command,
                  std::string& response,
                  unsigned timeoutInMS,
                  bool isVerbose = false,
                  int retryCount = 3 );

    ///
    /// @brief      Send a command to the serial interface and get a response.
    ///
    /// @details    Send a command to the serial interface, wait to get the
    ///             appropriate response or a timeout.
    ///
    /// @param[in]  command
    ///             The command string to be sent.
    ///
    /// @param[out] response
    ///             The string to fill in for the response.
    ///
    /// @param[out] testResponse
    ///             The string to use to test with.
    ///
    /// @param[in]  timeoutInMS
    ///             The timeout value in milliseconds for the reply.
    ///
    /// @param[in]  isVerbose
    ///             The flag for the verbosity level.
    ///
    /// @param[in]  retryCount
    ///             The number of attempts to make before aborting.
    ///
    /// @retval     > 0:    The number of characters in the response.
    ///             -1:     Error encountered.
    ///             -2:     Timeout
    ///
    ssize_t send( const std::string& command,
                  std::string& response,
                  std::string& testResponse,
                  unsigned timeoutInMS,
                  bool isVerbose = false,
                  int retryCount = 3 );

    ///
    /// @brief      Send a command to the serial interface and get a response.
    ///
    /// @details    Send a command to the serial interface, wait to get the
    ///             appropriate response or a timeout.
    ///
    /// @param[in]  command
    ///             The command string to be sent.
    ///
    /// @param[out] response
    ///             The vector to fill in for the response.
    ///
    /// @param[out] testResponse
    ///             The string to use to test with.
    ///
    /// @param[in]  timeoutInMS
    ///             The timeout value in milliseconds for the reply.
    ///
    /// @param[in]  isVerbose
    ///             The flag for the verbosity level.
    ///
    /// @param[in]  retryCount
    ///             The number of attempts to make before aborting.
    ///
    /// @retval     > 0:    The number of characters in the response.
    ///             -1:     Error encountered.
    ///             -2:     Timeout
    ///
    ssize_t send( const std::string& command,
                  std::vector<uint8_t>& response,
                  std::string& testResponse,
                  unsigned timeoutInMS,
                  bool isVerbose = false,
                  int retryCount = 3 );

    ///
    /// @brief      Send a set of bytes from a vector to the serial interface
    ///             and get a string response.
    ///
    /// @details    Send a byte vector to the serial interface, wait to get the
    ///             appropriate response or a timeout.
    ///
    /// @param[in]  bytes
    ///             The reference to the byte vector to be sent.
    ///
    /// @param[in]  isVerbose
    ///             The flag for the verbosity level.
    ///
    /// @retval     > 0:    The number of characters in the response.
    ///             -1:     Error encountered.
    ///             -2:     Timeout
    ///
    ssize_t send( std::vector<uint8_t>& bytes,
                  bool isVerbose = false );

private:
    ///
    /// @brief      Default constructor.
    ///
    /// @details    Force the use of the explicit constructor by not
    ///             allowing the default constructor to exist.
    ///
    ISP() = delete;

    ///
    /// @brief      ISP copy constructor (non-copyable)
    ///
    /// @details    Make the class non-copyable.
    ///
    /// @param[in]  ref
    ///             Reference to an ISP instance to copy from.
    ///
    ISP( const ISP& ref ) = delete;

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
    ISP& operator = ( const ISP& ref ) = delete;

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
    isp::Serial&    mSerial;
    bool            mIsActiveLowReset;
    bool            mIsVerbose;
    std::string     mChipId;
    bool            mIsEcho;
};  // class

} // namespace
#endif

