#include <fcntl.h>
#include <sys/procfs.h>
#include <sys/time.h>

#include "cpuload.h"
#include "fifo.h"

#define MAX_CPUS 32
#define CPUS_NUM 6
/* CPU */
static double		Loads[ MAX_CPUS ];
static _uint64		LastSutime[ MAX_CPUS ];
static _uint64		LastNsec[ MAX_CPUS ];
static int			ProcFd = -1;
static int			NumCpus = 0;
Queue idata[CPUS_NUM],totaldata[CPUS_NUM];
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
void InitCpuLoad(void)
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
	
	for(i=0; i<CPUS_NUM; i++){
		InitQueue(&idata[i]);
		InitQueue(&totaldata[i]);
	}	
}

/* Sample all CPUs  */
static void sample_cpus( void )
{
	debug_thread_t	debug_data;
	_uint64			cur, dsu, dt;
	int					i,j;
	static _uint64 total_i[CPUS_NUM]={0}, total_total[CPUS_NUM]={0};
	_uint64 popilast,poptotallast;
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
		
		if(idata[i].m_size == capacity){
			popilast = Pop(&idata[i]);
			poptotallast = Pop(&totaldata[i]);
			
			total_i[i] = total_i[i] - popilast + dsu;
			total_total[i] = total_total[i] - poptotallast + dt;
			
			Push(&idata[i], dsu);
			Push(&totaldata[i], dt);
		
			//printf("first=%d   mo=%d  procputimetotal=%d   m_size=%d\n",totalcputimecur,totalcputimelast,totalputimetotal,procpuload.m_size);
		}else{
			Push(&idata[i], dsu);
			Push(&totaldata[i], dt);
			if(IsFull(&idata[i]) == true){
				for(j=0;j<capacity;j++){
					total_i[i] += idata[i].m_array[j];
					total_total[i] += totaldata[i].m_array[j];
				}
			}
		}
		
		/* calculate load */
		Loads[i] = 100.0 - ( ( total_i[i] * 100.0 ) / total_total[i] );
		//Loads[i] = ( ( dsu * 100.0 ) / dt );
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

static double get_cpu( int n )
{
	return Loads[ n % MAX_CPUS ];
}

int GetCpuLoad(float *cpuload)
{
	int		i;

	sample_cpus();

	cpuload[0] = (get_cpu(0) + get_cpu(1) + get_cpu(2) + get_cpu(3) + get_cpu(4) + get_cpu(5))/6;
	for ( i = 0; i < 6; i++ )
		cpuload[i+1] = get_cpu(i);
	
	return 0;
}