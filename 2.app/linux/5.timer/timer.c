#include <sys/timerfd.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>


#define handle_error(msg) \
       do { perror(msg); exit(EXIT_FAILURE); } while (0)

int fRunning = 1;

int gTimerfd = -1;

int gEventfd = -1;

int initTimer(void);
void startTimer(int timerfd);
void stopTimer(int timerfd);
void print_elapsed_time(void);


void sig_handler(int signum)
{
    stopTimer(gTimerfd);
    
    close(gTimerfd);
    gTimerfd = -1;
    
    uint64_t u = 100;
    write(gEventfd, &u, sizeof(uint64_t));
    
    fRunning = 0;
}


void *thr_func(void *arg)
{
    int timerfd = *(int*)(arg);
    
    startTimer(timerfd);
    
    int epollfd = epoll_create1(EPOLL_CLOEXEC); // or epoll_create(1)
    if (epollfd == -1)
    {
        handle_error("epoll_create1");
    }

    struct epoll_event evTimer;
    evTimer.events = EPOLLIN; 
    evTimer.data.fd = timerfd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, timerfd, &evTimer);
    
    struct epoll_event evEvent;
    evEvent.events = EPOLLIN; 
    evEvent.data.fd = gEventfd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, gEventfd, &evEvent);

    const int maxEvents = 2; // 2 events
    struct epoll_event events[maxEvents]; 
    
    
    print_elapsed_time();
    printf("timer started\n");
    
    
    uint64_t exp = 0;
    int result = 0;
    while (fRunning)
    {
        int nfd = epoll_wait(epollfd, events, maxEvents, -1);
        if (nfd > 0)
        {
            for (int i = 0; i < nfd; ++i)
            {
                exp = 0;
                result = 0;
                
                if (events[i].data.fd == timerfd)
                {
                    result = read(timerfd, &exp, sizeof(uint64_t));
                    if (result == sizeof(uint64_t)) 
                    {
                        print_elapsed_time();
                    }
                    
                }
                else if (events[i].data.fd == gEventfd)
                {
                    result = read(gEventfd, &exp, sizeof(uint64_t));
                    if (result == sizeof(uint64_t)) 
                    {
                        if (exp == 100)
                        {
                            fRunning = 0;
                        }
                    }
                    
                }
            }

        }

    }
    
    return NULL;
}


int main(void)
{
    signal(SIGINT, sig_handler);
    
    gTimerfd = initTimer();
    if (gTimerfd < 0)
    {
        return -1;
    }
    
    gEventfd = eventfd(0, 0);
    if (gEventfd < 0)
    {
        return -1;
    }
    
    pthread_t tid;
    pthread_create(&tid, NULL, thr_func, &gTimerfd);
    
    pthread_join(tid, NULL);

    return 0;

}



int initTimer(void)
{
    //int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    int timerfd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (timerfd == -1)
    {
        printf("timerfd_create fail\n");
    }
    
    return timerfd;
}

void startTimer(int timerfd)
{
    struct itimerspec new_value = {};
    new_value.it_value.tv_sec  = 0;					//第一次超时时间设置
    new_value.it_value.tv_nsec = 40000000; // 50ms

    new_value.it_interval.tv_sec  = 0;				//之后每次超时时间设置
    new_value.it_interval.tv_nsec = 40000000; // 50ms

    if (timerfd_settime(timerfd, 0, &new_value, NULL) == -1)
    {
        printf("timerfd_settime fail\n");
    }
}

void stopTimer(int timerfd)
{
    if (timerfd > 0)
    {
        struct itimerspec new_value = {};
        new_value.it_value.tv_sec  = 0; 
        new_value.it_value.tv_nsec = 0;

        new_value.it_interval.tv_sec  = 0;
        new_value.it_interval.tv_nsec = 0;

        if (timerfd_settime(timerfd, 0, &new_value, NULL) == -1)
        {
            printf("timerfd_settime fail\n");
        }
    }
}


void print_elapsed_time(void)
{
    static struct timeval start = {};
    static int first_call = 1;

    if (first_call == 1)
    {
        first_call = 0;
        if (gettimeofday(&start, NULL) == -1)
        {
            handle_error("gettimeofday");
        }
    }

    struct timeval current = {};
    if (gettimeofday(&current, NULL) == -1)
    {
        handle_error("gettimeofday");
    }

    static int old_secs = 0, old_usecs = 0;

    int secs  = current.tv_sec - start.tv_sec;
    int usecs = current.tv_usec - start.tv_usec;
    if (usecs < 0)
    {
        --secs;
        usecs += 1000000;
    }

    usecs = (usecs + 500)/1000; // 四舍五入

    if (secs != old_secs || usecs != old_usecs)
    {
    	printf("%d.%03d\n", secs, usecs);
    	old_secs = secs;
    	old_usecs = usecs;
    }

}
