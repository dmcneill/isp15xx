///
/// @file   LED.cc
///
/// @date   06 Sep 2015
/// @author Don McNeill dmcneill@me.com
///

//  Includes
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "LED.hh"
#include "Log.hh"
#include "Utility.hh"

#include "Serial.hh"

//  Definitions
#define LED_SIGNAL "/sys/class/gpio/gpio25/value"


//  External References
extern  bool    gNoGPIO;


///
/// @brief      Default constructor for the LED class.
///
isp::LED::LED()
        : mFileDes(-1),
          mCycle(8U),
          mCounter(0U)
{
    if (gNoGPIO != true)
    {
        mFileDes = hwSignalOpen(LED_SIGNAL);
        if (mFileDes < 0)
        {
            LOG(ERROR) << "Cannot open LED signal";
            return;
        }
        hwSignalSet(mFileDes, LED_SIGNAL, false);
    }
}


///
/// @brief      Default destructor for the LED class.
///
isp::LED::~LED()
{
    if (gNoGPIO != true)
    {
        hwSignalSet(mFileDes, LED_SIGNAL, true);
        hwSignalClose(mFileDes, LED_SIGNAL);
    }
}


///
/// @brief      Default destructor for the LED class.
///
void isp::LED::Set(bool value)
{
    if (gNoGPIO != true)
    {
        hwSignalSet(mFileDes, LED_SIGNAL, value);
    }
}


//
//  @brief      Open up a HW signal for access.
//
int isp::LED::hwSignalOpen(const char * signal)
{
    int fd = -1;

    if (gNoGPIO != true)
    {
        fd = open(signal, O_WRONLY);

        if (fd < 0)
        {
            LOG(ERROR) << "Open failed for signal '"
                       << signal
                       << "' -- " << errno << " " << strerror(errno);
        }
    }
    return fd;
}


//
//  @brief      Close down a HW signal from access.
//
void isp::LED::hwSignalClose(int fd,
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
void isp::LED::hwSignalSet(int fd,
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



//
//  @brief      Cycle the LED.
//
void isp::LED::cycle()
{
    isp::LED::mCounter++;

    if (isp::LED::mCounter == isp::LED::mCycle)
        isp::LED::mCounter = 0;

    if (mCounter < isp::LED::mCycle / 2)
        Set(true);
    else
        Set(false);
} 
