
///
/// @file   Binary.cc
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
#include "Binary.hh"

using namespace std;

///
/// @brief      Binary constructor.
///
isp::Binary::Binary( std::string& filename,
                     uint8_t * pMemory,
                     size_t memSize )
     : m_filename( filename ),
       m_Size( 0U ),
       m_pBuffer( nullptr ),
       m_isDirty( false ),
       m_pMemory( pMemory ),
       m_StartAddress( 0U ),
       m_EndAddress( 0U )
{
    if (filename.size())
    {
        // Open the file
        m_ifs.open( m_filename, ios::binary );

        if( m_ifs.is_open() && m_ifs.good() )
        {
            // get length of file:
            m_ifs.seekg( 0, ios::end );
            m_Size = m_ifs.tellg();
            m_ifs.seekg( 0, ios::beg );

            m_pBuffer = new uint8_t[ m_Size ];
        }
        else if( !m_ifs.is_open() )
        {
            cerr << "Error: Cannot open file "
                 << m_filename
                 << endl;
        }
    }
}


///
/// @brief      Binary destructor.
///
isp::Binary::~Binary()
{
    if (m_pBuffer != nullptr)
        delete[] m_pBuffer;

    if (m_ifs.is_open())
        m_ifs.close();
}


///
/// @brief      Parse the ELF32 header.
///
bool isp::Binary::parse( bool isCheck, bool isDebug )
{
    m_StartAddress = 0U;
    m_EndAddress   = m_Size - 1;

    if( isCheck )
    {
        cout << "CHECKSUM is 0x" << hex << setw(8) << setfill('0')
             << calculateChecksum(reinterpret_cast<uint32_t *>(m_pBuffer),
                                  m_Size)
             << endl;
    }

    memcpy( m_pMemory,
            m_pBuffer,
            m_Size );

    return true;
}


///
/// @brief      Read the ELF32 file into memory.
///
bool isp::Binary::read()
{
    if( m_ifs.is_open() && m_ifs.good() && m_pBuffer && m_Size)
    {
        m_ifs.read( reinterpret_cast<char *>( m_pBuffer), m_Size );
        return true;
    }
    return false;
}


///
/// @brief      Write the memory block to the ELF32 file.
///
bool isp::Binary::write()
{
    bool result = false;

    // Open the file
    m_ofs.open( m_filename, ios::binary );

    // Write the content
    if( m_ofs.is_open() && m_ofs.good())
    {
        m_ofs.write( reinterpret_cast<char *>( m_pBuffer), m_Size );
        result = true;
    }

    // Close the file
    if( m_ofs.is_open() )
        m_ofs.close();
    return result;
}


///
/// @brief      Calculate a checksum at the starting address for a
///             given number of 32-bit integers.
///
uint32_t isp::Binary::calculateChecksum( uint32_t * pAddress, size_t size)
{
    uint32_t checksum = 0;

    for (size_t ii(0); ii < 7; ++ii)
    {
        checksum += pAddress[ii];
    }

    checksum = 0xffffffff - checksum + 1;
    if (pAddress[7] != checksum)
    {
        cout << "Updating checksum from 0x"
             << hex << setw(8) << setfill('0')
             << pAddress[7]
             << " to 0x"
             << hex << setw(8) << setfill('0')
             << checksum
             << endl;
        m_isDirty = true;
        pAddress[7] = checksum;
    }
    return checksum;
}
