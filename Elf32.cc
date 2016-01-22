
///
/// @file   Elf32.cc
///
/// @date   20 Dec 2014
/// @author Don McNeill dmcneill@me.com
///
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "Elf32.hh"
#include "Log.hh"

using namespace std;


///
/// @brief      Elf32 constructor.
///
isp::Elf32::Elf32(std::string& filename,
                  uint8_t * pMemory,
                  size_t memSize)
     : m_filename(filename),
       m_Size(0U),
       m_pBuffer(nullptr),
       m_isDirty(false),
       m_pMemory(pMemory),
       m_StartAddress(0U),
       m_EndAddress(0U)
{
    if (filename.size())
    {
        // Open the file
        m_ifs.open(m_filename, ios::binary);

        if (m_ifs.is_open() && m_ifs.good())
        {
            // get length of file:
            m_ifs.seekg(0, ios::end);
            m_Size = m_ifs.tellg();
            m_ifs.seekg(0, ios::beg);

            m_pBuffer = new uint8_t[ m_Size ];
        }
        else if (!m_ifs.is_open())
        {
            LOG(ERROR) << "Error: Cannot open file "
                         << m_filename;
        }
    }
}


///
/// @brief      Elf32 destructor.
///
isp::Elf32::~Elf32()
{
    if (m_pBuffer != nullptr)
        delete[] m_pBuffer;

    if (m_ifs.is_open())
        m_ifs.close();
}


///
/// @brief      Parse the ELF32 header.
///
bool isp::Elf32::parse(bool isDebug)
{
    Elf32_Ehdr *    p2Header = (Elf32_Ehdr *) m_pBuffer;
    SecMap          sectionMap;
    int             i;

    if (p2Header->e_ident[4] == 0 ||
        p2Header->e_ident[5] == 0)
        return false;

    if (isDebug)
        isp::Elf32::elfHeader(p2Header);

    //  Program Header Section
    Elf32_Phdr * p2Program = (Elf32_Phdr *) (m_pBuffer + p2Header->e_phoff);
    if (isDebug)
        isp::Elf32::program(p2Program);

    //  Section Header Section
    Elf32_Shdr * p2Section = (Elf32_Shdr *) (m_pBuffer +
                             p2Header->e_shoff);
    Elf32_Shdr * p2StrTab  = (Elf32_Shdr *) (m_pBuffer +
                             p2Header->e_shoff + (p2Header->e_shstrndx *
                             p2Header->e_shentsize));
    char * p2Strings = ((char *) m_pBuffer + p2StrTab->sh_offset);

    // Generate the map of sections we're interested in
    for (i = 0; i < p2Header->e_shnum; i++)
    {
        if ((i != p2Header->e_shstrndx) && (p2Section->sh_type != 0))
        {
            if (isDebug)
                isp::Elf32::section(p2Section, p2Strings);

            if (p2Section->sh_size && (p2Section->sh_flags & SHF_ALLOC))
            {
                if (!strncmp(&p2Strings[p2Section->sh_name], ".text", 6) ||
                    !strncmp(&p2Strings[p2Section->sh_name], ".data", 6) ||
                    !strncmp(&p2Strings[p2Section->sh_name], ".ARM.extab", 11) ||
                    !strncmp(&p2Strings[p2Section->sh_name], ".ARM.exidx", 11))
                {
                    Section sec(&p2Strings[p2Section->sh_name],
                                p2Section->sh_size,
                                p2Section->sh_addr,
                                p2Section->sh_addralign,
                                m_pBuffer + p2Section->sh_offset);
                    sectionMap.insert(std::pair<std::string,Section>(sec.getName(), sec));
                }
            } 
        }
        p2Section = (Elf32_Shdr *) ((uint8_t *) p2Section + p2Header->e_shentsize);
    }

    orderSection(sectionMap,      ".text",  true);
    orderSection(sectionMap, ".ARM.extab", false);
    orderSection(sectionMap, ".ARM.exidx", false);
    orderSection(sectionMap,      ".data", false);
    return true;
}


