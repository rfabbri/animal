/*
 * -------------------------------------------------------------------------
 * $Revision: 1.2 $ $Date: 2005-07-23 14:01:31 $
 *
 * ANIMAL - AN IMAging Library
 * Copyright (C) 2002,2003-2011  Ricardo Fabbri
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * LICENSE NOTE: THIS FILE CONTAINS CODE DERIVED FROM THE GANDALF LIBRARY
 *
 * Modifications started in 27 may 2004 from Gandalf v. 1.3.2 found at
 * http://gandalf-library.sf.net
 *
 * All original Gandalf code was (c) 2000 Imagineer Software Limited and 
 *                                        Industrial Research Limited
 *
 * The authors of the original, non-modified code were:
 * Phil McLauchlan, Allan Jaenicke, and Phillip Ngan
 *
 * Those authors are NOT responsible for the changes found herein.
 * The Animal authors assume their own animal incompetence for what
 * worsened, and thank the original authors for inspiring the rest.
 * -------------------------------------------------------------------------
 */ 




/*
 * Description:        
 * 
 * Definitions:
 * library:     a collection of code that can be used by disparate applications
 * application: the code or program that utilises the library
 * error module:a collection of code for centralised error handling
 * private:     a function that is intended to be called from the library
 * public:      a function that is intended to be called from the application
 * error record: a struct holding error code, file name, line number,
 *              and text message for one error. 
 * error trace: a LIFO stack of error records, which allows temporary storage of
 *              error information until defered retrieval by application
 * top record:  the most recent error stored in trace
 * detection:   code that detects occurance of an error
 * handling:    action undertaken as a result of detecting an error. In library
 *              this typically involves registering the error and returning
 *              from current function with an error status. In application
 *              this typically involves invoking the reporter function.
 * register:    the process of placing an error into the trace
 * flushing:    the clearing of the error trace
 * reporter:    a function provided by the application to access error
 *              stored in trace and then communicating that information
 *              to the user or to a log. The reporter function must then
 *              flush the error trace.
 *
 *
 * Summary: The 'application' calls 'library' function A which calls
 * 'library' function B, which has an error that is
 * 'detected'. Function B 'flushes' the 'error trace' (because it is
 * the last function called that uses the facilities of the 'error
 * module', and then 'registers' the error details into the 'error
 * trace' and unwinds to function A with a return value that indicates
 * an error has occured. Function A tests the return value and
 * 'detects' the error and so 'registers' an error into the 'trace' and
 * unwinds to the 'application' with a return value that indicates an
 * error has occured. The 'application' tests the return value and
 * 'detects' that an error has occured so calls a facility in the
 * 'error module' to report the error. The error report function in
 * turn calls an 'application' supplied 'error reporter' function with
 * a pointer to the 'error trace' as an argument. The address of the
 * error trace is stored as a module scoped variable in the 'error
 * module'. The 'error reporter' accesses the information contained in
 * the 'error trace' using accessor functions and communicates the
 * error details to the user or to a log in some application specific
 * way.
 *
 * Intent: The purpose of this the error module is to provide a
 * mechanism by which generic reusable code (typically a library) can
 * report errors to a variety of applications without the need to
 * modify the library code for each new application context. That is,
 * the error reporting mechanism of the library is highly decoupled
 * from that of the application. Communication of error information
 * from library to application is performed using a small and well
 * defined interface.
 *
 * Approach: The role of the library is to communicate full and
 * unprocessed error information to the application. The role of the
 * application is to access the error information and report it to the
 * user. This demarcation of roles allows the application to use its
 * own error reporting mechanism, without any need to embed application
 * specific code in the library. The library achieves generality
 * because it plays no role in reporting the error information, which
 * usually requires system and application specific facilities.
 *
 * Specifically, the library writes (registers) error information into
 * a LIFO stack (error trace) which is built up as the error unwinds
 * through the nested calls. When the library function called by the
 * application finally returns -- with an error code -- the application
 * uses an error reporter to access the errors details and processes
 * that information in any way it chooses (e.g. displays an error
 * dialogue box, logs the error in a database).
 *
 * The library function at which a new error occurs must first flush
 * the error trace before registering the error.
 *
 * Consequences and liabilities:
 * (1) The application is able to:
 *        - control when errors are reported to the user interface 
 *          (the library should not itself report errors to the user)
 *        - provide its own error reporting mechanism 
 *          (to suit its own user interface).
 *        - extract sufficient information from the library to
 *          enable sufficient error reporting to be performed.
 * (2) The library can be used with many applications, without modification.
 * (3) Interactive resolution of errors occuring in library is problematic.
 *     Essentially the library is a black box to the application.
 *
 *
 * Usage notes for application writer: (see animal_err_example_app.c) No
 * code is needed to initialise the error trace. But a error reporting
 * function is optionally installed in the error module using
 * animal_err_set_reporter(). The reporter is an application function of
 * type Animal_ErrorReporterFunc, which is defined in animal_err.h. The
 * reporter must get the error count using animal_err_get_error_count()
 * and then sequentially access the errors stored in the trace using
 * animal_err_get_error(n), where n is the n-th error, and n=1 is the most
 * recent error. If no error reporter is installed, then the error
 * module provides a default reporter animal_err_default_reporter(), whose
 * action is to print the error details into stderr. The function
 * animal_err_set_reporter(ANIMAL_ERR_DFL) causes the default error reporter
 * to be used, and the call animal_err_set_reporter(ANIMAL_ERR_IGN) inhibits
 * the error reporter from being called. animal_err_set_reporter() returns
 * the address of the error reporter that was replaced so that it can
 * be reinstalled later.
 *
 * When the application tests the return value of a library function
 * and detects that an error has occured, it should call animal_err_report()
 * which invokes the error reporter.
 *
 * The application writer can choose not to buffer the error details in
 * a trace, but instead have the library function report errors
 * immediately, by automatically calling animal_err_report() inside
 * animal_err_register(). No error trace is built up. If the application calls
 * animal_err_report(), no errors are reported because the trace will be
 * empty. Usage of the trace is controlled by animal_err_set_trace() with
 * arguments ANIMAL_ERR_TRACE_ON or ANIMAL_ERR_TRACE_OFF.
 *
 * Usage notes for library writer: (see animal_err_example_lib.h) When a
 * error is detected at the deepest function call that uses the
 * facilities of the error module, then animal_err_flush_trace() should be
 * called, followed by animal_err_register(). As the subsequent library
 * function unwinds, they should call animal_err_register() (but not
 * animal_err_flush_trace()), and return with an error code. This
 * continues until the call stack unwinds into the applicaton.
 *
 * Multi-thread safe: A programmer attempting to use this module in a
 * multithreaded system must heed all precautions attendent with using
 * fully share memory address spaces.  To make this module multithread
 * safe, global locks must used to prevent concurrent access to the
 * error trace.
 *
 * PMN 2000-11-28 Created.
 * 
 */


