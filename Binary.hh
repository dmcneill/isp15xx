
///
/// @file   Binary.hh
///
/// @date   20 Dec 2014
/// @author Don McNeill dmcneill@me.com
///
#ifndef BINARY_HH
#define BINARY_HH
#include <string>
#include <fstream>

// Namespace
namespace isp {


class Binary
{
public:
    ///
    /// @brief      Binary file constructor.
    ///
    /// @param[in]  filename    The input filename to open.
    ///
    /// @param[in]  pMemory     The output memory block to write to.
    ///
    /// @param[in]  memSize     The size of the output block in bytes.
    ///
    Binary(std::string& filename, uint8_t * pMemory, size_t memSize);

    ///
    /// @brief      Binary destructor.
    ///
    virtual ~Binary();

    ///
    /// @brief      Parse the Binary file..
    ///
    /// @param[in]  isCheck     Boolean flag for examining the checksum.
    ///
    /// @param[in]  isDebug     Boolean flag for verbosity.
    ///
    /// @return     Boolean true on success and false on error.
    ///
    bool parse(bool isCheck, bool isDebug);

    ///
    /// @brief      See if the binary file needs to be written.
    ///
    /// @return     Boolean true on dirty (needs write) and false if
    ///             no write needed.
    ///
    bool isDirty() { return m_isDirty; }

    ///
    /// @brief      Read the binary file into memory.
    ///
    /// @return     Boolean true on sucess and false on error.
    ///
    bool read();

    ///
    /// @brief      Write the memory block to the binary file.
    ///
    /// @return     Boolean true on sucess and false on error.
    ///
    bool write();

    ///
    /// @brief      Get the start address for the binary file.
    ///
    /// @details    Get the start address for the binary file so
    ///             that the starting block and size can be calculated.
    ///
    /// @return     The start address as an unsigned integer.
    ///
    uint32_t getStartAddress() { return m_StartAddress; }

    ///
    /// @brief      Get the end address for the binary file.
    ///
    /// @details    Get the end address for the binary file so
    ///             that the ending block and size can be calculated.
    ///
    /// @return     The end address as an unsigned integer.
    ///
    uint32_t getEndAddress() { return m_EndAddress; }

private:
    ///
    /// @brief      Default Binary constructor
    ///
    /// @details    Force the use of the explicit constructor by not
    ///             allowing the default constructor to exist.
    ///
    Binary() = delete;

    ///
    /// @brief      Copy constructor
    ///
    /// @details    Make the class non-copyable.
    ///
    /// @param[in]  binary      Reference to the Binary object
    ///                         to be copied.
    ///
    Binary(const Binary& binary) = delete;

    ///
    /// @brief      Assignment operator
    ///
    /// @details    Make the class non-assignable.
    ///
    /// @param[in]  binary      Reference to the Binary object
    ///                         to be copied.
    ///
    Binary& operator = (const Binary& binary) = delete;

    ///
    /// @brief      Calculate a checksum at the starting address for a
    ///             given number of 32-bit integers.
    ///
    /// @param[in]  p2Address   The 32-bit address to start from.
    ///
    /// @param[in]  size        The number of 32-bit integers to check.
    ///
    /// @return     The checksum 32-bit value.
    ///
    uint32_t calculateChecksum(uint32_t * pAddress, size_t size);

    // Data members
    std::string     m_filename;
    size_t          m_Size;
    uint8_t *       m_pBuffer;
    bool            m_isDirty;
    uint8_t *       m_pMemory;
    uint32_t        m_StartAddress;
    uint32_t        m_EndAddress;
    std::ifstream   m_ifs;
    std::ofstream   m_ofs;
}; // class

} // namespace

#endif