///
/// @brief      Place the named section in the memory at a given
///             order.
void isp::Elf32::orderSection(SecMap& sectionMap,
                              const char * sectionName,
                              bool isFirst)
{
    for (SecMap::iterator it = sectionMap.begin(); it != sectionMap.end(); ++it)
    {
        std::string name = it->first;

        if (sectionName == name)
        {
            Section sec = it->second;
            uint32_t nextAddress = sec.getStartAddress();

            if (isFirst)
                m_StartAddress = sec.getStartAddress();

            if (name == ".data")
                nextAddress = m_EndAddress + 1;

            memcpy(m_pMemory + nextAddress,
                   sec.getData(),
                   sec.getSize());

            // Needed to boot into the application
            if (isFirst)
                calculateChecksum(reinterpret_cast<uint32_t *>(m_pMemory + nextAddress));
                
            if (m_EndAddress < nextAddress + sec.getSize() - 1)
                m_EndAddress = nextAddress + sec.getSize() - 1;

            LOG(INFO) << setw(12) << setfill(' ')
                      << sec.getName()
                      << "  0x" << hex << setw(8) << setfill('0')
                      << nextAddress
                      << " --> "
                      << "0x" << hex << setw(8) << setfill('0')
                      << m_EndAddress;
        }
    }
}


///
/// @brief      Read the ELF32 file into memory.
///
bool isp::Elf32::read()
{
    if (m_ifs.is_open() && m_ifs.good() && m_pBuffer && m_Size)
    {
        m_ifs.read(reinterpret_cast<char *>(m_pBuffer), m_Size);
        return true;
    }
    return false;
}


///
/// @brief      Write the memory block to the ELF32 file.
///
bool isp::Elf32::write()
{
    bool result = false;

    // Open the file
    m_ofs.open(m_filename, ios::binary);

    // Write the content
    if (m_ofs.is_open() && m_ofs.good())
    {
        m_ofs.write(reinterpret_cast<char *>(m_pBuffer), m_Size);
        result = true;
    }

    // Close the file
    if (m_ofs.is_open())
        m_ofs.close();
    return result;
}


////////////////////////////////////////////////////////////
// Static methods
////////////////////////////////////////////////////////////

///
/// @brief      Align an address value.
///
uint32_t isp::Elf32::alignAddress(uint32_t address, unsigned align)
{
    uint32_t newAddress;

    switch (align)
    {
        default:
        case  1: newAddress = ((address - 1) +  1) & 0xFFFFFFFF; break;
        case  2: newAddress = ((address - 1) +  2) & 0xFFFFFFFE; break;
        case  4: newAddress = ((address - 1) +  4) & 0xFFFFFFFC; break;
        case  8: newAddress = ((address - 1) +  8) & 0xFFFFFFF8; break;
        case 16: newAddress = ((address - 1) + 16) & 0xFFFFFFF0; break;
    }
    return newAddress;
}


///
/// @brief      Convert integer value to string.
///
std::string& isp::Elf32::intToString(std::string& str, int type)
{
   std::stringstream out;
   out << hex << type;
   str = out.str();
   return str;
}