#ifndef _ANIMAL_ERR_H
#define _ANIMAL_ERR_H

#include "../common.h"
#include "misc_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup Common
 * \{
 */

/**
 * \addtogroup CommonError
 * \{
 */

/**
 * \brief A type definition for an application supplied error handling function.
 *
 * Modelled after __kernel_signhandler_t in /usr/include/linux/posix/signal.h
 */
typedef void (*Animal_ErrorReporterFunc)(void);


/**
 *  Defines to specify error reporting behaviour.
 *  Used by animal_err_set_reporter().
 */
#define ANIMAL_ERR_DFL ((Animal_ErrorReporterFunc) 0) /* Set default error reporter*/
#define ANIMAL_ERR_IGN ((Animal_ErrorReporterFunc) 1) /* Ignore error reporting*/


/**
 * Default error codes used within the error module.
 */


#define ANIMAL_EC_OK    0
#define ANIMAL_EC_FAIL -1

#define ANIMAL_EC_BASE  0
/*----------------------------------------------------------------------*/
#define ANIMAL_EC_DFT (ANIMAL_EC_BASE+1000)        /* Default error codes */
#define ANIMAL_EC_DFT_DEEP_ERROR  (ANIMAL_EC_DFT +  1) /* Error in exception module*/
#define ANIMAL_EC_DFT_SPARE       (ANIMAL_EC_DFT +  2) /* Spare record -placeholder*/
#define ANIMAL_EC_DFT_BAD_N       (ANIMAL_EC_DFT +  3) /* animal_err_get_error(), */
                                                /* Index out of bounds*/
#define ANIMAL_EC_DFT_EMPTY       (ANIMAL_EC_DFT +  4) /* Empty trace, */
                                                /* animal_err_get_error(), */

/**
 * \brief Macro: Registers an error.
 * \param func_name    Name of function in which error occurs
 * \param err_code     Numeric code of error
 * \param message      Message string describing error
 * \return No value.
 *
 * This macro abbreviates animal_err_register_fileline() by hiding the file and
 * line argument. Refer to animal_err_register_fileline() for details on using
 * this macro.
 *
 * \sa animal_err_register_fileline().
 */
#ifdef ANIMAL_GENERATE_DOCUMENTATION
void animal_err_register ( const char *func_name, int err_code,
                        const char *message );
#else
#define animal_err_register(func_name, err_code, message) \
   animal_err_register_fileline(func_name, err_code, __FILE__, __LINE__, message)
#endif /* #ifdef ANIMAL_GENERATE_DOCUMENTATION */



/**
 * \brief Error trace on/off.
 *
 * A Trace is a data structure containing complete history of error information
 * collected from the time the initiating error occurs, to the time it
 * unwinds to the application.
 *
 * ANIMAL_ERR_TRACE_ON Indicates that a trace structure is built and that the
 *                  error details are transfered to the application using
 *                  animal_err_report().
 *                  The function animal_err_register() adds an error to the trace.
 *
 * ANIMAL_ERR_TRACE_OFF Indicates that no trace structure be constructed. Instead
 *                   error details are reported immediately during
 *                   animal_err_register().
 *
 */
typedef enum { ANIMAL_ERR_TRACE_OFF, ANIMAL_ERR_TRACE_ON } Animal_TraceMode;


AnimalExport Animal_ErrorReporterFunc
          animal_err_set_reporter(Animal_ErrorReporterFunc app_error_reporter),
          animal_err_get_reporter( void );
AnimalExport int 
   animal_err_register_fileline(
                 const char * func_name,
                 int          err_code,
                 const char * file_name, /* Typically use __FILE__ */
                 int          line_number, /* Typically use __LINE__ */
                 const char * message),
   animal_err_get_error_count(void),
   animal_err_get_error ( int           n,
                        const char ** func_name,
                        int         * err_code,
                        const char ** file_name,
                        int         * line_number,
                        const char ** message );
AnimalExport void 
   animal_err_report( void ),
   animal_err_set_trace( Animal_TraceMode trace_mode ),
   animal_err_flush_trace(void);


/* error test macros compiled out when NDEBUG is defined */

/**
 * \brief Macro: Tests expression, fails and invokes error handler if false.
 * \param test An expresion to test for true/false value
 * \param funcname The function name in which this test occurs
 * \param code An error code to set if the test fails
 * \param message An error message for if the test fails
 * \return No value.
 * If the test fails, the error handler module is invoked with the given
 * \a funcname, \a code and \a message, and #ANIMAL_FALSE is returned by the
 * calling function. If \c NDEBUG is defined, the test is ignored.
 *
 * \sa animal_err_assert_ptr().
 */
#ifdef ANIMAL_GENERATE_DOCUMENTATION
Animal_Bool animal_err_test_bool ( Animal_Bool test, char *funcname,
                             int code, char *message );
#else
#ifndef NDEBUG
#define animal_err_test_bool(test,fname,code,mes) \
if (!(test)) \
{ \
   animal_err_flush_trace(); \
   animal_err_register_fileline ( fname, code, __FILE__, __LINE__, mes ); \
   return ANIMAL_FALSE; \
}
#else
#define animal_err_test_bool(test,fname,code,mes) ((void)0)
#endif
#endif /* #ifdef ANIMAL_GENERATE_DOCUMENTATION */

/**
 * \brief Tests expression, fails and invokes error handler if false.
 * \param test An expresion to test for true/false value
 * \param funcname The function name in which this test occurs
 * \param code An error code to set if the test fails
 * \param message An error message for if the test fails
 * \return No value.
 *
 * If the test fails, the error handler module is invoked with the given
 * \a funcname, \a code and \a message, and -1 is returned by the calling
 * function. If \c NDEBUG is defined, the test is ignored.
 *
 * \sa animal_err_assert_ptr().
 */
#ifdef ANIMAL_GENERATE_DOCUMENTATION
int animal_err_test_int ( Animal_Bool test, char *funcname,
                       int code, char *message );
#else
#ifndef NDEBUG
#define animal_err_test_int(test,fname,code,mes) \
if (!(test)) \
{ \
   animal_err_flush_trace(); \
   animal_err_register_fileline ( fname, code, __FILE__, __LINE__, mes ); \
   return -1; \
}
#else
#define animal_err_test_int(test,fname,code,mes) ((void)0)
#endif
#endif /* #ifdef ANIMAL_GENERATE_DOCUMENTATION */

/**
 * \brief Macro: Tests expression, fails and invokes error handler if false.
 * \param test An expresion to test for true/false value
 * \param funcname The function name in which this test occurs
 * \param code An error code to set if the test fails
 * \param message An error message for if the test fails
 * \return No value.
 *
 * If the test fails, the error handler module is invoked with the given
 * \a funcname, \a code and \a message, and \c UINT_MAX is returned by the
 * calling function. If \c NDEBUG is defined, the test is ignored.
 *
 * \sa animal_err_assert_ptr().
 */
#ifdef ANIMAL_GENERATE_DOCUMENTATION
int animal_err_test_uint ( Animal_Bool test, char *funcname,
                        int code, char *message );
#else
#ifndef NDEBUG
#define animal_err_test_uint(test,fname,code,mes) \
if (!(test)) \
{ \
   animal_err_flush_trace(); \
   animal_err_register_fileline ( fname, code, __FILE__, __LINE__, mes ); \
   return UINT_MAX; \
}
#else
#define animal_err_test_uint(test,fname,code,mes) ((void)0)
#endif
#endif /* #ifdef ANIMAL_GENERATE_DOCUMENTATION */


/**
 * \brief Macro: Tests expression, fails and invokes error handler if false.
 * \param test An expresion to test for true/false value
 * \param funcname The function name in which this test occurs
 * \param code An error code to set if the test fails
 * \param message An error message for if the test fails
 * \return No value.
 *
 * If the test fails, the error handler module is invoked with the given
 * \a funcname, \a code and \a message, and \c NULL is returned by the calling
 * function. If \c NDEBUG is defined, the test is ignored.
 *
 * \sa animal_err_assert_bool().
 */
#ifdef ANIMAL_GENERATE_DOCUMENTATION
void *animal_err_test_ptr ( Animal_Bool test, char *funcname,
                         int code, char *message );
#else
#ifndef NDEBUG
#define animal_err_test_ptr(test,fname,code,mes) \
if (!(test)) \
{ \
   animal_err_flush_trace(); \
   animal_err_register_fileline ( fname, code, __FILE__, __LINE__, mes ); \
   return NULL; \
}
#else
#define animal_err_test_ptr(test,fname,code,mes) ((void)0)
#endif
#endif /* #ifdef ANIMAL_GENERATE_DOCUMENTATION */

/**
 * \brief Macro: Tests expression, fails and invokes error handler if false.
 * \param test An expresion to test for true/false value
 * \param funcname The function name in which this test occurs
 * \param code An error code to set if the test fails
 * \param message An error message for if the test fails
 * \return No value.
 *
 * If the test fails, the error handler module is invoked with the given
 * \a funcname, \a code and \a message, and \c DBL_MAX is returned by the
 * calling function. If \c NDEBUG is defined, the test is ignored.
 *
 * \sa animal_err_assert_ptr().
 */
#ifdef ANIMAL_GENERATE_DOCUMENTATION
double animal_err_test_double ( Animal_Bool test, char *funcname,
                             int code, char *message );
#else
#ifndef NDEBUG
#define animal_err_test_double(test,fname,code,mes) \
if (!(test)) \
{ \
   animal_err_flush_trace(); \
   animal_err_register_fileline ( fname, code, __FILE__, __LINE__, mes ); \
   return DBL_MAX; \
}
#else
#define animal_err_test_double(test,fname,code,mes) ((void)0)
#endif
#endif /* #ifdef ANIMAL_GENERATE_DOCUMENTATION */

/**
 * \brief Macro: Tests expression, fails and invokes error handler if false.
 * \param test An expresion to test for true/false value
 * \param funcname The function name in which this test occurs
 * \param code An error code to set if the test fails
 * \param message An error message for if the test fails
 * \return No value.
 *
 * If the test fails, the error handler module is invoked with the given
 * \a funcname, \a code and \a message, and \c FLT_MAX is returned by the
 * calling function. If \c NDEBUG is defined, the test is ignored.
 *
 * \sa animal_err_assert_ptr().
 */
#ifdef ANIMAL_GENERATE_DOCUMENTATION
float animal_err_test_float ( Animal_Bool test, char *funcname,
                           int code, char *message );
#else
#ifndef NDEBUG
#define animal_err_test_float(test,fname,code,mes) \
if (!(test)) \
{ \
   animal_err_flush_trace(); \
   animal_err_register_fileline ( fname, code, __FILE__, __LINE__, mes ); \
   return FLT_MAX; \
}
#else
#define animal_err_test_float(test,fname,code,mes) ((void)0)
#endif
#endif /* #ifdef ANIMAL_GENERATE_DOCUMENTATION */

/**
 * \}
 */

/**
 * \}
 */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _ANIMAL_ERR_H */
