/*
 * -------------------------------------------------------------------------
 * $Revision: 1.1 $ $Date: 2005-07-20 19:15:48 $
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






#include <stdio.h>
#include "err_trace.h"
#include "err.h"

/**
 * \addtogroup Common
 * \{
 */

/**
 * \defgroup CommonError Error Handling
 * \{
 */

/*  Pointer to current error reporting function. This is setup using
 * animal_err_set_reporter(), and typically invoked using animal_err_report().
 *
 * Default reporter is animal_err_default_reporter().
 */
static Animal_ErrorReporterFunc
               animal_err_current_reporter = animal_err_default_reporter;


/* Flag indicating Trace Mode - see ANIMAL_ERR_TRACE_X in animal_exception.h for
 * details. Default action is to set trace off. Permitted values are:
 * ANIMAL_ERR_TRACE_OFF or ANIMAL_ERR_TRACE_ON.
 */
static Animal_TraceMode animal_err_trace_mode = ANIMAL_ERR_TRACE_OFF; 


/* The error trace */
/*  Statically allocate last and 2nd to last records for error trace */
static Animal_ErrorTrace record_last = { NULL, ANIMAL_ET_YES, ANIMAL_ET_NO,
                                      ANIMAL_ET_YES, NULL,
                                      ANIMAL_EC_DFT_SPARE, NULL, 0, NULL };
static Animal_ErrorTrace record_2nd_last = { &record_last,  ANIMAL_ET_YES, ANIMAL_ET_NO,
                                          ANIMAL_ET_YES, NULL,
                                          ANIMAL_EC_DFT_SPARE, NULL, 0, NULL };

/* Address of error trace (i.e. top of LIFO stack) */
static Animal_ErrorTrace * animal_err_trace_top = &record_2nd_last;

/**
 * \brief Installs an error reporter
 * \param app_error_reporter Pointer to an application defined function
 * \return Pointer to previously installed error reporter, if successful.
 *          \c NULL otherwise.
 *
 * This exception module allows an application defined function to be called
 * when an error is reported using animal_err_report().
 * animal_err_set_reporter() installs this error reporter.
 * \a app_error_reporter should be a pointer to an application defined function
 * to access error details, or the macros:
 *    - ANIMAL_ERR_DFL To set default error reporter
 *    - ANIMAL_ERR_IGN To ignore error reporting
 *
 * If ANIMAL_ERR_DFL is provided, then the default error reporter (see below) is
 * installed.
 * \warning The default error reporter simply writes a message to \c stderr.
 *          The exception module is initialised to use the default error
 *          reporter.
 * \sa animal_err_get_reporter().
 * \note Other functions that invoke the current error handler must check first
 *       if it is set to #ANIMAL_ERR_IGN. If so, ignore the invocation.
 *       animal_err_current_reporter() is module scope variable.
 */
AnimalExport Animal_ErrorReporterFunc animal_err_set_reporter(
                                    Animal_ErrorReporterFunc app_error_reporter )
{
   /*  Buffer old handler, so that it can be returned */
   Animal_ErrorReporterFunc animal_err_temp_reporter = animal_err_current_reporter; 
    
   /* Set reporter, noting default (ANIMAL_ERR_DFL) or ignore
      (ANIMAL_ERR_IGN) modes */
   if ( app_error_reporter == ANIMAL_ERR_DFL )
      animal_err_current_reporter = animal_err_default_reporter;
   else if ( app_error_reporter == ANIMAL_ERR_IGN )
      animal_err_current_reporter = ANIMAL_ERR_IGN;        
   else
      animal_err_current_reporter = app_error_reporter; /* Set reporter */

   return animal_err_temp_reporter;
}

/**
 * \brief Returns current error reporter.
 * \return Pointer to current error reporter, or #ANIMAL_ERR_DFL or #ANIMAL_ERR_IGN.
 *
 * Returns current error reporter.
 *
 * \sa animal_err_set_reporter().
 */
AnimalExport Animal_ErrorReporterFunc
 animal_err_get_reporter(void)
{
   return animal_err_current_reporter;
}

