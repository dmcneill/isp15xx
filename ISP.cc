///
/// @file   ISP.cc
///
/// @date   28 Dec 2014
/// @author Don McNeill dmcneill@me.com
///

//  Includes
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include "ISP.hh"
#include "Log.hh"
#include "Utility.hh"


//  Definitions
#define RESET   "/sys/class/gpio/gpio17/value"
#define ISP0    "/sys/class/gpio/gpio18/value"
#define ISP1    "/sys/class/gpio/gpio27/value"


//  External References
extern  bool    gQuit;
extern  bool    gIsVerbose;
extern  bool    gNoGPIO;


///
/// @brief      Explicit constructor for the ISP class.
///
isp::ISP::ISP(isp::Serial& serial,
              bool isActiveLowReset,
              bool isVerbose)
        : mSerial(serial),
          mIsActiveLowReset(isActiveLowReset),
          mIsVerbose(isVerbose),
          mIsEcho(true)
{}


//
//  @brief      Enter ISP mode on the target.
//
void isp::ISP::programMode()
{
    if (gNoGPIO == false)
    {
        int rst  = isp::ISP::hwSignalOpen(RESET);
        int isp0 = isp::ISP::hwSignalOpen(ISP0);
        int isp1 = isp::ISP::hwSignalOpen(ISP1);

        // Assert ISP0,1 at LOW (Serial ISP )
        hwSignalSet(isp0, ISP0, false);
        hwSignalSet(isp1, ISP1, false);
        usleep(100 * 1000);

        // Assert reset active
        if (mIsActiveLowReset)
            hwSignalSet(rst, RESET, false);
        else
            hwSignalSet(rst, RESET, true);
        usleep(500 * 1000);

        // deassert reset inactive
        if (mIsActiveLowReset)
            hwSignalSet(rst, RESET, true);
        else
            hwSignalSet(rst, RESET, false);
        usleep(100 * 1000);

        // Assert ISP0,1 at HIGH
        hwSignalSet(isp0, ISP0, false);
        hwSignalSet(isp1, ISP1, false);

        isp::ISP::hwSignalClose(rst, RESET);
        isp::ISP::hwSignalClose(isp0, ISP0);
        isp::ISP::hwSignalClose(isp1, ISP1);
    }
    else
    {
        std::cout << "Put the board in ISP UART0 mode and press RESET:"
                  << std::endl;
        getchar();
    }
}


//
//  @brief      Enter Application mode on the target.
//
void isp::ISP::applicationMode()
{
    if (gNoGPIO == false)
    {
        int rst  = isp::ISP::hwSignalOpen(RESET);
        int isp0 = isp::ISP::hwSignalOpen(ISP0);
        int isp1 = isp::ISP::hwSignalOpen(ISP1);

        // Assert ISP0,1 at HIGH (Application mode )
        hwSignalSet(isp0, ISP0, true);
        hwSignalSet(isp1, ISP1, true);
        usleep(100 * 1000);

        // Assert reset at LOW
        if (mIsActiveLowReset)
            hwSignalSet(rst, RESET, false);
        else
            hwSignalSet(rst, RESET, true);
        usleep(500 * 1000);

        // deassert reset at HIGH
        if (mIsActiveLowReset)
            hwSignalSet(rst, RESET, true);
        else
            hwSignalSet(rst, RESET, false);
        usleep(100 * 1000);

        // Assert ISP0,1 at HIGH
        hwSignalSet(isp0, ISP0, true);
        hwSignalSet(isp1, ISP1, true);

        isp::ISP::hwSignalClose(rst, RESET);
        isp::ISP::hwSignalClose(isp0, ISP0);
        isp::ISP::hwSignalClose(isp1, ISP1);
    }
    else
    {
        std::cout << "Put the board in Applicaton mode and press RESET:"
                  << std::endl;

        getchar();
    }
}