///
/// @brief      Convert section-type integer value to string.
///
std::string& isp::Elf32::sectionType(std::string& str, unsigned type)
{
   str = "";
   switch (type)
   {
      case 0: str = "NULL"; break;
      case 1: str = "PROGBITS"; break;
      case 2: str = "SYMTAB"; break;
      case 3: str = "STRTAB"; break;
      case 4: str = "RELA"; break;
      case 5: str = "HASH"; break;
      case 6: str = "DYNAMIC"; break;
      case 7: str = "NOTE"; break;
      case 8: str = "NOBITS"; break;
      case 9: str = "REL"; break;
      case 10: str = "SHLIB"; break;
      case 11: str = "DYNSYM"; break;
      case 14: str = "INIT_ARRAY"; break;
      case 15: str = "FINI_ARRAY"; break;
      case 16: str = "PREINIT_ARRAY"; break;
      case 17: str = "GROUP"; break;
      case 18: str = "SYMTAB_SHNDX"; break;
      case 19: str = "NUM"; break;

      case 0x60000000: str = "LOOS"; break;
      case 0x6FFFFFF5: str = "GNU_ATTRIBUTES"; break;
      case 0x6FFFFFF6: str = "GNU_HASH"; break;
      case 0x6FFFFFF7: str = "GNU_LIBLIST"; break;
      case 0x6FFFFFF8: str = "CHECKSUM"; break;
      case 0x6FFFFFFD: str = "GNU_VERDEF"; break;
      case 0x6FFFFFFE: str = "GNU_VERNEED"; break;
      case 0x6FFFFFFF: str = "GNU_VERSYM"; break;
      case 0x70000000: str = "LOPROC"; break;
      case 0x7FFFFFFF: str = "HIPROC"; break;
      case 0x80000000: str = "LOUSER"; break;
      case 0x8FFFFFFF: str = "HIUSER"; break;
      default: intToString(str, type); break;
   }
   return str;
}


///
/// @brief      Convert class-type integer value to string.
///
std::string& isp::Elf32::classToString(std::string& str, int classType)
{
   str = "";
   switch (classType)
   {
      case ELFCLASSNONE: str = "NONE"; break;
      case   ELFCLASS32: str = "32-bit"; break;
      case   ELFCLASS64: str = "64-bit"; break;
      default: str = "<<Unknown>>"; break;
   }
   return str;
}


///
/// @brief      Convert encoding-type integer value to string.
///
std::string& isp::Elf32::encodingToString(std::string& str, int encoding)
{
   str = "";
   switch (encoding)
   {
      case ELFDATANONE: str = "NONE"; break;
      case ELFDATA2LSB: str = "2's Comp LE"; break;
      case ELFDATA2MSB: str = "2's Comp BE"; break;
      default: str = "<<Unknown>>"; break;
   }
   return str;
}


///
/// @brief      Convert version integer value to string.
///
std::string& isp::Elf32::versionToString(std::string& str, int version)
{
   str = "";
   switch (version)
   {
      case    EV_NONE: str = "NONE"; break;
      case EV_CURRENT: str = "CURRENT"; break;
      default: str = "<<Unknown>>"; break;
   }
   return str;
}


///
/// @brief      Convert OS-ABI integer value to string.
///
std::string& isp::Elf32::osABItoString(std::string& str, int osABI)
{
   str = "";
   switch (osABI)
   {
      case       ELFOSABI_SYSV: str = "UNIX System V"; break;
      case       ELFOSABI_HPUX: str = "HPUX"; break;
      case     ELFOSABI_NETBSD: str = "NetBSD"; break;
      case      ELFOSABI_LINUX: str = "Linux"; break;
      case    ELFOSABI_SOLARIS: str = "Sun Solaris"; break;
      case        ELFOSABI_AIX: str = "IBM AIX"; break;
      case       ELFOSABI_IRIX: str = "SGI IRIX"; break;
      case    ELFOSABI_FREEBSD: str = "FreeBSD"; break;
      case      ELFOSABI_TRU64: str = "Compaq TRU64 UNIX"; break;
      case    ELFOSABI_MODESTO: str = "Novell Modesto"; break;
      case    ELFOSABI_OPENBSD: str = "OpenBSD"; break;
      case  ELFOSABI_ARM_AEABI: str = "ARM EABI"; break;
      case        ELFOSABI_ARM: str = "ARM"; break;
      case ELFOSABI_STANDALONE: str = "Standalone (Embedded)"; break;
      default: str = "<<Unknown>>"; break;
   }
   return str;
}


///
/// @brief      Convert type integer value to string.
///
std::string& isp::Elf32::typeToString(std::string& str, int type)
{
   str = "";
   switch (type)
   {
      case ET_NONE: str = "None"; break;
      case  ET_REL: str = "Relocatable file"; break;
      case ET_EXEC: str = "Executable file"; break;
      case  ET_DYN: str = "Shared object file"; break;
      case ET_CORE: str = "Core file"; break;
      default: str = "<<Unknown>>"; break;
   }
   return str;
}


