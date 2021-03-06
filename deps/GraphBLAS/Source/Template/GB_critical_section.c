//------------------------------------------------------------------------------
// Source/Template/GB_critical_section: execute code in a critical section
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2019, All Rights Reserved.
// http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

//------------------------------------------------------------------------------

// All access to the global matrix queue, via GB_queue_* operations, must
// be done through a critical section.  No other part of SuiteSparse:GraphBLAS
// uses this critical section; it is only used for accessing the global matrix
// queue via GB_queue_*.   All GB_queue_* operations use the GB_CRITICAL macro
// to check the result, and if the critical section fails (ok == false),
// they return GrB_PANIC.

// Critical sections for Windows threads and ANSI C11 threads are listed below
// as drafts, but these threading models are not yet supported.

// not parallel: this function does O(1) work and is already thread-safe.

{

    //--------------------------------------------------------------------------
    // POSIX pthreads
    //--------------------------------------------------------------------------

    #if defined (USER_POSIX_THREADS)
    {
        if (GB_Global.user_multithreaded)
        {
            ok = (pthread_mutex_lock (&GB_sync) == 0) ;
        }

        GB_CRITICAL_SECTION ;

        if (GB_Global.user_multithreaded)
        {
            ok = ok && (pthread_mutex_unlock (&GB_sync) == 0) ;
        }
    }

    //--------------------------------------------------------------------------
    // Microsoft Windows
    //--------------------------------------------------------------------------

    #elif defined (USER_WINDOWS_THREADS)
    {
        // This is not yet supported.
        if (GB_Global.user_multithreaded)
        {
            EnterCriticalSection (&GB_sync) ;
        }

        GB_CRITICAL_SECTION ;

        if (GB_Global.user_multithreaded)
        {
            LeaveCriticalSection (&GB_sync) ;
        }
    }

    //--------------------------------------------------------------------------
    // ANSI C11 threads
    //--------------------------------------------------------------------------

    #elif defined (USER_ANSI_THREADS)
    {
        // This should work per the ANSI C11 Spec, but is not yet supported.
        if (GB_Global.user_multithreaded)
        {
            ok = (mtx_lock (&GB_sync) == thrd_success) ;
        }

        GB_CRITICAL_SECTION ;

        if (GB_Global.user_multithreaded)
        {
            ok = ok && (mtx_unlock (&GB_sync) == thrd_success) ;
        }
    }

    //--------------------------------------------------------------------------
    // OpenMP
    //--------------------------------------------------------------------------

    #else   // USER_OPENMP_THREADS or USER_NO_THREADS
    {
        // default: use a named OpenMP critical section.  If OpenMP is not
        // available, then the #pragma is ignored and this becomes vanilla,
        // single-threaded code.
        #pragma omp critical (GB_critical_section)
        GB_CRITICAL_SECTION ;
    }
    #endif
}

#undef GB_CRITICAL_SECTION

