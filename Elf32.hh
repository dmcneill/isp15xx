
///
/// @file   Elf32.hh
///
/// @date   20 Dec 2014
/// @author Don McNeill dmcneill@me.com
///
#ifndef ELF32_HH
#define ELF32_HH
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include "elf.h"

// Namespace
namespace isp {

typedef std::vector<uint8_t> ByteVec;

class Section
{
public:
    ///
    /// @brief      Section constructor.
    ///
    /// @param[in]  name        The section name.
    ///
    /// @param[in]  size        Size of the section in bytes.
    ///
    /// @param[in]  startAddr   Start address of the section on chip.
    ///
    /// @param[in]  alignment   The start address alignment.
    ///
    /// @param[in]  pMemory     Pointer to byte memory to copy data from.
    ///
    Section(const char * name,
            size_t size,
            uint32_t startAddress,
            uint32_t alignment,
            uint8_t * pMemory)
        : m_Name(name),
          m_StartAddress(startAddr),
          m_Alignment(alignment),
          m_Size(size)
    {
        m_Vector.resize(size);

        ::memcpy(m_Vector.data(), pMemory, size);
    }

    ///
    /// @brief      Section destructor.
    ///
    ///
    virtual ~Section() {}

    ///
    /// @brief      Get the section's name.
    ///
    /// @return     The constant character string for the name.
    ///
    const char * getName() { return m_Name.c_str(); }

    ///
    /// @brief      Get the section's size in bytes.
    ///
    /// @return     The size of the data for the section in bytes.
    ///
    size_t getSize() { return m_Size; }

    ///
    /// @brief      Get the start address for the section.
    ///
    /// @return     The start address as a 32-bit unsigned int.
    ///
    uint32_t getStartAddress() { return m_StartAddress; }

    ///
    /// @brief      Get the alignment for the section.
    ///
    /// @return     The byte-alignment setting for the section.
    ///
    uint32_t getAlignment() { return m_Alignment; }

    ///
    /// @brief      Get the data block for the section.
    ///
    /// @return     The constant pointer to the start of the section data.
    ///
    uint8_t const * getData() { return m_Vector.data(); }

private:
    //  Data members
    std::string m_Name;
    uint32_t    m_StartAddress;
    uint32_t    m_Alignment;
    size_t      m_Size;
    ByteVec     m_Vector;
    
};


typedef std::map<std::string, Section> SecMap;

class Elf32
{
public:
    ///
    /// @brief      Elf32 constructor.
    ///
    /// @param[in]  filename    The input filename to open.
    ///
    /// @param[in]  pMemory     The output memory block to write to.
    ///
    /// @param[in]  memSize     The size of the output block in bytes.
    ///
    Elf32(std::string& filename, uint8_t * pMemory, size_t memSize);

    ///
    /// @brief      Elf32 destructor.
    ///
    virtual ~Elf32();

    ///
    /// @brief      Parse the ELF32 header.
    ///
    /// @param[in]  isCheck     Boolean flag for examining the checksum.
    ///
    /// @param[in]  isDebug     Boolean flag for verbosity.
    ///
    /// @return     Boolean true on success and false on error.
    ///
    bool parse(bool isCheck, bool isDebug);

    ///
    /// @brief      See if the ELF file needs to be written.
    ///
    /// @return     Boolean true on dirty (needs write) and false if
    ///             no write needed.
    ///
    bool isDirty() { return m_isDirty; }

    ///
    /// @brief      Read the ELF32 file into memory.
    ///
    /// @return     Boolean true on sucess and false on error.
    ///
    bool read();

    ///
    /// @brief      Write the memory block to the ELF32 file.
    ///
    /// @return     Boolean true on sucess and false on error.
    ///
    bool write();

    ///
    /// @brief      Get the start address for the .text section.
    ///
    /// @details    Get the start address for the .text section so
    ///             that the starting block and size can be calculated.
    ///
    /// @return     The start address as an unsigned integer.
    ///
    uint32_t getStartAddress() { return m_StartAddress; }

    ///
    /// @brief      Get the end address for the .text section.
    ///
    /// @details    Get the end address for the .text section so
    ///             that the ending block and size can be calculated.
    ///
    /// @return     The end address as an unsigned integer.
    ///
    uint32_t getEndAddress() { return m_EndAddress; }

    ///
    /// @brief      Align an address value.
    ///
    /// @param[in]  address The address value to align.
    ///
    /// @param[in]  align   The number of bytes to align.
    ///
    /// @return     The newly aligned address.
    ///
    uint32_t alignAddress(uint32_t address, unsigned align);

    ///
    /// @brief      Convert integer value to string.
    ///
    /// @param[out] str     The string reference to use for output.
    ///
    /// @param[in]  type    The integer value to examine.
    ///
    /// @return     The reference to the output string.
    ///
    static std::string& intToString(std::string& str, int type);

    ///
    /// @brief      Convert section-type integer value to string.
    ///
    /// @param[out] str     The string reference to use for output.
    ///
    /// @param[in]  type    The integer value to examine.
    ///
    /// @return     The reference to the output string.
    ///
    static std::string& sectionType(std::string& str, unsigned type);

