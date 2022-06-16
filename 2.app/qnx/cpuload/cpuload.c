#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <devctl.h>
#include <err.h>
#include <errno.h>
#include <libc.h>
#include <inttypes.h>
#include <atomic.h>
#include <pthread.h>
#include <sys/debug.h>
#include <sys/iofunc.h>
#include <sys/neutrino.h>
#include <sys/procfs.h>
#include <sys/resmgr.h>
#include <sys/syspage.h>
#include <sys/time.h>
#include <sys/types.h>

#define MAX_CPUS 32
/* CPU */
static double		Loads[ MAX_CPUS ];
static _uint64		LastSutime[ MAX_CPUS ];
static _uint64		LastNsec[ MAX_CPUS ];
static int			ProcFd = -1;
static int			NumCpus = 0;
/* Actual time in nsec */
static _uint64 CurTimeNs( void )
{
	struct timeval	tval;
	_uint64			sec, usec;
	gettimeofday( &tval, NULL );
	sec = tval.tv_sec;
	usec = tval.tv_usec;
	return ( ( sec * 1000000 ) + usec ) * 1000;
}
/* Initialize the CPU-measurement */
void init_cpus( void )
{
	debug_thread_t		debug_data;
	int						i;
	/* get number of CPUs */
	NumCpus = _syspage_ptr->num_cpu;
	memset( &debug_data, 0, sizeof debug_data );
	ProcFd = open( "/proc/1/as", O_RDONLY );
	for ( i = 0; i < NumCpus; i++ )
	{
		debug_data.tid = i + 1;
		devctl( ProcFd, DCMD_PROC_TIDSTATUS, &debug_data, sizeof debug_data, NULL );
		LastSutime[i] = debug_data.sutime;
		LastNsec[i] = CurTimeNs();
	}
}
/* Number of CPUs */
int num_cpus( void )
{
	return NumCpus;
}
/* Sample all CPUs  */
void sample_cpus( void )
{
	debug_thread_t	debug_data;
	_uint64			cur, dsu, dt;
	int					i;
	memset( &debug_data, 0, sizeof debug_data );
	for ( i = 0; i < NumCpus; i++ )
	{
		
		/* 
		The trick, is:
		The su-time of the idle-treads tells us how much CPU-time was
		/not/ consumed. Having this and knowing the indeed passed by 
		time we can calculate the CPU-Load in percent. 
		The idle.thread /i/+1 belongs belongs to CPU /i/. 
		*/
		debug_data.tid = i + 1;
		devctl( ProcFd, DCMD_PROC_TIDSTATUS, &debug_data, sizeof debug_data, NULL );
	      cur = CurTimeNs(); 
		dsu = debug_data.sutime - LastSutime[i];
		dt  = cur - LastNsec[i];
		/* calculate load */
		//Loads[i] = 100.0 - ( ( dsu * 100.0 ) / dt );
		Loads[i] = ( ( dsu * 100.0 ) / dt );
		/* balance rounding errors */
		if ( Loads[i] < 0 )
			Loads[i] = 0;
		else if ( Loads[i] > 100 )
			Loads[i] = 100;
		/*  Save old values for the next pass */
		LastNsec[i] = cur;
		LastSutime[i] = debug_data.sutime;
	}
}

double get_cpu( int n )
{
	return Loads[ n % MAX_CPUS ];
}
int main()
{
	int		i;
	init_cpus();
	for ( ; ; )
	{
		sample_cpus();
		printf( "\f" );
		printf( "CPU states: %.1f%\n", (600-(get_cpu(0) + get_cpu(1) + get_cpu(2) + get_cpu(3) + get_cpu(4) + get_cpu(5)))/6);
		for ( i = 0; i < num_cpus(); i++ )
			printf( "CPU #%d: %.1f%\n", i+1, get_cpu( i ) );
		delay( 5000 );
	}

	return EXIT_SUCCESS;
}