///
/// @brief      Convert machine-type integer value to string.
///
std::string& isp::Elf32::machineToString(std::string& str, int machine)
{
   str = "";
   switch (machine)
   {
      case        EM_NONE: str = "None"; break;
      case         EM_M32: str = "AT&T WE 32100"; break;
      case       EM_SPARC: str = "SUN SPARC"; break;
      case         EM_386: str = "Intel 80386"; break;
      case         EM_68K: str = "Motorola m68k family"; break;
      case         EM_88K: str = "Motorola m88k family"; break;
      case         EM_860: str = "Intel 80860"; break;
      case        EM_MIPS: str = "MIPS R3000 big-endian"; break;
      case        EM_S370: str = "IBM System/370"; break;
      case EM_MIPS_RS3_LE: str = "MIPS R3000 little-endian"; break;
      case      EM_PARISC: str = "HPPA"; break;
      case      EM_VPP500: str = "Fujitsu VPP500"; break;
      case EM_SPARC32PLUS: str = "Sun's v8plus"; break;
      case         EM_960: str = "Intel 80960"; break;
      case         EM_PPC: str = "PowerPC"; break;
      case       EM_PPC64: str = "PowerPC 64-bit"; break;
      case        EM_S390: str = "IBM S390"; break;
      case        EM_V800: str = "NEC V800 series"; break;
      case        EM_FR20: str = "Fujitsu FR20"; break;
      case        EM_RH32: str = "TRW RH-32"; break;
      case         EM_RCE: str = "Motorola RCE"; break;
      case         EM_ARM: str = "ARM"; break;
      case  EM_FAKE_ALPHA: str = "Digital Alpha"; break;
      case          EM_SH: str = "Hitachi SH"; break;
      case     EM_SPARCV9: str = "SPARC v9 64-bit"; break;
      case     EM_TRICORE: str = "Siemens Tricore"; break;
      case         EM_ARC: str = "Argonaut RISC Core"; break;
      case      EM_H8_300: str = "Hitachi H8/300"; break;
      case     EM_H8_300H: str = "Hitachi H8/300H"; break;
      case         EM_H8S: str = "Hitachi H8S"; break;
      case      EM_H8_500: str = "Hitachi H8/500"; break;
      case       EM_IA_64: str = "Intel Merced"; break;
      case      EM_MIPS_X: str = "Stanford MIPS-X"; break;
      case    EM_COLDFIRE: str = "Motorola Coldfire"; break;
      case      EM_68HC12: str = "Motorola M68HC12"; break;
      case         EM_MMA: str = "Fujitsu MMA Multimedia Accelerato"; break;
      case         EM_PCP: str = "Siemens PCP"; break;
      case        EM_NCPU: str = "Sony nCPU embeeded RISC"; break;
      case        EM_NDR1: str = "Denso NDR1 microprocessor"; break;
      case    EM_STARCORE: str = "Motorola Start*Core processor"; break;
      case        EM_ME16: str = "Toyota ME16 processor"; break;
      case       EM_ST100: str = "STMicroelectronic ST100 processor"; break;
      case       EM_TINYJ: str = "Advanced Logic Corp. Tinyj emb.fa"; break;
      case      EM_X86_64: str = "AMD x86-64 architecture"; break;
      case        EM_PDSP: str = "Sony DSP Processor"; break;
      case        EM_FX66: str = "Siemens FX66 microcontroller"; break;
      case     EM_ST9PLUS: str = "STMicroelectronics ST9+ 8/16 mc"; break;
      case         EM_ST7: str = "STmicroelectronics ST7 8 bit mc"; break;
      case      EM_68HC16: str = "Motorola MC68HC16 microcontroller"; break;
      case      EM_68HC11: str = "Motorola MC68HC11 microcontroller"; break;
      case      EM_68HC08: str = "Motorola MC68HC08 microcontroller"; break;
      case      EM_68HC05: str = "Motorola MC68HC05 microcontroller"; break;
      case         EM_SVX: str = "Silicon Graphics SVx"; break;
      case        EM_ST19: str = "STMicroelectronics ST19 8 bit mc"; break;
      case         EM_VAX: str = "Digital VAX"; break;
      case        EM_CRIS: str = "Axis Communications 32-bit"; break;
      case     EM_JAVELIN: str = "Infineon Technologies 32-bit"; break;
      case    EM_FIREPATH: str = "Element 14 64-bit DSP"; break;
      case         EM_ZSP: str = "LSI Logic 16-bit DSP"; break;
      case        EM_MMIX: str = "Donald Knuth's educational 64-bit"; break;
      case       EM_HUANY: str = "Harvard machine-independent object"; break;
      case       EM_PRISM: str = "SiTera Prism"; break;
      case         EM_AVR: str = "Atmel AVR 8-bit microcontroller"; break;
      case        EM_FR30: str = "Fujitsu FR30"; break;
      case        EM_D10V: str = "Mitsubishi D10V"; break;
      case        EM_D30V: str = "Mitsubishi D30V"; break;
      case        EM_V850: str = "NEC v850"; break;
      case        EM_M32R: str = "Mitsubishi M32R"; break;
      case     EM_MN10300: str = "Matsushita MN10300"; break;
      case     EM_MN10200: str = "Matsushita MN10200"; break;
      case          EM_PJ: str = "picoJava"; break;
      case    EM_OPENRISC: str = "OpenRISC 32-bit"; break;
      case      EM_ARC_A5: str = "ARC Cores Tangent-A5"; break;
      case      EM_XTENSA: str = "Tensilica Xtensa Architecture"; break;
      case       EM_ALPHA: str = "Alpha"; break;
      default: str = "<<Unknown>>"; break;
   }
   return str;
}