//
//  @brief       Synchronize the serial port to the target.
//
isp::ISP::Error isp::ISP::synchronize(bool isVerbose)
{
    Error errorCode = ERR_ISP_TIMEOUT;

    do
    {
        ssize_t bytesRead = 0;

        if (gQuit == true)
            break;

        // First, clear out any residual read bytes
        if (mSerial.isOpen())
        {
            std::string nothing;
            unsigned readTime = 0U;

            mSerial.read(nothing, isp::ISP::MEDIUM_TIMEOUT, readTime);
        }

        // Send out the query '?' and attempt to synchronize
        {
            const char query[2] = "?";
            std::string answer;
            std::string test = "Synchronized\r\n";

            bytesRead = send(query, answer, test, 10, isVerbose);
            if (answer.find("Synchronized") != std::string::npos)
            {
                std::string tmp = "OK";

                bytesRead = send(test, answer, tmp, 20);
                if ((bytesRead == 0) || answer.find("OK") == std::string::npos)
                    break;
            }
            else
            {
                break;
            }
        }

        // Send out ESC
        {
            const char esc[2] = { 0x27, 0 };
            std::string answer;

            bytesRead = send(esc, answer, 10, isVerbose);
            if ((bytesRead == 0) || answer.find(esc) == std::string::npos)
                break;
        }

        // Do an ID query
        {
            std::string command = "J\r\n";
            std::string test = (mIsEcho? command + "0\r\n": "0\r\n");
            std::string answer;

            bytesRead = send(command, answer, test, 20, isVerbose);
            size_t pos = answer.find(test);

            if ((bytesRead > 0) && (pos != std::string::npos))
            {
                errorCode = ERR_ISP_NO_ERROR;
                std::string tmp = answer.substr(pos + test.length());
                mChipId = isp::Utility::trim(tmp);
            }
        }
    } while (false);

    return errorCode;
}


//
//  @brief      Set the baud rate and number of stop bits
//              for the target.
//
isp::ISP::Error isp::ISP::setBaudRate(unsigned baud,
                                      unsigned stopBits,
                                      unsigned timeoutInMS,
                                      bool isVerbose)
{
    Error errorCode = ERR_ISP_TIMEOUT;

    do
    {
        if (gQuit == true)
            break;

        std::string command = "B " + std::to_string(baud) + " " + std::to_string(stopBits) + "\r\n";
        std::string test = (mIsEcho? command: "");
        std::string answer;

        ssize_t bytesRead = send(command, answer, test, timeoutInMS, isVerbose);
        size_t pos = answer.find(test);

        if ((bytesRead > 0) && (pos != std::string::npos))
        {
            std::string tmp = answer.substr(pos + test.length());
            std::vector<std::string> results;

            isp::Utility::split(tmp, "\r\n", results);
            if (results.size() > 0)
            {
                errorCode = static_cast<Error>(
                        isp::Utility::stringToInt(results[0]));
            }

            if (errorCode == ERR_ISP_NO_ERROR)
            {
                LOG(INFO) << "Baud rate set to "
                          << baud
                          << " and number of stop bits is "
                          << stopBits;
            }
        }

    } while (false);

    return errorCode;
}


//
//  @brief      Get the chip identifier for the target.
//
isp::ISP::Error isp::ISP::queryId(uint32_t& chipId,
                                  unsigned timeoutInMS,
                                  bool isVerbose)
{
    Error errorCode = ERR_ISP_TIMEOUT;

    do
    {
        if (gQuit == true)
            break;

        std::string command = "J\r\n";
        std::string test = (mIsEcho? command: "");
        std::string answer;

        ssize_t bytesRead = send(command, answer, test, timeoutInMS, isVerbose);
        size_t pos = answer.find(test);

        if ((bytesRead > 0) && (pos != std::string::npos))
        {
            std::string tmp = answer.substr(pos + test.length());
            std::vector<std::string> results;

            isp::Utility::split(tmp, "\r\n", results);
            if (results.size() > 0)
            {
                errorCode = static_cast<Error>(
                        isp::Utility::stringToInt(results[0]));
            }

            if ((errorCode == ERR_ISP_NO_ERROR) && (results.size() > 1))
            {
                chipId = static_cast<uint32_t>(
                                    isp::Utility::stringToInt(results[1]));
                LOG(INFO) << "Device is 0x" << std::hex << chipId;
            }
        }

    } while (false);

    return errorCode;
}


