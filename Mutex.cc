///
/// @file   Mutex.cc
///
/// @date   20 Dec 2014
/// @author Don McNeill dmcneill@me.com
///


//  Includes
#include <iostream>
#include <string.h>
#include <pthread.h>
#include "Log.hh"
#include "Mutex.hh"


//
//  @brief      Mutex constructor
//
isp::Mutex::Mutex ()
      : m_RefCount(0),
        m_Error(0)
{
    int result = pthread_mutex_init(&m_Mutex, NULL);

    if (result)
    {
        m_Error = result;
        LOG(ERROR) << "Pthread mutex init failed: "
                   << strerror(result);
    }
}


//
//  @brief      Mutex destructor
//
isp::Mutex::~Mutex ()
{
    int result = pthread_mutex_destroy(&m_Mutex);

    if (result)
    {
        LOG(ERROR) << "Pthread mutex destroy failed: "
                   << "RefCount="
                   << m_RefCount
                   << ' '
                   << strerror(result);
    }
}


//
//  @brief      Lock the mutex
//
bool isp::Mutex::lock ()
{
    int result = pthread_mutex_lock(&m_Mutex);

    if (result)
    {
        LOG(ERROR) << "Pthread mutex lock failed: "
                   << strerror(result);
    }
    else
    {
        ++m_RefCount;
    }

    return (m_Error = result? false: true );
}


//
//  @brief      Test the lock on the mutex
//
bool isp::Mutex::trylock ()
{
    int result = pthread_mutex_trylock(&m_Mutex);

    if (result)
    {
        LOG(ERROR) << "Pthread mutex trylock failed: "
                   << strerror(result);
    }
    else
    {
        ++m_RefCount;
    }
    return (m_Error = result? false: true );
}


//
//  @brief      Unlock the mutex
//
bool isp::Mutex::unlock ()
{
    int result = pthread_mutex_unlock(&m_Mutex);

    if (result)
    {
        LOG(ERROR) << "Pthread mutex unlock failed: "
                   << strerror(result);
    }
    else
    {
        --m_RefCount;
    }

    return (m_Error = result? false: true );
}


//
//  @brief      Condition explicit constructor
//
isp::Condition::Condition(
            int value)
      : m_Value(value),
        m_Error(0)
{
    int result = pthread_cond_init(&m_Cond, NULL);

    if (result)
    {
        m_Error = result;
        LOG(ERROR) << "Pthread cond init failed: "
                   << strerror(result);
    }
}


//
//  @brief      Condition destructor
//
isp::Condition::~Condition()
{
    int result = pthread_cond_destroy(&m_Cond);

    if (result)
    {
        LOG(ERROR) << "Pthread cond destroy failed: "
                   << strerror(result);
    }
}


//
//  @brief      Wait on a condition object.
//
bool isp::Condition::wait(Mutex& mutex)
{
    int result = pthread_cond_wait(&m_Cond, mutex.get());

    if (result)
    {
        m_Error = result;
        LOG(ERROR) << "Pthread cond wait failed: "
                   << strerror(result);
    }
    return (m_Error = result? false: true );
}


//
//  @brief      Signal a condition object.
//
bool isp::Condition::signal()
{
    int result = pthread_cond_signal(&m_Cond);

    if (result)
    {
        m_Error = result;
        LOG(ERROR) << "Pthread cond signal failed: "
                   << strerror(result);
    }
    return (m_Error = result? false: true );
}


//
//  @brief      Copy constructor
//
isp::Condition::Condition(const isp::Condition& cond)
{
    this->m_Cond  = cond.m_Cond;
    this->m_Value = cond.m_Value;
    this->m_Error = cond.m_Error;
}


//
//  @brief      Assignment operator
//
isp::Condition& isp::Condition::operator = (const isp::Condition& cond)
{
    this->m_Cond  = cond.m_Cond;
    this->m_Value = cond.m_Value;
    this->m_Error = cond.m_Error;
    return *this;
}
