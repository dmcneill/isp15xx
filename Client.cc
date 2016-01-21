///
/// @file   Client.cc
///
/// @date   30 Dec 2014
/// @author Don McNeill dmcneill@me.com
///

//  Includes
#include <iomanip>
#include <iostream>
#include "Client.hh"
#include "iHex.hh"
#include "Log.hh"
#include "Serial.hh"
#include "Utility.hh"


// External References
extern  bool        gIsVerbose;
extern  bool        gIsActiveLowReset;
extern  bool        gQuit;
extern  uint32_t    gStartAddress;
extern  uint32_t    gEndAddress;
extern  uint32_t    gStartSector;
extern  uint32_t    gEndSector;
extern  uint8_t     gMemory[];

//
//  @brief      Test the client ISP interface.
//
isp::ISP::Error isp::eraseClient(const char * device, const unsigned syncRetries)
{
    LOG(INFO) << "Entering " << __func__ << "()";

    isp::Serial     serial(device);
    isp::ISP        isp(serial, gIsActiveLowReset, gIsVerbose);
    isp::ISP::Error error = isp::ISP::ERR_ISP_NO_ERROR;

    do
    {
        for (unsigned retries = syncRetries; retries > 0; --retries)
        {
            // Enter ISP programming mode.
            isp.programMode();

            if (gQuit == true)
                break;

            // Synchronize to the target
            if ((error = isp.synchronize()))
            {
                LOG(WARNING) << "Initial synchronization failed: " << error;
                if (retries)
                {
                    LOG(INFO) << "Retrying synchronization...";
                    continue;
                }
            }
            else
            {
                break;
            }
        }

        if (error != isp::ISP::ERR_ISP_NO_ERROR)
        {
            LOG(ERROR) << "Synchronization failed -- ABORTING";
            break;
        }

        // Setup the baud rate
        if ((error = isp.setBaudRate(115200)))
        {
            LOG(ERROR) << "Error in setting baud rate: " << error;
            break;
        }

        // Target chip ID
        uint32_t chip = 0U;
        if ((error = isp.queryId(chip)))
        {
            LOG(ERROR) << "Error in querying chip ID: " << error;
            break;
        }

        // Unlock flash
        if ((error = isp.unlockFlash()))
        {
            LOG(ERROR) << "Error in unlocking flash: " << error;
            break;
        }

        // Blank check
        std::vector<bool> sectorMap
        {
             false, false, false, false, false, false, false, false,
             false, false, false, false, false, false, false, false,
             false, false, false, false, false, false, false, false,
             false, false, false, false, false, false, false, false,
             false, false, false, false, false, false, false, false,
             false, false, false, false, false, false, false, false,
             false, false, false, false, false, false, false, false,
             false, false, false, false, false, false, false, false
        };

        LOG(INFO) << "Blank check...";
        for (unsigned ii = 0; ii < 64; ++ii)
        {
            error = isp.blankCheckSector(ii, sectorMap);
            LOG(INFO) << "Sector " << ii << " is " << (sectorMap[ii]? "blank": "NOT-BLANK");
        }

        LOG(INFO) << "Erasing flash...";
        gEndSector = 63;

        // Now start to program....
        for (int32_t sector = gEndSector; sector >= 0; --sector)
        {
            do
            {
                // Unlock flash
                if ((error = isp.unlockFlash(isp::ISP::SHORT_TIMEOUT)))
                {
                    LOG(ERROR) << "Error in unlocking flash: " << error;
                    break;
                }

                // If the sector is not blank, erase it.
                if (!sectorMap[ sector ])
                {
                    // Prepare sectors for writing
                    if ((error = isp.prepareSectors(sector, sector, isp::ISP::MEDIUM_TIMEOUT)))
                    {
                        LOG(ERROR) << "Error preparing sectors: " << error;
                        break;
                    }

                    // Erase flash
                    if ((error = isp.eraseSectors(sector, sector, isp::ISP::LONG_TIMEOUT)))
                    {
                        LOG(ERROR) << "Error erasing sectors: " << error;
                        break;
                    }
                }

            } while (false);
        }
    } while (false);

    LOG(INFO) << "Leaving eraseClient: errorCode is " << error;

    isp.applicationMode();
    return error;
}