//
//  @brief      Get the unique identifier for the target chip.
//
isp::ISP::Error isp::ISP::queryUID(std::vector<std::string>& vec,
                                   unsigned timeoutInMS,
                                   bool isVerbose)
{
    Error errorCode = ERR_ISP_TIMEOUT;

    do
    {
        if (gQuit == true)
            break;

        // Unique ID
        std::string command = "N\r\n";
        std::string test = (mIsEcho? command: "");
        std::string answer;

        ssize_t bytesRead = send(command, answer, test, timeoutInMS, isVerbose);
        size_t pos = answer.find(test);

        if ((bytesRead > 0) && (pos != std::string::npos))
        {
            std::string tmp = answer.substr(pos + test.length());
            std::vector<std::string> results;

            isp::Utility::split(tmp, "\r\n", results);
            if (results.size() > 0)
            {
                errorCode = static_cast<Error>(
                        isp::Utility::stringToInt(results[0]));
            }

            if (errorCode == ERR_ISP_NO_ERROR)
            {
                results.erase(results.begin());
                results.erase(results.end());
                vec = results;

                unsigned ii = 0;
                for (std::string& s : vec)
                    LOG(INFO) << "UID[" << ii++ << "] is " << s;
            }
        }

    } while (false);

    return errorCode;
}


//
//  @brief      Get the bootloader version for the target chip.
//
isp::ISP::Error isp::ISP::queryBootloaderVersion(std::vector<std::string>& vec,
                                                 unsigned timeoutInMS,
                                                 bool isVerbose)
{
    Error errorCode = ERR_ISP_TIMEOUT;

    do
    {
        if (gQuit == true)
            break;

        // Bootloader version
        std::string command = "K\r\n";
        std::string test = (mIsEcho? command: "");
        std::string answer;

        ssize_t bytesRead = send(command, answer, test, timeoutInMS, isVerbose);
        size_t pos = answer.find(test);

        if ((bytesRead > 0) && (pos != std::string::npos))
        {
            std::string tmp = answer.substr(pos + test.length());
            std::vector<std::string> results;

            isp::Utility::split(tmp, "\r\n", results);
            if (results.size() > 0)
            {
                errorCode = static_cast<Error>(
                        isp::Utility::stringToInt(results[0]));
            }

            if (errorCode == ERR_ISP_NO_ERROR)
            {
                results.erase(results.begin());
                results.erase(results.end());
                vec = results;

                for (std::string& s : vec)
                    LOG(INFO) << "version: " << s;
            }
        }

    } while (false);

    return errorCode;
}


//
//  @brief      Read the CRC checksum starting at the given address for the
//              given number of bytes.
//
isp::ISP::Error isp::ISP::queryCRC(uint32_t address,
                                   size_t size,
                                   uint32_t& crc,
                                   unsigned timeoutInMS,
                                   bool isVerbose)
{
    Error errorCode = ERR_ISP_TIMEOUT;

    do
    {
        if (gQuit == true)
            break;

        std::string command = "S " + std::to_string(address) + " " + std::to_string(size) + "\r\n";
        std::string test = (mIsEcho? command: "");
        std::string answer;

        ssize_t bytesRead = send(command, answer, test, timeoutInMS, isVerbose);
        size_t pos = answer.find(test);

        if ((bytesRead > 0) && (pos != std::string::npos))
        {
            std::string tmp = answer.substr(pos + test.length());
            std::vector<std::string> results;

            isp::Utility::split(tmp, "\r\n", results);
            if (results.size() > 0)
            {
                errorCode = static_cast<Error>(
                        isp::Utility::stringToInt(results[0]));
            }

            if ((errorCode == 0) && (results.size() > 1))
            {
                crc = static_cast<uint32_t>(
                        isp::Utility::stringToInt(results[1]));
                LOG(INFO) << "Checksum is 0x" << std::hex << crc;
            }
        }

        if (errorCode != isp::ISP::ERR_ISP_NO_ERROR)
        {
            if (isVerbose)
                LOG(ERROR) << "Error "
                           << errorCode
                           << " querying CRC";
        }

    } while (false);

    return errorCode;
}


//
//  @brief      Unlock flash for the target chip.
//
isp::ISP::Error isp::ISP::unlockFlash(unsigned timeoutInMS,
                                      bool isVerbose)
{
    Error errorCode = ERR_ISP_TIMEOUT;

    do
    {
        if (gQuit == true)
            break;

        // Unlock flash
        std::string command = "U 23130\r\n";
        std::string test = (mIsEcho? command: "");
        std::string answer;

        ssize_t bytesRead = send(command, answer, test, timeoutInMS, isVerbose);
        size_t pos = answer.find(test);

        if ((bytesRead > 0) && (pos != std::string::npos))
        {
            std::string tmp = answer.substr(pos + test.length());
            std::vector<std::string> results;

            isp::Utility::split(tmp, "\r\n", results);
            if (results.size() > 0)
            {
                errorCode = static_cast<Error>(
                        isp::Utility::stringToInt(results[0]));
            }
            else
            {
                LOG(ERROR) << "Error "
                           << errorCode
                           << " in unlocking flash";
            }
        }

    } while (false);

    return errorCode;
}


