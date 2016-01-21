///
/// @file   Main.cc
///
/// @date   20 Dec 2014
/// @author Don McNeill dmcneill@me.com
///


//  Includes
#include <getopt.h>
#include <iomanip>
#include <iostream>
#include <thread>
#include <future>
#include "Alarm.hh"
#include "Binary.hh"
#include "Client.hh"
#include "CmdLine.hh"
#include "Elf32.hh"
#include "iHex.hh"
#include "ISP.hh"
#include "LED.hh"
#include "Log.hh"
#include "Serial.hh"
#include "Signal.hh"
#include "Types.hh"
#include "Utility.hh"


//  Type definitions
#define NO_OPTION       (0)
#define ERASE_OPTION    (1)
#define PROGRAM_OPTION  (2)
#define EXAMINE_OPTION  (4)


//  Global variables
int         gOption             = NO_OPTION;
bool        gIsVerbose          = false;
bool        gIsActiveLowReset   = true;
bool        gQuit               = false;
bool        gNoGPIO             = false;
uint32_t    gStartAddress       = 0U;
uint32_t    gEndAddress         = 0U;
uint32_t    gStartSector        = 0U;
uint32_t    gEndSector          = 0U;
unsigned    gSyncRetries        = 2;
uint8_t     gMemory[ 512 * 1024 ];

//  Static variables
static  std::string gInputFilename;
static  std::string gSerialDevice;
static  isp::LED *  gLEDPtr;

///
/// @brief      File worker static method.
///
/// @details    Create the file worker thread to decode the Intel Hex file.
///
/// @param[in]  filename
///             The filename for the Intel Hex file.
///
/// @retval     0           Success.
/// @retval     <other>     Error.
///
static int fileWorker(
               const char * filename )
{
    std::string filenameStr = (filename? filename: "");
    int         result = 1;

    LOG(INFO) << "Entering fileWorker...";
    do
    {
        std::string fileExtension = isp::Utility::ExtractFileExtension(filenameStr);

        if (fileExtension == ".hex")
        {
            isp::iHex    intelHexFile(filenameStr.c_str(), gMemory, sizeof(gMemory));

            if (intelHexFile.parse())
            {
                gStartAddress =  intelHexFile.getStartAddress();
                gEndAddress   =  intelHexFile.getEndAddress();
                gStartSector  = (intelHexFile.getStartAddress() / FLASH_SECTOR_SIZE);
                gEndSector    = (intelHexFile.getEndAddress()   / FLASH_SECTOR_SIZE);
                LOG(INFO) << "Sectors: start="
                          << std::dec << gStartSector
                          << " End:="
                          << std::dec << gEndSector
                          << " count="
                          << std::dec << (gEndSector - gStartSector + 1);
                result = 0;
            }
        }
        else if (fileExtension == ".axf" || fileExtension == ".elf")
        {
            isp::Elf32  elf(filenameStr, gMemory, sizeof(gMemory));

            if (elf.read() && elf.parse(true, gIsVerbose))
            {
                gStartAddress =  elf.getStartAddress();
                gEndAddress   =  elf.getEndAddress();
                gStartSector  = (elf.getStartAddress() / FLASH_SECTOR_SIZE);
                gEndSector    = (elf.getEndAddress()   / FLASH_SECTOR_SIZE);
                LOG(INFO) << "Sectors: start="
                          << std::dec << gStartSector
                          << " End:="
                          << std::dec << gEndSector
                          << " count="
                          << std::dec << (gEndSector - gStartSector + 1);
                result = 0;
            }
        }
        else if (fileExtension == ".bin")
        {
            isp::Binary binary(filenameStr, gMemory, sizeof(gMemory));

            if (binary.read() && binary.parse(true, gIsVerbose))
            {
                gStartAddress =  binary.getStartAddress();
                gEndAddress   =  binary.getEndAddress();
                gStartSector  = (binary.getStartAddress() / FLASH_SECTOR_SIZE);
                gEndSector    = (binary.getEndAddress()   / FLASH_SECTOR_SIZE);
                LOG(INFO) << "Sectors: start="
                          << std::dec << gStartSector
                          << " End:="
                          << std::dec << gEndSector
                          << " count="
                          << std::dec << (gEndSector - gStartSector + 1);
                result = 0;
            }
        }
    } while (false);

    LOG(INFO) << "Leaving fileWorker: result is " << result;
    return result;
}


///
/// @brief      Erase client worker static method.
///
/// @details    Create the client-side instance to implement ISP client
///             functionality.  This method will initiate a conversation with
///             the remote ISP.
///
/// @param[in]  device
///             The device for the serial port.
///
/// @retval     0           Success.
/// @retval     <other>     Error.
///
static int eraseWorker(const char * device)
{
    int result = -1;

    do
    {
        isp::ISP::Error error = isp::eraseClient(device, gSyncRetries);
        result = static_cast<int>(error);

    } while (false);

    LOG(INFO) << "Leaving eraseWorker: result is " << result;
    return result;
}