///
/// @brief      Convert section-flags integer value to string.
///
std::string& isp::Elf32::flagsToString(std::string& str, int flags)
{
   str = "";

   for (int i = 0; i < 32; i++)
   {
      int bit = flags & (1 << i);

      switch (bit)
      {
         case            SHF_WRITE: str += "WRITE "; break;
         case            SHF_ALLOC: str += "ALLOC "; break;
         case        SHF_EXECINSTR: str += "EXEC "; break;
         case            SHF_MERGE: str += "MERGE "; break;
         case          SHF_STRINGS: str += "STRINGS "; break;
         case        SHF_INFO_LINK: str += "INFO-LINK"; break;
         case       SHF_LINK_ORDER: str += "LINK-ORDER "; break;
         case SHF_OS_NONCONFORMING: str += "NONCONFORMING"; break;
         case            SHF_GROUP: str += "GROUP "; break;
         case              SHF_TLS: str += "TLS "; break;
         case          SHF_ORDERED: str += "ORDERED "; break;
         case          SHF_EXCLUDE: str += "EXCLUDE "; break;
         default: break;
      }
   }
   str += '(';
   std::string n;
   intToString(n, flags);
   str += n;
   str += ')';
   return str;
}


///
/// @brief      Convert program-type integer value to string.
///
/// @param[out] str     The string reference to use for output.
///
/// @param[in]  type    The integer value to examine.
///
/// @return     The reference to the output string.
///
std::string& isp::Elf32::programTypeToString(std::string& str, int type)
{
   str = "";
   switch (type)
   {
      case         PT_NULL: str = "UNUSED"; break;
      case         PT_LOAD: str = "Loadable program segment"; break;
      case      PT_DYNAMIC: str = "Dynamic linking information"; break;
      case       PT_INTERP: str = "Program interpreter"; break;
      case         PT_NOTE: str = "Auxiliary information"; break;
      case        PT_SHLIB: str = "Reserved"; break;
      case         PT_PHDR: str = "Entry for header table itself"; break;
      case          PT_TLS: str = "Thread-local storage segment"; break;
      case         PT_LOOS: str = "Start of OS-specific"; break;
      case PT_GNU_EH_FRAME: str = "eh_frame_hdr"; break;
      case    PT_GNU_STACK: str = "stack executability"; break;
      case    PT_GNU_RELRO: str = "Read-only after relocation"; break;
      case      PT_SUNWBSS: str = "Sun Specific segment"; break;
      case    PT_SUNWSTACK: str = "SUN Stack segment"; break;
      case         PT_HIOS: str = "End of OS-specific"; break;
      case       PT_LOPROC: str = "Start of processor-specific"; break;
      case       PT_HIPROC: str = "End of processor-specific"; break;
      default: str = "<<Unknown>>"; break;
   }
   return str;
}