/**
 * \brief Registers occurence of an error.
 * \param func_name    Name of function in which error occurs
 * \param err_code     Numeric code of error
 * \param file_name    Name of file in which error occurs
 * \param line_number  Line in file at which error occurs
 * \param message      Message string describing error
 *
 * \return The error number of error registered. #ANIMAL_EC_DFT_DEEP_ERROR if a
 *         deep error occurs, \a err_code otherwise.
 *                
 * Registers occurence of an error. Intended to be called at the lowest
 * function level immediately after the occurence of an error, and called at
 * every level of the function call stack during unwinding, until the error is
 * handled, or it unwinds into a function level where a different error
 * handling mechanism is used.
 *
 * If the trace mode is #ANIMAL_ERR_TRACE_OFF, this function causes the current
 * error reporter to called immediately. If the trace mode is
 * #ANIMAL_ERR_TRACE_ON, this function causes the details of the error to be
 * placed onto an error trace. The error details are reported in "batch" at a
 * later time upon invokation of animal_err_report().
 *
 * \warning #ANIMAL_EC_DFT_DEEP_ERROR is registered in the top record of the error
 *          trace if a deep error occurs. The error requested to be registered
 *          is placed in the second top record of the error trace. This error
 *          may be missing the message string, because it may have been the
 *          process of allocating memory for this string that caused the deep
 *          error to occur.
 *
 * This function is typically called using the macro animal_err_register().
 *
 * \sa animal_err_register() (macro), animal_err_set_trace().
 * \note If trace mode is off then call error reporter immediately, otherwise
 *       push error details onto error trace.
 */
AnimalExport int
 animal_err_register_fileline ( const char *func_name,
                             int         err_code,
                             const char *file_name,
                             int         line_number,
                             const char *message )
{
   Animal_ErrorTrace *atrace = NULL;
   int             the_err_code = ANIMAL_EC_FAIL; /* Registered error code */

   /* When trace mode is off, still use the trace, but flush it before and
    * after use.
    */
   if ( animal_err_trace_mode == ANIMAL_ERR_TRACE_OFF )
      animal_err_trace_top = animal_et_flush_trace(animal_err_trace_top);

   animal_err_trace_top = animal_et_push ( animal_err_trace_top, func_name, err_code,
                                     file_name, line_number, message );

   /*  Note the registered error code. Could be a deep error */
   if ( (atrace = animal_et_get_record_first(animal_err_trace_top)) != NULL )
      the_err_code = atrace->err_code;

   /*  Report immediately. Use trace as interim storage. Flush afterwards */
   if ( animal_err_trace_mode == ANIMAL_ERR_TRACE_OFF )
   {
      animal_err_report();
      animal_err_trace_top = animal_et_flush_trace(animal_err_trace_top);
      /*  This flush is belts and braces. The primary flush is at the head
       * of this function.
       */
   }

   return the_err_code;
}

/**
 * \brief Invokes current error reporter.
 * \return No value.
 *
 * Invokes current error reporter. That's all. It is the responsibility of the
 * error reporter to traverse the error trace, read the details of each error,
 * and to report those details in whatever application specific manner it
 * chooses.
 *
 * This function is automatically called from animal_err_register_fileline() when
 * trace mode is OFF.
 *               
 * \warning The error reporter will not be called if
 *          animal_err_set_reporter(#ANIMAL_ERR_IGN) has been called.
 *          The default error reporter is called when
 *           animal_err_set_reporter(#ANIMAL_ERR_DFL);
 * \sa animal_err_set_trace(), animal_err_set_reporter().
 */
AnimalExport void
 animal_err_report(void)
{
   if (animal_err_current_reporter == ANIMAL_ERR_DFL )
      animal_err_default_reporter();
   else if ( animal_err_current_reporter != ANIMAL_ERR_IGN )
      animal_err_current_reporter();
    
   return;
} /* animal_err_report() */

/**
 * \brief Enable or disable use of trace to store error details.
 * \param trace_mode Whether to switch trace mode on or off
 * \return No value.
 *
 * A trace is a data structure that stacks error details for subsequent
 * reporting (activated by animal_err_report()). If trace is disabled by
 * passing \a trace_mode as #ANIMAL_ERR_TRACE_OFF, then errors are reported
 * immediately upon being registered i.e. when animal_err_register() is called.
 * Otherwise trace mode is switched on by passing #ANIMAL_ERR_TRACE_ON.
 *
 * \warning Trace mode is initialised to #ANIMAL_ERR_TRACE_OFF.
 *          Any non-zero trace_mode is assumed equivalent to
 *          #ANIMAL_ERR_TRACE_OFF.
 *          When trace is turned off, the trace is flushed immediately.
 * \sa animal_err_report(), animal_err_register().
 */