//
//  @brief      Prepare flash sectors for write operations.
//
isp::ISP::Error isp::ISP::prepareSectors(unsigned start,
                                         unsigned end,
                                         unsigned timeoutInMS,
                                         bool isVerbose)
{
    Error errorCode = ERR_ISP_TIMEOUT;

    do
    {
        if (gQuit == true)
            break;

        // Prepare sectors
        std::string command = "P " + std::to_string(start) + " " + std::to_string(end) + "\r\n";
        std::string test = (mIsEcho? command: "");
        std::string answer;

        ssize_t bytesRead = send(command, answer, test, timeoutInMS, isVerbose);
        size_t pos = answer.find(test);

        if ((bytesRead > 0) && (pos != std::string::npos))
        {
            std::string tmp = answer.substr(pos + test.length());
            std::vector<std::string> results;

            isp::Utility::split(tmp, "\r\n", results);
            if (results.size() > 0)
            {
                errorCode = static_cast<Error>(
                        isp::Utility::stringToInt(results[0]));
            }

            if (errorCode == ERR_ISP_NO_ERROR && isVerbose)
            {
                if (start == end)
                {
                    LOG(INFO) << "Sector " << start << " is prepared for write operations";
                }
                else
                {
                    LOG(INFO) << "Sectors " << start << " to " << end << " prepared for write operations";
                }
            }
            else
            {
                if (isVerbose)
                    LOG(ERROR) << "Error: "
                               << errorCode
                               << " preparing sectors for write";
            }
        }

    } while (false);

    return errorCode;
}


//
//  @brief      Erase flash sectors.
//
isp::ISP::Error isp::ISP::eraseSectors(unsigned start,
                                       unsigned end,
                                       unsigned timeoutInMS,
                                       bool isVerbose)
{
    Error errorCode = ERR_ISP_TIMEOUT;

    do
    {
        if (gQuit == true)
            break;

        // Erase sectors
        std::string command = "E " + std::to_string(start) + " " + std::to_string(end) + "\r\n";
        std::string test = (mIsEcho? command: "");
        std::string answer;

        ssize_t bytesRead = send(command, answer, test, timeoutInMS, isVerbose);
        size_t pos = answer.find(test);

        if ((bytesRead > 0) && (pos != std::string::npos))
        {
            std::string tmp = answer.substr(pos + test.length());
            std::vector<std::string> results;

            isp::Utility::split(tmp, "\r\n", results);
            if (results.size() > 0)
            {
                errorCode = static_cast<Error>(
                        isp::Utility::stringToInt(results[0]));
            }

            if (errorCode == ERR_ISP_NO_ERROR && isVerbose)
            {
                if (start == end)
                {
                    LOG(INFO) << "Sector " << start << " is erased";
                }
                else
                {
                    LOG(INFO) << "Sectors " << start << " to " << end << " erased";
                }
            }
            else
            {
                if (isVerbose)
                    LOG(ERROR) << "Error: "
                               << errorCode
                               << " performing sector erase";
            }
        }

    } while (false);

    return errorCode;
}


//
//  @brief      Blank check flash sector.
//
isp::ISP::Error isp::ISP::blankCheckSector(unsigned sector,
                                           std::vector<bool>& sectorMap,
                                           unsigned timeoutInMS,
                                           bool isVerbose)
{
    Error errorCode = ERR_ISP_TIMEOUT;

    do
    {
        if (gQuit == true)
            break;

        // Blank check sectors
        std::string command = "I " + std::to_string(sector) + " " + std::to_string(sector) + "\r\n";
        std::string test = (mIsEcho? command: "");
        std::string answer;

        ssize_t bytesRead = send(command, answer, test, timeoutInMS, isVerbose);
        size_t pos = answer.find(test);

        if ((bytesRead > 0) && (pos != std::string::npos))
        {
            std::string tmp = answer.substr(pos + test.length());
            std::vector<std::string> results;

            isp::Utility::split(tmp, "\r\n", results);
            if (results.size() > 0)
            {
                errorCode = static_cast<Error>(
                        isp::Utility::stringToInt(results[0]));
            }

            if (errorCode == ERR_ISP_NO_ERROR)
            {
                sectorMap[ sector ] = true;
            }
            else if(errorCode == ERR_ISP_SECTOR_NOT_BLANK)
            {
                sectorMap[ sector ] = false;
            }
            else
            {
                if (isVerbose)
                    LOG(ERROR) << "Error: "
                               << errorCode
                               << " performing blank check";
            }
        }

    } while (false);

    return errorCode;
}


