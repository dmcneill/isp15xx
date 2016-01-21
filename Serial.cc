///
/// @file   Serial.cc
///
/// @date   20 Dec 2014
/// @author Don McNeill dmcneill@me.com
///


//  Includes
#include <unistd.h>
#include "Serial.hh"
#include "Log.hh"
#include "Utility.hh"


//
//  @brief      Explicit constructor for the Serial class.
//
isp::Serial::Serial(const char * pDevice,
                    int controlFlags,
                    int inputFlags)
            : mError(0),
              mIsOpen(false),
              mFileDes(-1)
{
    do
    {
        //  First, open a file descriptor to the serial port
        mFileDes = open(pDevice, O_RDWR | O_NOCTTY | O_NONBLOCK);

        //  If the open fails, abort out...
        if (mFileDes < 0)
        {
            mError = -errno;
            break;
        }

        // Flush out the old tty
        tcflush(mFileDes, TCOFLUSH);
        tcflush(mFileDes, TCIFLUSH);

        // Set the file descriptor for non-blocking
        fcntl(mFileDes, F_SETFL, fcntl(mFileDes, F_GETFL) & ~O_NONBLOCK);

        //  Save the old terminal settings.
        tcgetattr(mFileDes, &mOldSettings) ;

        //  Setup new termIO attributes...
        bzero(&mNewSettings, sizeof(mNewSettings));
        mNewSettings.c_cflag = controlFlags;        // Baud rate
        mNewSettings.c_cflag |= CS8;                // 8-bit data
        mNewSettings.c_cflag |= CLOCAL;             // Ignore modem control lines
        mNewSettings.c_cflag |= CREAD;              // Enable receiver

        mNewSettings.c_iflag |= IGNBRK;             // Ignore break
        mNewSettings.c_iflag |= IGNPAR;             // Ignore parity
        mNewSettings.c_iflag |= (IXON | IXOFF);     // Use XON / XOFF for flow control

        mNewSettings.c_oflag = 0;                   // No post processing
        mNewSettings.c_lflag = 0;

        cfmakeraw(&mNewSettings);

        //  Non-canonical mode- read will wait until VMIN characters
        //  can be read and then return that number of characters.  Zero
        //  is returned on EOF.
        mNewSettings.c_cc[VTIME] = 1;
        mNewSettings.c_cc[VMIN]  = 0;

        //  Flush the Receiver Queue on the terminal
        tcflush(mFileDes, TCIFLUSH);

        //  Set the terminal attributes to be implemented
        //  IMMEDIATELY
        tcsetattr(mFileDes, TCSANOW, &mNewSettings);
        mIsOpen = true;
    } while (false);
}


//
//  @brief      Default destructor for the Serial class.
//
isp::Serial::~Serial()
{
    //  See if we are open...
    if (mIsOpen)
    {
        //  Flush the Receiver Queue on the terminal
        tcflush(mFileDes, TCIFLUSH);

        //  Reset the terminal attributes back to the original
        tcsetattr(mFileDes, TCSANOW, &mOldSettings);

        //  Close the file descriptor and kill the signal
        mIsOpen = false;
        close(mFileDes);
        mFileDes = -1;
    }
}


//
//  @brief      Read an input buffer from the Serial port.
//
ssize_t isp::Serial::read(char * buffer,
                          size_t size,
                          unsigned timeInMS,
                          unsigned& readTime)
{
    ssize_t result = -1;
    ssize_t bytesRead = 0;
    char *  pBuffer = buffer;

    do
    {
        if (!mIsOpen)
            break;

        if (!pBuffer)
            break;

        if (size == 0)
            break;

        fd_set fdSet;
        unsigned timeout = timeInMS;
        readTime = 0U;

        while (timeout)
        {
            FD_ZERO(&fdSet);
            FD_SET(mFileDes, &fdSet);
            struct timeval tv = { 0U, 1000U };

            result = select(mFileDes + 1, &fdSet, NULL, NULL, &tv);
            if (result > 0)
            {
                if (FD_ISSET(mFileDes, &fdSet))
                {
                    result = ::read(mFileDes, pBuffer, size);
                    if (result < 0)
                    {
                        mError = -errno;
                        return 0;
                    }
                    else if (result > 0)
                    {
                        if (!readTime)
                        {
                            readTime = timeout;
                        }
                        pBuffer += result;
                        size -= result;
                        bytesRead += result;
                        timeout = timeInMS;
                    }
                    else
                    {
                        break;
                    }
                }
            }
            else if (result < 0 )
            {
                // Error
                mError = -errno;
                break;
            }
            else if (result == 0)
            {
                // Timeout
                timeout--;
            }
        }
    } while (false);

    if (!result && bytesRead)
        result = bytesRead;

    return result;
}