//
//  @brief      Test the client ISP interface.
//
isp::ISP::Error isp::programClient(const char * device, const unsigned syncRetries)
{
    isp::Serial     serial(device);
    isp::ISP        isp(serial, gIsActiveLowReset, gIsVerbose);
    isp::ISP::Error error = isp::ISP::ERR_ISP_NO_ERROR;

    LOG(INFO) << "Entering " << __func__ << "()";

    do
    {
        for (unsigned retries = syncRetries; retries > 0; --retries)
        {
            // Enter ISP programming mode.
            isp.programMode();

            if (gQuit == true)
                break;

            // Synchronize to the target
            if ((error = isp.synchronize()))
            {
                LOG(WARNING) << "Initial synchronization failed: " << error;
                if (retries)
                {
                    LOG(INFO) << "Retrying synchronization...";
                    continue;
                }
            }
            else
            {
                break;
            }
        }

        if (error != isp::ISP::ERR_ISP_NO_ERROR)
        {
            LOG(ERROR) << "Synchronization failed -- ABORTING";
            break;
        }

        // Setup the baud rate
        if ((error = isp.setBaudRate(115200)))
        {
            LOG(ERROR) << "Error in setting baud rate: " << error;
            break;
        }

        // Target chip ID
        uint32_t chip = 0U;
        if ((error = isp.queryId(chip)))
        {
            LOG(ERROR) << "Error in querying chip ID: " << error;
            break;
        }

        // Blank check
        std::vector<bool> sectorMap
        {
             false, false, false, false, false, false, false, false,
             false, false, false, false, false, false, false, false,
             false, false, false, false, false, false, false, false,
             false, false, false, false, false, false, false, false,
             false, false, false, false, false, false, false, false,
             false, false, false, false, false, false, false, false,
             false, false, false, false, false, false, false, false,
             false, false, false, false, false, false, false, false
        };

        LOG(INFO) << "Blank check...";
        for (unsigned ii = gStartSector; ii <= gEndSector; ++ii)
        {
            error = isp.blankCheckSector(ii, sectorMap);
            LOG(INFO) << "Sector " << ii << " is " << (sectorMap[ii]? "blank": "NOT-BLANK");
        }

        LOG(INFO) << "Programming flash...";

        // Now start to program...
        for (int32_t sector = gEndSector; sector >= 0; --sector)
        {
            do
            {
                // Unlock flash
                if ((error = isp.unlockFlash(isp::ISP::SHORT_TIMEOUT)))
                {
                    LOG(ERROR) << "Error in unlocking flash: " << error;
                    break;
                }

                // If the sector is not blank, erase it.
                if (!sectorMap[ sector ])
                {
                    // Prepare sectors for writing
                    if ((error = isp.prepareSectors(sector, sector, isp::ISP::MEDIUM_TIMEOUT)))
                    {
                        LOG(ERROR) << "Error preparing sectors: " << error;
                        break;
                    }

                    // Erase flash
                    if ((error = isp.eraseSectors(sector, sector, isp::ISP::LONG_TIMEOUT)))
                    {
                        LOG(ERROR) << "Error erasing sectors: " << error;
                        break;
                    }
                }

                // Write the memory to flash
                for (int32_t ram = FLASH_SECTOR_SIZE - RAM_SECTOR_SIZE; ram >= 0; ram -= RAM_SECTOR_SIZE )
                {
                    // Disable echo
                    if ((error = isp.echo(false, isp::ISP::MEDIUM_TIMEOUT)))
                    {
                        LOG(ERROR) << "Error in setting echo: " << error;
                        break;
                    }

                    size_t offset = (sector * FLASH_SECTOR_SIZE + ram);
                    {
                        std::vector<uint8_t> ramBytes(&gMemory[offset],
                                                      &gMemory[offset + (RAM_SECTOR_SIZE / 2)]);

                        // Now write the RAM with the data to program
                        if ((error = isp.writeMemory(RAM_PROGRAM_ADDRESS,
                                                     (RAM_SECTOR_SIZE / 2),
                                                     ramBytes,
                                                     isp::ISP::LONG_TIMEOUT)))
                        {
                            LOG(ERROR) << "Error in writing memory: " << error;
                            break;
                        }
                    }

                    offset += (RAM_SECTOR_SIZE / 2);
                    {
                        std::vector<uint8_t> ramBytes(&gMemory[offset],
                                                      &gMemory[offset + (RAM_SECTOR_SIZE / 2)]);

                        // Now write the RAM with the data to program
                        if ((error = isp.writeMemory(RAM_PROGRAM_ADDRESS + (RAM_SECTOR_SIZE / 2),
                                                     (RAM_SECTOR_SIZE / 2),
                                                     ramBytes,
                                                     isp::ISP::LONG_TIMEOUT)))
                        {
                            LOG(ERROR) << "Error in writing memory: " << error;
                            break;
                        }
                    }

                    // Enable echo
                    if ((error = isp.echo(true, isp::ISP::MEDIUM_TIMEOUT)))
                    {
                        LOG(ERROR) << "Error in setting echo: " << error;
                        break;
                    }

                    // Unlock flash
                    if ((error = isp.unlockFlash(isp::ISP::MEDIUM_TIMEOUT)))
                    {
                        LOG(ERROR) << "Error in unlocking flash: " << error;
                        break;
                    }

                    // Prepare sectors for writing
                    if ((error = isp.prepareSectors(sector, sector, isp::ISP::MEDIUM_TIMEOUT)))
                    {
                        LOG(ERROR) << "Error preparing sectors: " << error;
                        break;
                    }

                    // Copy to flash
                    uint32_t flashAddress = (sector * FLASH_SECTOR_SIZE + ram);
                    LOG(INFO) << "Writing flash at 0x"
                              << std::setw(8) << std::setfill('0') << std::hex << flashAddress;

                    if ((error = isp.copyToFlash(flashAddress,
                                                 RAM_PROGRAM_ADDRESS,
                                                 RAM_SECTOR_SIZE,
                                                  isp::ISP::LONG_TIMEOUT )))
                    {
                        LOG(ERROR) << "Error on copy to flash: " << error;
                        break;
                    }
                }

            } while (false);
        }

        if (error == isp::ISP::ERR_ISP_NO_ERROR)
            LOG(INFO) << "Programming flash success!";

    } while (false);

    LOG(INFO) << "Leaving " << __func__ << "(): errorCode is " << error;
    isp.applicationMode();
    return error;
}