///
/// @brief      Examine memory client worker static method.
///
/// @details    Create the client-side instance to implement ISP client
///             functionality.  This method will initiate a conversation with
///             the remote ISP.
///
/// @param[in]  device
///             The device for the serial port.
///
/// @retval     0           Success.
/// @retval     <other>     Error.
///
static int examineWorker(const char * device)
{
    int result = -1;

    do
    {
        isp::ISP::Error error = isp::examineClient(device, gSyncRetries);
        result = static_cast<int>(error);

    } while (false);

    LOG(INFO) << "Leaving examineWorker: result is " << result;
    return result;
}


///
/// @brief      Client worker static method.
///
/// @details    Create the client-side instance to implement ISP client
///             functionality.  This method will initiate a conversation with
///             the remote ISP.
///
/// @param[in]  device
///             The device for the serial port.
///
/// @retval     0           Success.
/// @retval     <other>     Error.
///
static int clientWorker(const char * device)
{
    int result = -1;

    do
    {
        isp::ISP::Error error = isp::programClient(device, gSyncRetries);
        result = static_cast<int>(error);

    } while (false);

    LOG(INFO) << "Leaving clientWorker: result is " << result;
    return result;
}


///
/// @brief      Handler for SIGALRM.
///
/// @details    This method handles periodic timed intervals through the
///             SIGALRM signal,
///
/// @param[in]  event       The signal number for the alarm.
///
void alarmHandler(int event)
{
    // Check the event code
    switch (event)
    {
        case SIGALRM:
            break;

        default:
            break;
    }
}


///
/// @brief      Handler for SIGINT and SIGTERM.
///
/// @details    This method controls the handling of the SIGINT and SIGTERM
///             signals and allows an orderly shutdown of the system.
///
/// @param[in]  event       The signal number; SIGINT or SIGTERM.
///
void termHandler(int event)
{
    switch(event)
    {
        case SIGINT:
        case SIGTERM:
            // Termination request - set the flag
            if (gIsVerbose)
            {
               LOG(INFO) << "Signal "
                         << event
                         << " hit (Termination)";
            }
            if (false == gQuit)
            {
                gQuit = true;
            }
            break;

        default:
            break;
    }
}


///
/// @brief      Process command line aruments.
///
/// @details    Process the command line options for the application.
///
/// @param[in]  argc    Number of command line arguments, including
///                     the invoking program name.
/// @param[in]  argv    List of constant c-strings for each argument,
///                     starting with the program name itself at the
///                     index of zero.
/// @param[out] error   The error code that is changed on the occurence
///                     of an error.
///
static void doCommandLine(
                int argc,
                char * const argv[],
                isp::tClientErrors& error )
{
    do
    {
        isp::CmdLine    cmdLine(argc, argv);
        std::string     argument;
        size_t          index = -1;

        if (cmdLine.find("--device", index) ||
            cmdLine.find("-d", index))
        {
            if (!cmdLine.get(index + 1, argument))
            {
                std::cerr << "No device argument found!"
                          << std::endl;

                error = isp::ISP_INVALID_ARGUMENT;
                break;
            }
            else
            {
                gSerialDevice = argument;
                index = -1;
            }
        }

        if (cmdLine.find("--filename", index) ||
            cmdLine.find("-f", index))
        {
            if (!cmdLine.get(index + 1, argument))
            {
                std::cerr << "No filename argument found!"
                          << std::endl;

                error = isp::ISP_INVALID_ARGUMENT;
                break;
            }
            else
            {
                gInputFilename = argument;
                index = -1;
            }
        }

        if (cmdLine.find("--nogoio", index) ||
            cmdLine.find("-g", index))
        {
            gNoGPIO = true;
            index = -1;
        }

        if (cmdLine.find("--help", index) ||
            cmdLine.find("-h", index))
        {
            error = isp::ISP_HELP_ARGUMENT;
            break;
        }

        if (cmdLine.find("--erase", index) ||
            cmdLine.find("-e", index))
        {
            gOption |= ERASE_OPTION;
            index = -1;
        }

        if (cmdLine.find("--program", index) ||
            cmdLine.find("-p", index))
        {
            gOption |= PROGRAM_OPTION;
            index = -1;
        }

        if (cmdLine.find("--reset", index) ||
            cmdLine.find("-r", index))
        {
            gIsActiveLowReset = false;
            index = -1;
        }

        if (cmdLine.find("--verbose", index) ||
            cmdLine.find("-v", index))
        {
            gIsVerbose = false;
            index = -1;
        }

        if (cmdLine.find("--examine", index) ||
            cmdLine.find("-x", index))
        {
            gOption |= EXAMINE_OPTION;
            index = -1;
        }
    } while (false);

    return;
}


