///
/// @file   Client.hh
///
/// @date   30 Dec 2014
/// @author Don McNeill dmcneill@me.com
///
#ifndef CLIENT_H
#define CLIENT_H

//  Includes
#include <stdint.h>
#include <string.h>
#include "ISP.hh"

// Definitions
#define FLASH_SECTOR_SIZE     (4096)
#define RAM_SECTOR_SIZE       (1024)
#define RAM_PROGRAM_ADDRESS   (0x02001000)


//  Namespace
namespace isp {

///
/// @brief      Erase the chip.
///
/// @param[in]  device
///             The string for the serial device to open.
///
/// @param[in]  syncRetries
///             The number of retries to establish synchronization.
///
/// @return     The error code for the operation where zero is success and
///             any other value is an error.
///
extern ISP::Error eraseClient( const char * device, unsigned syncRetries );

///
/// @brief      Program the target through the ISP client interface.
///
/// @param[in]  device
///             The string for the serial device to open.
///
/// @param[in]  syncRetries
///             The number of retries to establish synchronization.
///
/// @return     The error code for the operation where zero is success and
///             any other value is an error.
///
extern ISP::Error programClient( const char * device, unsigned syncRetries );

///
/// @brief      Examine target memory through the ISP client interface.
///
/// @param[in]  device
///             The string for the serial device to open.
///
/// @param[in]  syncRetries
///             The number of retries to establish synchronization.
///
/// @return     The error code for the operation where zero is success and
///             any other value is an error.
///
extern ISP::Error examineClient( const char * device, unsigned syncRetries );

} // namespace
#endif

