///
/// @file   iHex.cc
///
/// @date   21 Dec, 2014
/// @author Don McNeill dmcneill@me.com
///

//  Includes
#include <vector>
#include <string>
#include <stdint.h>
#include "iHex.hh"
#include "Log.hh"
#include "Utility.hh"


//
//  @brief      Explicit class constructor.
//
isp::iHex::iHex( const char * filename, uint8_t * pMemory, size_t size )
      : mFilename( filename ),
        mOffsetAddress( 0U ),
        mStartAddress( size ),
        mEndAddress( 0U ),
        mpMemory( pMemory )
{}


//
//  @brief      Default class destructor.
//
isp::iHex::~iHex()
{}


//
//  @brief      Parse the file contents.
//
bool isp::iHex::parse()
{
    bool        result = false;

    // Open the stream
    if( mFilename.length())
    {
        std::ifstream inFile(mFilename.c_str());
        if (inFile.is_open())
        {
            std::string line;
            while (getline(inFile,line))
            {
                if (line[0] != ':')
                    break;

                if (line.length() < 11)
                    break;

                // Remove the leading ':' and trailing '\n'
                std::string trimmedLine = line.substr(1);
                isp::Utility::cutLast( trimmedLine );

                // Process the line
                if (!process( trimmedLine ))
                    break;
            }
            inFile.close();
            result = true;
        }
    }
    return result;
}


//
//  @brief      Process the Intel Hex line.
//
bool isp::iHex::process(std::string& line)
{
    bool result = false;
    std::vector<uint8_t> bytes;
    std::vector<uint8_t> data;

    do
    {
        Utility::stringToByte(const_cast<const std::string&>( line ), bytes );

        unsigned count    = bytes[0];
        unsigned address  = ((bytes[1] << 8) | bytes[2]);
        int      type     = bytes[3];
        unsigned cksum    = bytes[4 + count];
        unsigned checksum = 0;

        // Parse the data
        for( unsigned ii = 0; ii < count; ++ii )
            data.push_back( bytes[4 + ii]);

        // Calculate the checksum
        for( unsigned ii = 0; ii < bytes.size() - 1; ++ii )
            checksum += bytes[ii];
        checksum = (~(checksum - 1) & 0xFF);

        // See if we got a match
        if( checksum != cksum)
        {
            std::cerr << "Error checksum mismatch - inline: 0x"
                      << std::hex << cksum
                      << "  Calculated: 0x"
                      << std::hex << checksum
                      << std::endl;
            break;
        }

        result = true;

        switch (type)
        {
            case 0: // Data
            {
                if( address < mStartAddress )
                    mStartAddress = address;
                if( (address + count - 1) > mEndAddress )
                    mEndAddress = (address + count - 1);

                for( unsigned ii = 0; ii < data.size(); ++ii )
                    mpMemory[ mOffsetAddress + address + ii] = data[ ii ];
            }
            break;

            case 1: // EOF
            {
                // At the end; just return...
                LOG(INFO) << "starting address: 0x"
                          << std::hex << mStartAddress
                          << "  ending address: 0x"
                          << std::hex << mEndAddress;
                doChecksum();
                return result;
            }
            break;

            case 2: // Extended Segment Address
            {
                // Data contains segment address
                if( count == 2)
                {
                    mOffsetAddress = ((data[0] << 8) | data[1]) << 4;
                }
            }
            break;

            case 3: // Start Segment Address
            {
                if( count == 4)
                {
                    uint32_t start = ((data[0] << 8) | data[1]) << 4;
                    start += ((data[2] << 8) | data[3]);
                }
            }
            break;

            case 4: // Extended Linear Address
            {
                if( count == 2)
                {
                    mOffsetAddress = ((data[0] << 8) | data[1]) << 16;
                }
            }
            break;

            case 5: // Start Linear Address
            {
                if( count == 4)
                {
                    uint32_t start = ((data[0] << 8) | data[1]) << 16;
                    start |= ((data[2] << 8) | data[3]);
                }
            }
            break;
        }
    } while (false);

    return result;
}


//
//  @brief      Calculate 2's compliment checksum for the application.
//
void isp::iHex::doChecksum()
{
    uint32_t * pAddress = reinterpret_cast<uint32_t *>( mpMemory );
    uint32_t checksum = 0U;

    for( unsigned ii = 0; ii < 7; ++ii )
        checksum += *(pAddress + ii);
    pAddress[7] = (~checksum + 1);
}
