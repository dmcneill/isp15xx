///
/// @file   CmdLine.hh
///
/// @date   08 Sep 2015
/// @author Don McNeill dmcneill@me.com
///
#ifndef CMDLINE_HH_
#define CMDLINE_HH_

//  Includes
#include <vector>
#include <string>

//  Namespace
namespace isp {

///
/// @brief      CmdLine class for parsing command-line options.
///
/// @details    This class defines the methods for the implementation
///             of a command line parser.
///
class CmdLine
{
public:

    ///
    /// @brief      CmdLine explicit constructor.
    ///
    /// @param[in]  argc    The number of command line arguments to process.
    /// @param[in]  argv    The list of C-strings to examine.
    ///
    /// @details    Instantiate the CmdLine class for command-line parsing.
    ///
    CmdLine( int argc, char * const * argv);

    ///
    /// @brief      CmdLine destructor.
    ///
    /// @details    Teardown the CmdLine instance.
    ///
    virtual ~CmdLine();

    ///
    /// @brief      Determine if a specific option exists.
    ///
    /// @details    Scan the list of strings to determine if the option exists.
    ///
    /// @param[in]  input   A constant character string to search for in the
    ///                     list.
    ///
    /// @param[out] index   The relative index for the string that was found.
    ///
    /// @returns    Boolean true on success and false on error.
    ///
    bool find( const char * input, size_t& index );

    ///
    /// @brief      Get the nth string from the command line option list.
    ///
    /// @details    Retrives the nth item from the list of strings.
    ///
    /// @param[in]  index   The relative index for the string to be retrieved.
    ///
    /// @param[out] option  A reference to a string to be filled in if the
    ///                     index is in range.
    ///
    /// @returns    Boolean true on success and false on error.
    ///
    bool get( size_t index, std::string& option );

private:
    ///
    /// @brief      CmdLine copy constructor (non-copyable)
    ///
    /// @details    Make the class non-copyable.
    ///
    /// @param[in]  ref
    ///             Reference to a CmdLine instance to copy from.
    ///
    CmdLine( const CmdLine& ref ) = delete;

    ///
    /// @brief      CmdLine assignment operator (not-assignable)
    ///
    /// @details    Make the class non-assignable.
    ///
    /// @param[in]  ref
    ///             Reference to a CmdLine instance to copy from.
    ///
    /// @return     New instance for the left-hand side of the expression.
    ///
    CmdLine& operator = ( const CmdLine& ref ) = delete;

    // Data elements
    std::vector<std::string>    list;
};  // class

} // namespace
#endif
