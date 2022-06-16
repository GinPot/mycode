#ifndef _FIFO_H
#define _FIFO_H

#define true 1
#define false 0

#define capacity 10 //队列最大元素个数

typedef struct Queue
{
    int m_array[capacity];
    int m_front;//首元素下标
    int m_rear;//尾元素下标
    int m_size;//当前元素个数
}Queue;

extern void InitQueue(Queue *pQueue);
extern int IsFull(Queue *pQueue);
extern void Push(Queue *pQueue, int v);
extern int IsEmpty(Queue *pQueue);
extern int Pop(Queue *pQueue);

    
#endif