    ///
    /// @brief      Convert class-type integer value to string.
    ///
    /// @param[out] str     The string reference to use for output.
    ///
    /// @param[in]  type    The integer value to examine.
    ///
    /// @return     The reference to the output string.
    ///
    static std::string& classToString(std::string& str, int classType);

    ///
    /// @brief      Convert encoding-type integer value to string.
    ///
    /// @param[out] str     The string reference to use for output.
    ///
    /// @param[in]  type    The integer value to examine.
    ///
    /// @return     The reference to the output string.
    ///
    static std::string& encodingToString(std::string& str, int encoding);

    ///
    /// @brief      Convert version integer value to string.
    ///
    /// @param[out] str     The string reference to use for output.
    ///
    /// @param[in]  type    The integer value to examine.
    ///
    /// @return     The reference to the output string.
    ///
    static std::string& versionToString(std::string& str, int version);

    ///
    /// @brief      Convert OS-ABI integer value to string.
    ///
    /// @param[out] str     The string reference to use for output.
    ///
    /// @param[in]  type    The integer value to examine.
    ///
    /// @return     The reference to the output string.
    ///
    static std::string& osABItoString(std::string& str, int osABI);

    ///
    /// @brief      Convert type integer value to string.
    ///
    /// @param[out] str     The string reference to use for output.
    ///
    /// @param[in]  type    The integer value to examine.
    ///
    /// @return     The reference to the output string.
    ///
    static std::string& typeToString(std::string& str, int type);

    ///
    /// @brief      Convert machine-type integer value to string.
    ///
    /// @param[out] str     The string reference to use for output.
    ///
    /// @param[in]  type    The integer value to examine.
    ///
    /// @return     The reference to the output string.
    ///
    static std::string& machineToString(std::string& str, int machine);

    ///
    /// @brief      Convert section-flags integer value to string.
    ///
    /// @param[out] str     The string reference to use for output.
    ///
    /// @param[in]  type    The integer value to examine.
    ///
    /// @return     The reference to the output string.
    ///
    static std::string& flagsToString(std::string& str, int flags);

    ///
    /// @brief      Convert program-type integer value to string.
    ///
    static std::string& programTypeToString(std::string& str, int type);

    ///
    /// @brief      Convert program-flags integer value to string.
    ///
    /// @param[out] str     The string reference to use for output.
    ///
    /// @param[in]  type    The integer value to examine.
    ///
    /// @return     The reference to the output string.
    ///
    static std::string& programFlagsToString(std::string& str, int flags);

    ///
    /// @brief      Convert section-flags integer value to string.
    ///
    /// @param[out] str     The string reference to use for output.
    ///
    /// @param[in]  type    The integer value to examine.
    ///
    /// @return     The reference to the output string.
    ///
    static std::string& sectionFlagsToString(std::string& str, int flags);

    ///
    /// @brief      Display the ELF file header content.
    ///
    /// @param[in]  p2Header    The ELF32 file header staring address in memory.
    ///
    static void elfHeader(Elf32_Ehdr * p2Header);

    ///
    /// @brief      Display the ELF file program content.
    ///
    /// @param[in]  p2Header    The ELF32 program header staring address in memory.
    ///
    static void program(Elf32_Phdr * p2Program);

    ///
    /// @brief      Display the ELF section header content.
    ///
    /// @param[in]  p2Section   The ELF32 section header staring address in memory.
    ///
    /// @param[in]  p2Strings   Pointer to the name string table in memory.
    ///
    static void section(Elf32_Shdr * p2Section, char * p2Strings);

private:
    ///
    /// @brief      Default Elf32 constructor
    ///
    /// @details    Force the use of the explicit constructor by not
    ///             allowing the default constructor to exist.
    ///
    Elf32() = delete;

    ///
    /// @brief      Copy constructor
    ///
    /// @details    Make the class non-copyable.
    ///
    /// @param[in]  elf32       Reference to the Elf32 object
    ///                         to be copied.
    ///
    Elf32(const Elf32& elf32) = delete;

    ///
    /// @brief      Assignment operator
    ///
    /// @details    Make the class non-assignable.
    ///
    /// @param[in]  elf32       Reference to the Elf32 object
    ///                         to be copied.
    ///
    Elf32& operator = (const Elf32& elf32) = delete;

    ///
    /// @brief      Calculate a checksum at the starting address for a
    ///             given number of 32-bit integers.
    ///
    /// @param[in]  p2Address   The 32-bit address to start from.
    ///
    /// @return     The checksum 32-bit value.
    ///
    uint32_t calculateChecksum(uint32_t * pAddress);

    ///
    /// @brief      Place the named section in the memory at a given
    ///             order.
    ///
    /// @param[in]  sectionMap  The section map reference to use.
    ///
    /// @param[in]  sectionName The key string for the section to use.
    ///
    /// @param[in]  isFirst     Flag to denote the first section.
    ///
    void orderSection(SecMap& sectionMap,
                      const char * sectionName,
                      bool isFirst);

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
