///
/// @file   iHex.hh
///
/// @date   21 Dec 2014
/// @author Don McNeill dmcneill@me.com
///
#ifndef IHEX_HPP
#define IHEX_HPP

//  Includes
#include <iostream>
#include <fstream>
#include <string>

//  Namespace
namespace isp {

///
/// @brief      Delta list class for the TR-069 client.
///
/// @details    This class contains the support methods necessary to implement
///             a delta-list scheduler for client threading support.
///
class iHex
{
public:
    ///
    /// @brief      Explicit constructor.
    ///
    /// @details    Explicit constructor for the iHex class.
    ///
    /// @param[in]  filename
    ///             The string for the filename; can be ether a filename in
    ///             the current directory or a full path.
    ///
    /// @param[in]  pMemory
    ///             The memory block to be filled in.
    ///
    /// @param[in]  size
    ///             The size of the memory block in bytes.
    ///
    iHex(const char * filename, uint8_t * pMemory, size_t size);

    ///
    /// @brief      Default destructor.
    ///
    /// @details    Default destructor for the iHex class.
    ///
    virtual ~iHex();

    ///
    /// @brief      Parse the file contents.
    ///
    /// @details    Parse the entire file and determine the range
    ///             for the addresses.
    ///
    /// @retval     true    Indicates success.
    /// @retval     false   Indicates an error was encountered.
    ///
    bool parse();

    ///
    /// @brief      Process the Intel Hex line.
    ///
    /// @details    Process each Intel Hex record.
    ///
    /// @retval     true    Indicates success.
    /// @retval     false   Indicates an error was encountered.
    ///
    bool process(std::string& line);

    ///
    /// @brief      Calculate 2's compliment checksum for the application.
    ///
    void doChecksum();

    ///
    /// @brief      Get the start address for the Intel Hex file.
    ///
    /// @details    Get the start address for the Intel hex file so
    ///             that the starting block and size can be calculated.
    ///
    /// @return     The start address as an unsigned integer.
    ///
    uint32_t getStartAddress() { return mStartAddress; }

    ///
    /// @brief      Get the end address for the Intel Hex file.
    ///
    /// @details    Get the end address for the Intel hex file so
    ///             that the ending block and size can be calculated.
    ///
    /// @return     The end address as an unsigned integer.
    ///
    uint32_t getEndAddress() { return mEndAddress; }

private:
    ///
    /// @brief      Default constructor.
    ///
    /// @details    Force the use of the explicit constructor by not
    ///             allowing the default constructor to exist.
    ///
    iHex() = delete;

    ///
    /// @brief      Copy constructor
    ///
    /// @details    Make the class non-copyable.
    ///
    /// @param[in]  ihex        Reference to the Intel Hex object
    ///                         to be copied.
    ///
    iHex(const iHex& ihex) = delete;

    ///
    /// @brief      Assignment operator
    ///
    /// @details    Make the class non-assignable.
    ///
    /// @param[in]  ihex        Reference to the Intel Hex object
    ///                         to be copied.
    ///
    iHex& operator = (const iHex& ihex) = delete;

    //  Data Members
    std::string     mFilename;
    uint32_t        mOffsetAddress;
    uint32_t        mStartAddress;
    uint32_t        mEndAddress;
    uint8_t *       mpMemory;
};
}
#endif
