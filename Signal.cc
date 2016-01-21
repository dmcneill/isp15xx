///
/// @file   Signal.cc
///
/// @date   20 Dec 2014
/// @author Don McNeill dmcneill@me.com
///


//  Includes
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include "Log.hh"
#include "Signal.hh"


//
//  @brief      Set a callback on a UNIX signal.
//
isp::Signal::Signal(
             int number,
             tSigFunc callback)
      : m_number(number),
        m_Func(callback)
{
    if (m_Func)
    {
        m_action.sa_handler = callback;
        sigemptyset(&m_action.sa_mask);
        m_action.sa_flags = 0;

        if (0 != sigaction(m_number, &m_action, NULL))
        {
            LOG(ERROR) << "*** Error setting signal: "
                       << m_number;
        }
    }
}


//
//  @brief      Set the default to handle the UNIX signal.
//
isp::Signal::Signal(
             int number)
      : m_number(number),
        m_Func(NULL)
{
    if (m_Func)
    {
        if (SIG_ERR == signal(m_number, SIG_DFL))
        {
            LOG(ERROR) << "*** Error resetting signal: "
                       << m_number;
        }
    }
}


//
//  @brief      Signal destructor
//
isp::Signal::~Signal()
{
    if (m_Func)
    {
        if (SIG_ERR == signal(m_number, SIG_DFL))
        {
            LOG(ERROR) << "*** Error resetting signal: "
                       << m_number;
        }
    }
}