///
/// @brief      Application entry point.
///
/// @details    The application entry point for the process to startup.
///
/// @param[in]  argc    Number of command line arguments, including
///                     the invoking program name.
/// @param[in]  argv    List of constant c-strings for each argument,
///                     starting with the program name itself at the
///                     index of zero.
///
/// @retval     0       Success.
/// @retval     <other> Unix-style error codes.
///
int main(int argc,
         char * const argv[] )
{
    isp::tClientErrors error = isp::ISP_NO_ERROR;
    int                returnCode = 0;

    // Process the command line arguments
    doCommandLine(argc, argv, error);

    // Check the required arguments
    if ((gOption == 0) && (error != isp::ISP_HELP_ARGUMENT ))
    {
        if ((gSerialDevice.length() == 0) || (gInputFilename.length() == 0))
            error = isp::ISP_INVALID_ARGUMENT;
    }
    else if ((gOption & PROGRAM_OPTION) ||
             (gOption & EXAMINE_OPTION))
    {
        if ((gSerialDevice.length() == 0) || (gInputFilename.length() == 0))
            error = isp::ISP_INVALID_ARGUMENT;
    }
    else if (gOption & ERASE_OPTION)
    {
        if (gSerialDevice.length() == 0)
            error = isp::ISP_INVALID_ARGUMENT;
    }

    // Test for any error
    if (error != isp::ISP_NO_ERROR)
    {
        switch (error)
        {
            case isp::ISP_HELP_ARGUMENT:
            {
                std::cerr << "ISP Client for LPC15xx Microcontroller"               << std::endl;
                std::cerr << "Copyright 2015 uControl, Inc.  All rights reserved."  << std::endl;
                std::cerr << ""                                                     << std::endl;
                std::cerr << "Usage:"                                               << std::endl;
                std::cerr << "isp15xx [OPTIONS] -p -d <device> -f <filename>"       << std::endl;
                std::cerr << "isp15xx [OPTIONS] --program -device=<device> ";
                std::cerr << "-filename=<filename>"                                 << std::endl;
                std::cerr << " where:"                                              << std::endl;
                std::cerr << "  --erase    | -e    Erase the flash"                 << std::endl;
                std::cerr << "  --program  | -p    Program the flash"               << std::endl;
                std::cerr << "  --device   | -d    Serial port device name"         << std::endl;
                std::cerr << "  --filename | -f    Intel Hex filename"              << std::endl;
                std::cerr << " OPTIONS:"                                            << std::endl;
                std::cerr << "  --reset    | -r    Mark reset as active HIGH"       << std::endl;
                std::cerr << "  --nogpio   | -g    Don't use GPIO for RST, ISP"     << std::endl;
                std::cerr << "  --verbose  | -v    Verbose messages"                << std::endl;
                std::cerr << "  --examine  | -x    Examine memory"                  << std::endl;
                std::cerr << "  --help     | -h    Show this help"                  << std::endl;
                exit(0);
            }
            break;

            case isp::ISP_INVALID_ARGUMENT:
            {
                LOG(ERROR) << "Invalid argument";
                exit(1);
            }
            break;

            default:
            {
                LOG(ERROR) << "<Unknown>";
                exit(1);
            }
            break;
        }
    }

    do
    {
        isp::Signal sigInt(SIGINT, termHandler);
        isp::Signal sigTerm(SIGTERM, termHandler);
        isp::Alarm  alarm(alarmHandler, 4U);
        isp::Signal sigPipe(SIGPIPE);
        bool        isDone = false;

        // Setup the LED output
        gLEDPtr = new isp::LED;

        if (gOption & ERASE_OPTION)
        {
            // Start the erase thread
            std::thread clientThread(eraseWorker, gSerialDevice.c_str());
            isDone = false;

            // Update the periodic interval
            while (gQuit == false && isDone == false)
            {
                if (clientThread.joinable())
                {
                    clientThread.join();
                    isDone = true;
                }
            }
        }

        if ((gOption & PROGRAM_OPTION) || (gOption & EXAMINE_OPTION))
        {
            // Start the file thread
            std::future<int> fileThread = std::async(fileWorker, gInputFilename.c_str());
            int fileWorkerStatus = fileThread.get();

            // Check the status of the file thread
            if (fileWorkerStatus != 0)
            {
                LOG(ERROR) << "Error return from file worker thread: "
                             << fileWorkerStatus;
                break;
            }
        }

        if (gOption & PROGRAM_OPTION)
        {
            // Start the program thread
            std::thread clientThread(clientWorker, gSerialDevice.c_str());
            isDone = false;

            // Update the periodic interval
            while (gQuit == false && isDone == false)
            {
                if (clientThread.joinable())
                {
                    clientThread.join();
                    isDone = true;
                }
            }
        }

        if (gOption & EXAMINE_OPTION)
        {
            // Start the program thread
            std::thread examineThread(examineWorker, gSerialDevice.c_str());
            isDone = false;

            // Update the periodic interval
            while (gQuit == false && isDone == false)
            {
                if (examineThread.joinable())
                {
                    examineThread.join();
                    isDone = true;
                }
            }
        }

        LOG (INFO) << "Tearing down...";

    } while (false);

    if (gLEDPtr)
    {
        delete gLEDPtr;
        gLEDPtr = nullptr;
    }

    if (gIsVerbose)
    {
        LOG(INFO) << "*** Calling exit("
                    << returnCode
                    << ")";
    }
    exit(returnCode);
}