///
/// @brief      Convert program-flags integer value to string.
///
std::string& isp::Elf32::programFlagsToString(std::string& str, int flags)
{
   str = "";

   for (int i = 0; i < 32; i++)
   {
      int bit = flags & (1 << i);

      switch (bit)
      {
         case PF_X: str += "E "; break;
         case PF_W: str += "W "; break;
         case PF_R: str += "R "; break;
         default: break;
      }
   }
   return str;
}


///
/// @brief      Convert section-flags integer value to string.
///
std::string& isp::Elf32::sectionFlagsToString(std::string& str, int flags)
{
   str = "";

   for (int i = 0; i < 3; i++)
   {
      int bit = flags & (1 << i);

      switch (bit)
      {
         case            SHF_WRITE: str += "WRITE "; break;
         case            SHF_ALLOC: str += "ALLOC "; break;
         case        SHF_EXECINSTR: str += "EXEC "; break;
         default: break;
      }
   }
   str += '(';
   std::string n;
   intToString(n, flags);
   str += n;
   str += ')';
   return str;
}


///
/// @brief      Display the ELF file header content.
///
void isp::Elf32::elfHeader(Elf32_Ehdr * p2Header)
{
    std::string str;

    LOG(INFO) << "ELF Ident / Magic........................: ."
              << p2Header->e_ident[1]
              << p2Header->e_ident[2]
              << p2Header->e_ident[3];
    LOG(INFO) << " Class...................................: "
              << isp::Elf32::classToString(str, p2Header->e_ident[4]);
    LOG(INFO) << " Encoding................................: "
              << isp::Elf32::encodingToString(str, p2Header->e_ident[5]);
    LOG(INFO) << " Version.................................: "
              << isp::Elf32::versionToString(str, p2Header->e_ident[6]);
    LOG(INFO) <<" OS ABI..................................: "
              << isp::Elf32::osABItoString(str, p2Header->e_ident[7]);
    LOG(INFO) <<" ABI Version.............................: "
              << (int) p2Header->e_ident[8];
    LOG(INFO) <<" Pad Index...............................: "
              << (int) p2Header->e_ident[9];
    LOG(INFO) <<"ELF Type.................................: "
              << isp::Elf32::typeToString(str, p2Header->e_type);
    LOG(INFO) <<"ELF Machine..............................: "
              << isp::Elf32::machineToString(str, p2Header->e_machine);
    LOG(INFO) <<"ELF Version..............................: "
              << isp::Elf32::versionToString(str, p2Header->e_version);
    LOG(INFO) <<"ELF Entry Address........................: "
              << "0x" << hex << setw(8) << setfill('0')
              << p2Header->e_entry;
    LOG(INFO) <<"ELF Program Header Offset................: "
              << "0x" << hex << setw(8) << setfill('0')
              << p2Header->e_phoff;
    LOG(INFO) <<"ELF Section Header Offset................: "
              << "0x" << hex << setw(8) << setfill('0')
              << p2Header->e_shoff;
    LOG(INFO) <<"ELF Flags................................: "
              << isp::Elf32::flagsToString(str, p2Header->e_flags);
    LOG(INFO) <<"ELF Section Header Size..................: "
              << p2Header->e_ehsize;
    LOG(INFO) <<"ELF Program Header Entry Size............: "
              << p2Header->e_phentsize;
    LOG(INFO) <<"ELF Number of Program Header Entries.....: "
              << p2Header->e_phnum;
    LOG(INFO) <<"ELF Section Header Entry Size............: "
              << p2Header->e_shentsize;
    LOG(INFO) <<"ELF Number of Section Header Entries.....: "
              << p2Header->e_shnum;
    LOG(INFO) <<"ELF Section Name String Table Index......: "
              << "0x" << hex << setw(8) << setfill('0')
              << p2Header->e_shstrndx;
}