//
//  @brief      Test the client ISP interface.
//
isp::ISP::Error isp::examineClient(const char * device, const unsigned syncRetries)
{
    LOG(INFO) << "Entering " << __func__ << "()";

    isp::Serial     serial(device);
    isp::ISP        isp(serial, gIsActiveLowReset, gIsVerbose);
    isp::ISP::Error error = isp::ISP::ERR_ISP_NO_ERROR;

    do
    {
        for (unsigned retries = syncRetries; retries > 0; --retries)
        {
            // Enter ISP programming mode.
            isp.programMode();

            if (gQuit == true)
                break;

            // Synchronize to the target
            if ((error = isp.synchronize()))
            {
                LOG(WARNING) << "Initial synchronization failed: " << error;
                if (retries)
                {
                    LOG(INFO) << "Retrying synchronization...";
                    continue;
                }
            }
            else
            {
                break;
            }
        }

        if (error != isp::ISP::ERR_ISP_NO_ERROR)
        {
            LOG(ERROR) << "Synchronization failed -- ABORTING";
            break;
        }

        // Setup the baud rate
        if ((error = isp.setBaudRate(115200)))
        {
            LOG(ERROR) << "Error in setting baud rate: " << error;
            break;
        }

        // Target chip ID
        uint32_t chip = 0U;
        if ((error = isp.queryId(chip)))
        {
            LOG(ERROR) << "Error in querying chip ID: " << error;
            break;
        }

        uint8_t * memBlock = new uint8_t[ (gEndSector - gStartSector + 1) * FLASH_SECTOR_SIZE ];
        if (!memBlock )
            break;

        LOG(INFO) << "Verifying...";

        // Now start to read the memory...
        for (uint32_t sector = gStartSector; sector <= gEndSector; ++sector)
        {
            do
            {
                // Read memory...
                for (uint32_t ram = 0U; ram < FLASH_SECTOR_SIZE; ram += RAM_SECTOR_SIZE )
                {
                    size_t offset = (sector * FLASH_SECTOR_SIZE + ram);
                    {
                        std::vector<uint8_t> ramBytes;

                        // Now write the RAM with the data to program
                        if ((error = isp.readMemory(offset, (RAM_SECTOR_SIZE / 2), ramBytes)))
                        {
                            LOG(ERROR) << "Error in reading memory: " << error;
                            break;
                        }
                        else
                        {
                            for (unsigned ii = 0; ii < ramBytes.size(); ++ii)
                                memBlock[ offset + ii ] = ramBytes.data()[ ii ];

                            if (gIsVerbose)
                                isp::Utility::hexDump(ramBytes.data(), ramBytes.size(), offset);
                        }
                    }

                    offset += (RAM_SECTOR_SIZE / 2);
                    {
                        std::vector<uint8_t> ramBytes;

                        // Now write the RAM with the data to program
                        if ((error = isp.readMemory(offset, (RAM_SECTOR_SIZE / 2), ramBytes)))
                        {
                            LOG(ERROR) << "Error in writing memory: " << error;
                            break;
                        }
                        else
                        {
                            for (unsigned ii = 0; ii < ramBytes.size(); ++ii)
                                memBlock[ offset + ii ] = ramBytes.data()[ ii ];

                            if (gIsVerbose)
                                isp::Utility::hexDump(ramBytes.data(), ramBytes.size(), offset);
                        }
                    }
                }

                if (error != isp::ISP::ERR_ISP_NO_ERROR)
                    break;

            } while (false);
        }

        for (unsigned ii = gStartAddress; ii < gEndAddress; ++ii)
        {
            if (gMemory[ ii ] != memBlock[ ii ])
            {
                LOG(ERROR) << "Mismatch at address 0x"
                           << std::setw(8) << std::setfill('0') << std::hex << ii;
                error = isp::ISP::ERR_ISP_COMPARE_ERROR;
                break;
            }
        }

        if (error == isp::ISP::ERR_ISP_NO_ERROR)
            LOG(INFO) << "Verify success!";

        if (memBlock)
        {
            delete[] memBlock;
            memBlock = nullptr;
        }

    } while (false);

    LOG(INFO) << "Leaving " << __func__ << "(): errorCode is " << error;

    isp.applicationMode();
    return error;
}