//
//  @brief      Read an input string from the Serial port.
//
ssize_t isp::Serial::read(std::string& str,
                          unsigned timeoutInMS,
                          unsigned& readTime,
                          bool isVerbose)
{
    ssize_t result = -1;
    ssize_t bytesRead = 0;
    size_t  size = isp::Serial::RxBufferSize;

    do
    {
        if (!mIsOpen)
            break;

        if (size == 0)
            break;

        char * pBuffer = new char[ size + 1 ];
        if (!pBuffer)
        {
            mError = -ENOMEM;
            break;
        }

        do
        {
            result = read(pBuffer, size, timeoutInMS, readTime);
            if (result > 0 )
            {
                LOG(TRACE) << "Result: " << result  << "  Read time: " << readTime
                           << " ms  timeout: " << timeoutInMS << " ms";
                if (isVerbose)
                    Utility::hexDump(reinterpret_cast<const uint8_t *>(pBuffer), result);
                pBuffer[result] = '\0';
                str += pBuffer;
                pBuffer[0] = '\0';
                bytesRead += result;
            }
        } while (result > 0);

        delete[] pBuffer;
        pBuffer = NULL;

    } while (false);

    return bytesRead;
}


//
//  @brief      Read an input byte-vector from the Serial port.
//
ssize_t isp::Serial::read(std::vector<uint8_t>& bVector,
                          unsigned timeoutInMS,
                          unsigned& readTime,
                          bool isVerbose )
{
    ssize_t result = -1;
    ssize_t bytesRead = 0;
    size_t  size = isp::Serial::RxBufferSize;

    do
    {
        if (!mIsOpen)
            break;

        if (size == 0)
            break;

        char * pBuffer = new char[ size + 1 ];
        if (!pBuffer)
        {
            mError = -ENOMEM;
            break;
        }

        do
        {
            result = read(pBuffer, size, timeoutInMS, readTime);
            if (result > 0 )
            {

                LOG(TRACE) << "Result: " << result  << "  Read time: " << readTime
                           << " ms  timeout: " << timeoutInMS << " ms";
                if (isVerbose)
                    Utility::hexDump(reinterpret_cast<const uint8_t *>(pBuffer), result);
                pBuffer[result] = '\0';
                for (int ii = 0; ii < result; ++ii)
                {
                    bVector.push_back(static_cast<uint8_t>(pBuffer[ii]));
                }
                bytesRead += result;
            }
        } while (result > 0);

        delete[] pBuffer;
        pBuffer = NULL;

    } while (false);

    return bytesRead;
}


//
//  @brief      Write an output buffer to the Serial port.
//
ssize_t isp::Serial::write(const char * pBuffer, size_t size)
{
    ssize_t result = -1;

    do
    {
        if (!mIsOpen)
            break;

        if (!pBuffer)
            break;

        if (size == 0)
            break;

        result = ::write(mFileDes, pBuffer, size);
        if (result < 0)
        {
            mError = -errno;
        }
    } while (false);

    return result;
}


//
//  @brief      Write an output string to the Serial port.
//
ssize_t isp::Serial::write(const std::string& str)
{
    ssize_t result = -1;

    do
    {
        if (!mIsOpen)
            break;

        const char * pBuffer = str.c_str();

        result = ::write(mFileDes, pBuffer, str.length());
        if (result < 0)
        {
            mError = -errno;
        }
    } while (false);

    return result;
}


//
//  @brief      Write an output byte-vector to the Serial port.
//
ssize_t isp::Serial::write(const std::vector<uint8_t>& vec)
{
    ssize_t result = -1;

    do
    {
        if (!mIsOpen)
            break;

        if (!vec.size())
            break;

        const uint8_t * pVector = vec.data();

        result = ::write(mFileDes, pVector, vec.size());
        if (result < 0)
        {
            mError = -errno;
        }
    } while (false);

    return result;
}


