#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>

#include "fifo.h"


 
void InitQueue(Queue *pQueue)
{
    pQueue->m_front = 0;
    pQueue->m_rear = 0;
    pQueue->m_size = 0;
}
 
int IsFull(Queue *pQueue)
{
    return pQueue->m_size == capacity ? true : false;
}
 
void Push(Queue *pQueue, int v)
{
    pQueue->m_array[pQueue->m_rear] = v;
 
    pQueue->m_rear++;
    if (pQueue->m_rear == capacity)
    {
        pQueue->m_rear = 0;
    }
 
    pQueue->m_size++;
}
 
int IsEmpty(Queue *pQueue)
{
    return pQueue->m_size == 0 ? true : false;
}
 
int Pop(Queue *pQueue)
{
    int current = pQueue->m_front;
 
    pQueue->m_front++;
    if (pQueue->m_front == capacity)
    {
        pQueue->m_front = 0;
    }
 
    pQueue->m_size--;
 
    return pQueue->m_array[current];
}