AnimalExport void
 animal_err_set_trace( Animal_TraceMode trace_mode )
{
   if ( trace_mode == ANIMAL_ERR_TRACE_OFF )
      animal_err_trace_top = animal_et_flush_trace(animal_err_trace_top);
    
   animal_err_trace_mode = trace_mode;
   return;
}


/**
 * \brief Flush all errors in trace.
 * \return No value.
 *
 * Flush all errors in error trace
 * \note Traverse error trace from start to finish deleting all error records
 *       (except two reserved ones which are transparent to this module).
 */
AnimalExport void
 animal_err_flush_trace( void )
{
   animal_err_trace_top = animal_et_flush_trace(animal_err_trace_top);
   return;
} /*  animal_err_flush_trace() */


/**
 * \brief Gets the number of errors in error trace.
 * \return Number of errors in error trace.
 *
 * Gets the number of errors in error trace.
 *
 * \sa animal_err_get_error().
 */
AnimalExport int
 animal_err_get_error_count( void )
{
   return animal_et_get_record_count(animal_err_trace_top);
} /* animal_err_get_error_count() */

/**
 * \brief Gets details of n-th error stored in the error trace.
 * \param n            Index of requested error [1..N]
 * \param func_name    Name of function in which error occurs
 * \param err_code     Numeric code of error
 * \param file_name    Name of file in which error occurs
 * \param line_number  Line in file at which error occurs
 * \param message      Message string describing error
 * \return Status of n-th error.
 *
 * If any of above pointers are \c NULL, then those details are not returned.
 *
 * Gets details of \a n-th error. \a n=1 refers to the most recent error
 * registered in error trace. Usually animal_err_get_error_count() is called to
 * obtain the number of error records in the trace. Return values:
 *     - #ANIMAL_EC_DFT_BAD_N Index '\a n' out of bounds,
 *     - #ANIMAL_EC_DFT_EMPTY Error trace is empty (regardless of requested n)
 *     - #ANIMAL_EC_OK        Otherwise.
 *
 * \warning The returned strings (\a func_name, \a file_name, \a message) are
 *          not guaranteed to exist at a later time, nor should they be
 *          modified in place. Therefore the calling function must either
 *          use the returned strings immediately or make copies.
 *
 *          Because the stack is numbered from the top (1=most
 *          recent) an arbitrary index i may refer to different
 *          error records at different times. However, index 1
 *          always refers to the most recent error.
 *            
 * \sa animal_err_get_error_count().
 */
AnimalExport int
 animal_err_get_error ( int          n,
                     const char **func_name,
                     int         *err_code,
                     const char **file_name,
int         *line_number,
                     const char **message )
{
   int count;
   int i;                                /* Loop counter */
   Animal_ErrorTrace *a_record = NULL;

   if ( (count = animal_et_get_record_count(animal_err_trace_top)) < 1 )
      return ANIMAL_EC_DFT_EMPTY;                /* Error trace is empty */
    
   /*  Boundary check on n */
   if ( (n < 1) || (n > count) )
      return ANIMAL_EC_DFT_BAD_N;                /* Out of bounds, bye */

   for ( a_record = animal_et_get_record_first(animal_err_trace_top), i = 1;
         i < n; i++ )
      a_record = animal_et_get_record_next(a_record); /* Step to N-th error */

   if ( func_name != NULL )
      *func_name = a_record->func_name;

   if ( err_code != NULL )
      *err_code = a_record->err_code;

   if ( file_name != NULL )
      *file_name = a_record->file_name;
    
   if ( line_number != NULL )
      *line_number = a_record->line_number;
    
   if ( message != NULL )
      *message = a_record->message;
    
   return ANIMAL_EC_OK;
} /* animal_err_get_error() */

/**
 * \}
 */

/**
 * \}
 */