//
//  @brief      Read memory from the target device.
//
isp::ISP::Error isp::ISP::readMemory(uint32_t address,
                                     size_t size,
                                     std::vector<uint8_t>& vec,
                                     unsigned timeoutInMS,
                                     bool isVerbose)
{
    Error errorCode = ERR_ISP_TIMEOUT;

    do
    {
        if (gQuit == true)
            break;

        // Read memory
        std::string command = "R " + std::to_string(address) + " " + std::to_string(size) + "\r\n";
        std::string test = (mIsEcho? command: "");
        ssize_t bytesRead = send(command, vec, test, timeoutInMS, isVerbose);

        if (bytesRead > 0)
        {
            // Convert the vector to a string
            std::string bytes = reinterpret_cast<const char *>(vec.data());
            size_t pos = bytes.find(test);

            // See if we found the command echoed
            if (pos != std::string::npos)
            {
                // Erase the command portion
                vec.erase(vec.begin(), vec.begin() + pos + test.length());
            }

            // Convert the vector to a string
            bytes = reinterpret_cast<const char *>(vec.data());
            pos = bytes.find("\r\n");

            // If we found the termination for the error code...
            if (pos != std::string::npos)
            {
                // Isolate the error code
                std::string error = bytes.substr(0, pos);
                errorCode = static_cast<Error>(
                    isp::Utility::stringToInt(error));

                // Remove the error code portion from the vector
                vec.erase(vec.begin(), vec.begin() + pos + 2);
            }
            else
            {
                errorCode = ERR_ISP_PARAM_ERROR;
                if (isVerbose)
                    LOG(ERROR) << "Error: "
                               << errorCode
                               << " readming memory";
            }
        }

    } while (false);

    return errorCode;
}


//
//  @brief      Enable or disable echo from the target.
//
isp::ISP::Error isp::ISP::echo(bool enable,
                               unsigned timeoutInMS,
                               bool isVerbose)
{
    Error errorCode = ERR_ISP_TIMEOUT;

    do
    {
        if (gQuit == true)
            break;

        // Echo on / off
        std::string command = (enable? "A 1\r\n" : "A 0\r\n");
        std::string test = (mIsEcho? command: "");
        std::string answer;

        ssize_t bytesRead = send(command, answer, test, timeoutInMS, isVerbose);
        size_t pos = answer.find(test);

        if ((bytesRead > 0) && (pos != std::string::npos))
        {
            std::string tmp = answer.substr(pos + test.length());
            std::vector<std::string> results;

            isp::Utility::split(tmp, "\r\n", results);
            if (results.size() > 0)
            {
                errorCode = static_cast<Error>(
                        isp::Utility::stringToInt(results[0]));
            }

            if (errorCode == ERR_ISP_NO_ERROR)
            {
                mIsEcho = enable;
            }
            else
            {
                LOG(ERROR) << "Error "
                           << errorCode
                           << " in setting Echo";
            }
        }

    } while (false);

    return errorCode;
}


//
//  @brief      Copy RAM to flash memory (program flash)
//
isp::ISP::Error isp::ISP::copyToFlash(uint32_t flash,
                                      uint32_t address,
                                      size_t size,
                                      unsigned timeoutInMS,
                                      bool isVerbose)
{
    Error errorCode = ERR_ISP_TIMEOUT;

    do
    {
        if (gQuit == true)
            break;

        // Write memory
        std::string command = "C " + std::to_string(flash)
                            + " " + std::to_string(address) +
                            + " " + std::to_string(size) + "\r\n";
        std::string test = (mIsEcho? command: "");
        std::string answer;

        ssize_t bytesRead = send(command, answer, test, timeoutInMS, isVerbose);
        size_t pos = answer.find(test);

        if ((bytesRead > 0) && (pos != std::string::npos))
        {
            std::string tmp = answer.substr(pos + test.length());
            std::vector<std::string> results;

            isp::Utility::split(tmp, "\r\n", results);
            if (results.size() > 0)
            {
                errorCode = static_cast<Error>(
                        isp::Utility::stringToInt(results[0]));
            }

            if (errorCode == ERR_ISP_NO_ERROR)
            {
                if (isVerbose)
                    LOG(INFO) << "Program flash at 0x"
                              << std::hex << std::setw(8) << std::setfill('0') << flash
                              << " From RAM at 0x"
                              << std::hex << std::setw(8) << std::setfill('0') << address
                              << " for " << size << " bytes";
            }
            else
            {
                LOG(ERROR) << "Error: "
                           << errorCode
                           << " programming flash from RAM";
            }
        }

    } while (false);

    return errorCode;
}


