#ifndef _MEM_TESTER_
#define _MEM_TESTER_

/*
 * Author:  David Robert Nadeau
 * Site:    http://NadeauSoftware.com/
 * License: Creative Commons Attribution 3.0 Unported License
 *          http://creativecommons.org/licenses/by/3.0/deed.en_US
 */

#if defined(_WIN32)
#include <windows.h>
#include <psapi.h>

#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#include <sys/resource.h>

#if defined(__APPLE__) && defined(__MACH__)
#include <mach/mach.h>

#elif (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__)))
#include <fcntl.h>
#include <procfs.h>

#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
#include <stdio.h>

#endif

#else
#error "Cannot define getPeakRSS( ) or getCurrentRSS( ) for an unknown OS."
#endif

/**
* Returns the peak (maximum so far) resident set size (physical
* memory use) measured in bytes, or zero if the value cannot be
* determined on this OS.
*/
double getPeakRSS( )
{
#if defined(_WIN32)
  /* Windows -------------------------------------------------- */
  PROCESS_MEMORY_COUNTERS info;
  GetProcessMemoryInfo( GetCurrentProcess( ), &info, sizeof(info) );
  return ((double)info.PeakWorkingSetSize)/1048576;

#elif (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__)))
  /* AIX and Solaris ------------------------------------------ */
  struct psinfo psinfo;
  int fd = -1;
  if ( (fd = open( "/proc/self/psinfo", O_RDONLY )) == -1 )
    return (double)0;      /* Can't open? */
  if ( read( fd, &psinfo, sizeof(psinfo) ) != sizeof(psinfo) )
  {
    close( fd );
    return (double)0;      /* Can't read? */
  }
  close( fd );
  return ((double)psinfo.pr_rssize)/1024;

#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
  /* BSD, Linux, and OSX -------------------------------------- */
  struct rusage rusage;
  getrusage( RUSAGE_SELF, &rusage );
#if defined(__APPLE__) && defined(__MACH__)
  return ((double)rusage.ru_maxrss)/1048576;
#else
  return ((double)rusage.ru_maxrss)/1024;
#endif

#else
  /* Unknown OS ----------------------------------------------- */
  return (double)0;          /* Unsupported. */
#endif
}





/**
* Returns the current resident set size (physical memory use) measured
* in bytes, or zero if the value cannot be determined on this OS.
*/
double getCurrentRSS( )
{
#if defined(_WIN32)
  /* Windows -------------------------------------------------- */
  PROCESS_MEMORY_COUNTERS info;
  GetProcessMemoryInfo( GetCurrentProcess( ), &info, sizeof(info) );
  return ((double)info.WorkingSetSize)/1048576;

#elif defined(__APPLE__) && defined(__MACH__)
  /* OSX ------------------------------------------------------ */
  struct mach_task_basic_info info;
  mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;
  if ( task_info( mach_task_self( ), MACH_TASK_BASIC_INFO,
                  (task_info_t)&info, &infoCount ) != KERN_SUCCESS )
    return (double)0;      /* Can't access? */
  return ((double)info.resident_size)/1048576;

#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
  /* Linux ---------------------------------------------------- */
  long rss = 0L;
  FILE* fp = NULL;
  if ( (fp = fopen( "/proc/self/statm", "r" )) == NULL )
    return (double)0;      /* Can't open? */
  if ( fscanf( fp, "%*s%ld", &rss ) != 1 )
  {
    fclose( fp );
    return (double)0;      /* Can't read? */
  }
  fclose( fp );
  return ((double)rss * (double)sysconf( _SC_PAGESIZE))/1048576;

#else
  /* AIX, BSD, Solaris, and Unknown OS ------------------------ */
  return (double)0;          /* Unsupported. */
#endif
}


double maxCurrentRSS;

void clearMaxCurrentRSS(){
  maxCurrentRSS = 0;
}

void checkMaxCurrentRSS(){
  double currentRSS = getCurrentRSS();
  if(maxCurrentRSS < currentRSS)
    maxCurrentRSS = currentRSS;
}

double getMaxCurrentRSS(){
  checkMaxCurrentRSS();
  return maxCurrentRSS;
}

#endif