///
/// @brief      Display the ELF file program content.
///
void isp::Elf32::program(Elf32_Phdr * p2Program)
{
    std::string str;

    LOG(INFO) << " Program Type............................: "
              << isp::Elf32::programTypeToString(str, p2Program->p_type);
    LOG(INFO) << " Program Segment File Offset.............: "
              << "0x" << hex << setw(8) << setfill('0')
              << p2Program->p_offset;
    LOG(INFO) << " Program Virtual Address.................: "
              << "0x" << hex << setw(8) << setfill('0')
              << p2Program->p_vaddr;
    LOG(INFO) << " Program Physical Address................: "
              << "0x" << hex << setw(8) << setfill('0')
              << p2Program->p_paddr;
    LOG(INFO) << " Program Segment File Size...............: "
              << "0x" << hex << setw(8) << setfill('0')
              << p2Program->p_filesz;
    LOG(INFO) << " Program Segment Memory Size.............: "
              << "0x" << hex << setw(8) << setfill('0')
              << p2Program->p_memsz;
    LOG(INFO) << " Program Segment Flags...................: "
              << isp::Elf32::programFlagsToString(str, p2Program->p_flags);
    LOG(INFO) << " Program Segment Alignment...............: "
              << p2Program->p_align;
}


///
/// @brief      Display the ELF section header content.
///
void isp::Elf32::section(Elf32_Shdr * p2Section, char * p2Strings)
{
    std::string typeStr;
    std::string str;
    sectionType(typeStr, (int) p2Section->sh_type);

    LOG(INFO);
    LOG(INFO) << " Section Type............................: "
              << typeStr;
//  if (p2Section->sh_type != 0L)
    {
        LOG(INFO) << " Section Name............................: "
                  << &p2Strings[p2Section->sh_name];
        LOG(INFO) << " Section Flags...........................: "
                  << isp::Elf32::sectionFlagsToString(str, p2Section->sh_flags);
        LOG(INFO) << " Section Virtual Address at Execution....: "
                  << "0x" << hex << setw(8) << setfill('0')
                  << p2Section->sh_addr;
        LOG(INFO) << " Section File Offset.....................: "
                  << "0x" << hex << setw(8) << setfill('0')
                  << p2Section->sh_offset;
        LOG(INFO) << " Section Size............................: "
                  << "0x" << hex << setw(8) << setfill('0')
                  << p2Section->sh_size;
        LOG(INFO) << " Section Link (to another section).......: "
                  << "0x" << hex << setw(8) << setfill('0')
                  << p2Section->sh_link;
        LOG(INFO) << " Section Information.....................: "
                  << "0x" << hex << setw(8) << setfill('0')
                  << p2Section->sh_info;
        LOG(INFO) << " Section Alignment.......................: "
                  << p2Section->sh_addralign;
        LOG(INFO) << " Section Number of table entries.........: "
                  << p2Section->sh_entsize;
    }
    return;
}


///
/// @brief      Calculate a checksum at the starting address for a
///             given number of 32-bit integers.
///
uint32_t isp::Elf32::calculateChecksum(uint32_t * pAddress)
{
    uint32_t checksum = 0;

    for (size_t ii(0); ii < 7; ++ii)
    {
        checksum += pAddress[ii];
    }

    checksum = 0xffffffff - checksum + 1;
    if (pAddress[7] != checksum)
    {
        LOG(INFO) << "Updating checksum from 0x"
                  << hex << setw(8) << setfill('0')
                  << pAddress[7]
                  << " to 0x"
                  << hex << setw(8) << setfill('0')
                  << checksum;
        m_isDirty = true;
        pAddress[7] = checksum;
    }
    return checksum;
}