//
//  @brief      Execute starting at a given address.
//
isp::ISP::Error isp::ISP::execute(uint32_t address,
                                  unsigned timeoutInMS,
                                  bool isVerbose)
{
    Error errorCode = ERR_ISP_TIMEOUT;

    do
    {
        if (gQuit == true)
            break;

        // Write memory
        std::string command = "G " + std::to_string(address) + " T\r\n";
        std::string test = (mIsEcho? command: "");
        std::string answer;

        ssize_t bytesRead = send(command, answer, test, timeoutInMS, isVerbose);
        size_t pos = answer.find(test);

        if ((bytesRead > 0) && (pos != std::string::npos))
        {
            std::string tmp = answer.substr(pos + test.length());
            std::vector<std::string> results;

            isp::Utility::split(tmp, "\r\n", results);
            if (results.size() > 0)
            {
                errorCode = static_cast<Error>(
                        isp::Utility::stringToInt(results[0]));
            }

            if (errorCode == ERR_ISP_NO_ERROR)
            {
                if (isVerbose)
                    LOG(INFO) << "Execute from 0x"
                              << std::hex << std::setw(8)
                              << std::setfill('0') << address;
            }
            else
            {
                LOG(ERROR) << "Error: "
                           << errorCode
                           << " executing from RAM";
            }
        }

    } while (false);

    return errorCode;
}


//
//  @brief      Read memory from the target device.
//
isp::ISP::Error isp::ISP::writeMemory(uint32_t address,
                                      size_t size,
                                      std::vector<uint8_t>& vec,
                                      unsigned timeoutInMS,
                                      bool isVerbose)
{
    Error errorCode = ERR_ISP_TIMEOUT;

    do
    {
        if (gQuit == true)
            break;

        // Write memory
        std::string command = "W " + std::to_string(address) + " " + std::to_string(size) + "\r\n";
        std::string test = (mIsEcho? command: "");
        std::string answer;

        ssize_t bytesRead = send(command, answer, test, timeoutInMS, isVerbose);
        size_t pos = answer.find(test);

        if ((bytesRead > 0) && (pos != std::string::npos))
        {
            std::string tmp = answer.substr(pos + test.length());
            std::vector<std::string> results;

            isp::Utility::split(tmp, "\r\n", results);
            if (results.size() > 0)
            {
                errorCode = static_cast<Error>(
                        isp::Utility::stringToInt(results[0]));
            }

            if (errorCode == ERR_ISP_NO_ERROR)
            {
                // Now write out the data
                bytesRead = send(vec, isVerbose);
                if (bytesRead > 0)
                {
                    errorCode = ERR_ISP_NO_ERROR;
                    if (isVerbose)
                    {
                        LOG(INFO) << "Wrote "
                                  << vec.size()
                                  << " (passed "
                                  << size
                                  << ") bytes to address at 0x"
                                  << std::hex << std::setw(8) << std::setfill('0') << address;
                    }
                }
            }
            else
            {
                LOG(ERROR) << "Error: "
                           << errorCode
                           << " writing memory";
            }
        }

    } while (false);

    return errorCode;
}


//
//  @brief      Send a command to the serial interface and get a response.
//
ssize_t isp::ISP::send(const std::string& command,
                       std::string& response,
                       unsigned timeoutInMS,
                       bool isVerbose,
                       int retryCount)
{
    ssize_t bytesRead = -1;
    size_t size = command.length();
    int retry = retryCount;

    if (mSerial.isOpen())
    {
        while (--retry)
        {
            unsigned readTime = 0U;

            // Clear out the response
            response.clear();

            if (isVerbose)
                isp::Utility::hexDump(reinterpret_cast<const uint8_t *>(
                                                command.c_str()),
                                       size);

            // Send the command
            mSerial.write(command.c_str(), size);

            bytesRead = mSerial.read(response, timeoutInMS, readTime, isVerbose);
            if (bytesRead > 0)
            {
                if (isVerbose)
                    isp::Utility::hexDump(reinterpret_cast<const uint8_t *>(
                                                response.c_str()),
                                           response.length());
                return response.length();
            }
        }
        bytesRead = -2;
    }
    return bytesRead;
}


