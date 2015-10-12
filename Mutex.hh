///
/// @file   Mutex.hh
///
/// @date   20 Dec 2014
/// @author Don McNeill dmcneill@me.com
///
#ifndef MUTEX_HH_
#define MUTEX_HH_

//  Includes
#include <iostream>
#include <pthread.h>
#include "Log.hh"

//  Namespace
namespace isp {

///
/// @brief      Template class for Mutex Lock.
///
/// @details    This class contains the support methods necessary to implement
///             the ISP Mutex Lock interface to pthreads.
///
template <class T> class Lock
{
public:

    ///
    /// @brief      Lock constructor
    ///
    /// @details    Constructor that calls the template class
    ///             lock method.
    ///
    /// @param[in]  obj         Reference to the template class
    ///                         object of type T.
    ///
    Lock( T &obj ) : m_obj( obj ) { m_obj.lock(); }

    ///
    /// @brief      Lock destructor
    ///
    /// @details    Destructor that calls the template class
    ///             unlock method.
    ///
    ~Lock() { m_obj.unlock(); }

    ///
    /// @brief      Lock the lock
    ///
    /// @details    Method that calls the template data object
    ///             lock method.
    ///
    /// @return     Boolean true on success and false on error.
    ///
    bool lock() { return m_obj.lock(); }

    ///
    /// @brief      Unlock the lock
    ///
    /// @details    Method that calls the template data object
    ///             unlock method.
    ///
    /// @return     Boolean true on success and false on error.
    ///
    bool unlock() { return m_obj.unlock(); }

private:
    ///
    /// @brief      Default Lock constructor
    ///
    /// @details    NOT USED
    ///
    Lock() = delete;

    ///
    /// @brief      Copy constructor
    ///
    /// @details    Make the class non-copyable.
    ///
    /// @param[in]  rlock       Reference to the Lock Object
    ///                         to be copied.
    ///
    Lock( const Lock& rlock ) = delete;

    ///
    /// @brief      Assignment operator
    ///
    /// @details    Make the class non-assignable.
    ///
    /// @param[in]  rlock       Reference to the Lock Object
    ///                         to be copied.
    ///
    Lock& operator = ( const Lock& rlock ) = delete;

    ///
    /// @brief      Address of operator
    ///
    /// @details    Make the class non-assignable.
    ///
    Lock * operator & () { return this; }

    //  Data members
    T&  m_obj;
};


class Mutex
{
public:
    ///
    /// @brief      Mutex constructor
    ///
    /// @details    Instantiate a pthread mutex.
    ///
    Mutex();

    ///
    /// @brief      Mutex destructor
    ///
    /// @details    Teardown a pthread mutex.
    ///
    virtual ~Mutex();

    ///
    /// @brief      Lock the mutex
    ///
    /// @details    Method to lock the mutex to provide exclusive
    ///             access to an object.
    ///
    /// @return     Boolean true on success and false on error.
    ///
    bool lock();

    ///
    /// @brief      Test the lock on the mutex
    ///
    /// @details    Method to test the lock on mutex to provide
    ///             exclusive  access to an object.
    ///
    /// @return     Boolean true on success and false on error.
    ///
    bool trylock();

    ///
    /// @brief      Unlock the mutex
    ///
    /// @details    Method to unlock the mutex to restore global
    ///             access to an object.
    ///
    /// @return     Boolean true on success and false on error.
    ///
    bool unlock();

    ///
    /// @brief      Get a reference to the pthread mutex object.
    ///
    /// @details    Obtain a reference to the pthread mutex object
    ///             for use in other classes.
    ///
    /// @return     Pointer to the address of the pthread mutex
    ///             object.
    ///
    pthread_mutex_t * get() { return &m_Mutex; }

    ///
    /// @brief      Display the state.
    ///
    /// @details    Show the state of the mutex.
    ///
    ///
    void show() { LOG( INFO ) << "RefCount: " << m_RefCount; }

private:
    ///
    /// @brief      Copy constructor
    ///
    /// @details    Make the class non-copyable.
    ///
    /// @param[in]  mutex       Reference to the Mutex object
    ///                         to be copied.
    ///
    Mutex( const Mutex& mutex ) = delete;

    ///
    /// @brief      Assignment operator
    ///
    /// @details    Make the class non-assignable.
    ///
    /// @param[in]  mutex       Reference to the Mutex object
    ///                         to be copied.
    ///
    Mutex& operator = ( const Mutex& mutex ) = delete;

    //  Data members
    pthread_mutex_t m_Mutex;
    int             m_RefCount;
    int             m_Error;
};


class Condition
{
public:
    ///
    /// @brief      Condition explicit constructor
    ///
    /// @details    Instantiate a condition variable for use
    ///             with pthreads.
    ///
    /// @param[in]  value       Integer value for the condition
    ///                          setting -- defaults to zero.
    ///
    Condition(int value = 0);

    ///
    /// @brief      Condition destructor
    ///
    /// @details    Teardown a condition object.
    ///
    ~Condition();

    ///
    /// @brief      Copy constructor
    ///
    /// @details    Make the class copyable.
    ///
    /// @param[in]  cond        Reference to the Condition
    ///                         object to be copied.
    ///
    Condition( const Condition& cond );

    ///
    /// @brief      Assignment operator
    ///
    /// @details    Make the class assignable.
    ///
    /// @param[in]  cond        Reference to the Condition
    ///                         object to be copied.
    ///
    Condition& operator = ( const Condition& cond );

    ///
    /// @brief      Wait on a condition object.
    ///
    /// @details    Wait for a condition object to become valid.
    ///
    /// @param[in]  mutex       Reference to the mutex to use
    ///                         on the condition wait call -- this
    ///                         will lock the mutex until the
    ///                         condition is changed and the mutex
    ///                         is signalled.
    ///
    /// @return     Boolean true on success and false on error.
    ///
    bool wait( Mutex& mutex );

    ///
    /// @brief      Signal a condition object.
    ///
    /// @details    Signal the underlying mutex that the condition
    ///             variable is locked to.
    ///
    /// @return     Boolean true on success and false on error.
    ///
    bool signal();

    ///
    /// @brief      Set the condition value.
    ///
    /// @details    value       The value to set the condition to.
    ///
    void set(int value) { m_Value = value; }

    ///
    /// @brief      Get the condition object's current value.
    ///
    /// @return     Return the current value of the condition
    ///             variable.
    ///
    int get() { return m_Value; }

private:
    //  Data members
    pthread_cond_t m_Cond;
    int m_Value;
    int m_Error;

};  // class

} // namespace
#endif
