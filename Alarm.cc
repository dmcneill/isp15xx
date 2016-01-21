///
/// @file   Alarm.hh
///
/// @date   20 Dec 2014
/// @author Don McNeill dmcneill@me.com
///

//  Includes
#include <iostream>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include "Alarm.hh"
#include "Log.hh"


//
//  @brief      Set a callback on a SIGALRM signal.
//
isp::Alarm::Alarm(
             tSigFunc callback,
             int timeInMS )
      : Signal(SIGALRM, callback),
        m_Seconds(0U),
        m_Milliseconds(timeInMS)
{
    if (callback)
    {
        itimerval TimerVal = { { 0, m_Milliseconds * 1000 }, { 0, m_Milliseconds * 1000 } };

        if (0 != setitimer(ITIMER_REAL,
                   (const itimerval *) &TimerVal,
                   NULL))
        {
            LOG(ERROR) << "*** Error setting alarm: "
                       << strerror(errno);
        }
    }
}


//
//  @brief      Set a callback on a SIGALRM signal.
//
isp::Alarm::Alarm(
             int seconds,
             tSigFunc callback )
      : Signal(SIGALRM, callback),
        m_Seconds(seconds),
        m_Milliseconds(0U)
{
    if (callback)
    {
        itimerval TimerVal = { { m_Seconds, 0 }, { m_Seconds, 0 } };

        if (0 != setitimer(ITIMER_REAL,
                   (const itimerval *) &TimerVal,
                   NULL))
        {
            LOG(ERROR) << "*** Error setting alarm: "
                       << strerror(errno);
        }
    }
}


//
//  @brief      Alarm destructor
//
isp::Alarm::~Alarm()
{
    itimerval TimerVal = { { 0, 0 }, { 0, 0 } };

    if (0 != setitimer(ITIMER_REAL,
               (const itimerval *) &TimerVal,
               NULL))
    {
        LOG(ERROR) << "*** Error resetting alarm: "
                   << strerror(errno);
    }
}