//
//  @brief      Send a command to the serial interface and get a response.
//
ssize_t isp::ISP::send(const std::string& command,
                       std::string& response,
                       std::string& testResponse,
                       unsigned timeoutInMS,
                       bool isVerbose,
                       int retryCount)
{
    ssize_t bytesRead = -1;
    size_t size = command.length();
    int retry = retryCount;

    if (mSerial.isOpen())
    {
        while (retry--)
        {
            unsigned readTime = 0U;

            // Clear out the response
            response.clear();

            if (isVerbose)
                isp::Utility::hexDump(reinterpret_cast<const uint8_t *>(
                                            command.c_str()),
                                       size);

            // Send the command
            mSerial.write(command.c_str(), size);

            bytesRead = mSerial.read(response, timeoutInMS, readTime, isVerbose);
            if (bytesRead > 0)
            {
                if (isVerbose)
                    isp::Utility::hexDump(reinterpret_cast<const uint8_t *>(
                                                response.c_str()),
                                           response.length());

                if (response.find(testResponse) != std::string::npos)
                    return response.length();
            }
            bytesRead = -2;
        }
    }
    return bytesRead;
}


//
//  @brief      Send a command to the serial interface and get a response.
//
ssize_t isp::ISP::send(const std::string& command,
                       std::vector<uint8_t>& response,
                       std::string& testResponse,
                       unsigned timeoutInMS,
                       bool isVerbose,
                       int retryCount)
{
    ssize_t bytesRead = -1;
    size_t size = command.length();
    int retry = retryCount;

    if (mSerial.isOpen())
    {
        while (--retry)
        {
            unsigned readTime = 0U;

            // Clear out the response
            response.clear();

            if (isVerbose)
                isp::Utility::hexDump(reinterpret_cast<const uint8_t *>(
                                            command.c_str()),
                                       size);

            // Send the command
            mSerial.write(command.c_str(), size);

            bytesRead = mSerial.read(response, timeoutInMS, readTime, isVerbose);
            if (bytesRead > 0)
            {
                if (isVerbose)
                    isp::Utility::hexDump(reinterpret_cast<const uint8_t *>(
                                                response.data()),
                                           response.size());

                if (strstr(reinterpret_cast<const char *>(response.data()),
                             testResponse.c_str()))
                    return response.size();
            }
            bytesRead = -2;
        }
    }
    return bytesRead;
}


//
//  @brief      Send a set of bytes from a vector to the serial interface
//
ssize_t isp::ISP::send(std::vector<uint8_t>& bytes,
                       bool isVerbose)
{
    ssize_t bytesRead = -1;
    size_t size = bytes.size();

    if (mSerial.isOpen())
    {
        if (isVerbose)
            isp::Utility::hexDump(bytes.data(), size);

        // Send the command
        mSerial.write(reinterpret_cast<const char *>(bytes.data()), size);

        bytesRead = size;
    }
    return bytesRead;
}


//
//  @brief      Open up a HW signal for access.
//
int isp::ISP::hwSignalOpen(const char * signal)
{
    int fd = open(signal, O_WRONLY);

    if (fd < 0)
    {
        LOG(ERROR) << "Open failed for signal '"
                   << signal
                   << "' -- " << errno << " " << strerror(errno);
    }
    return fd;
}


//
//  @brief      Close down a HW signal from access.
//
void isp::ISP::hwSignalClose(int fd,
                             const char * signal)
{
    if (fd >= 0)
    {
        int result = close(fd);

        if (result < 0)
        {
            LOG(ERROR) << "Close failed for signal '"
                       << signal
                       << "' -- " << errno << " " << strerror(errno);
        }
    }
}


//
//  @brief      Set a signal to a given state.
//
void isp::ISP::hwSignalSet(int fd,
                           const char * signal,
                           bool value)
{
    if (fd >= 0)
    {
        std::string vString = (value? "1\n": "0\n" );

        int result = ::write(fd, vString.c_str(), vString.size());
        if (result < 0)
        {
            LOG(ERROR) << "Write failed for signal '"
                       << signal
                       << "' -- " << errno << " " << strerror(errno);

        }
    }
}
