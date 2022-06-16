/*
 * Copyright (C) 2008-2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include <LLog.h>
//#include <cutils/sockets.h>


#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include "SocketFrameListener.h"
#include "SocketFrameClient.h"
# define TEMP_FAILURE_RETRY(expression) \
  (__extension__							      \
    ({ long int __result;						      \
       do __result = (long int) (expression);				      \
       while (__result == -1L && errno == EINTR);			      \
       __result; }))

#define CtrlPipe_Shutdown 0
#define CtrlPipe_Wakeup   1
#define SER_PORT 8999

SocketFrameListener::SocketFrameListener(const char *socketName, bool listen, SocketCallback callback) {
    init(socketName, -1, listen, callback);
}

void SocketFrameListener::init(const char *socketName, int socketFd, bool listen, SocketCallback callback) {
    mListen = listen;
    mSocketName = socketName;
    mSock = socketFd;
    mCallBack = callback;
    mUseCmdNum = false;
    pthread_mutex_init(&mClientsLock, NULL);
    mClients = new SocketClientCollection();
}

SocketFrameListener::~SocketFrameListener() {
    if ((mSocketName || mListen) && mSock > -1)
        close(mSock);

    if (mCtrlPipe[0] != -1) {
        close(mCtrlPipe[0]);
        close(mCtrlPipe[1]);
    }
    SocketClientCollection::iterator it;
    for (it = mClients->begin(); it != mClients->end();) {
        (*it)->decRef();
        it = mClients->erase(it);
    }
    delete mClients;
}

int SocketFrameListener::startListener(int backlog) {
    if(mListen || mSocketName){
        if ((mSock = getLogSocket(mSocketName, mListen)) < 0) {
            mCallBack.callbackState(-1);
            BLOGD("Obtaining file descriptor socket '%s' failed: %s", mSocketName, strerror(errno));
            return -1;
        }
        BLOGD("got mSock = %d for %s", mSock, mSocketName);
    }else if (!mSocketName && !mListen && mSock == -1) {
        BLOGD("Failed to length unbound listener");
        errno = EINVAL;
        mCallBack.callbackState(-1);
        return -1;
    }

    if (mListen && listen(mSock, backlog) < 0) {
        mCallBack.callbackState(-1);
        BLOGD("Unable to listen on socket (%s)", strerror(errno));
        return -1;
    } else if (!mListen)
        mClients->push_back(new SocketFrameClient(mSock, false, mUseCmdNum));

    if (pipe(mCtrlPipe)) {
        BLOGD("pipe failed (%s)", strerror(errno));
        return -1;
    }

    if (pthread_create(&mThread, NULL, SocketFrameListener::threadStart, this)) {
        mCallBack.callbackState(-1);
        BLOGD("pthread_create (%s)", strerror(errno));
        return -1;
    }

    return 0;
}

int SocketFrameListener::stopListener() {
    char c = CtrlPipe_Shutdown;
    int  rc;

    rc = TEMP_FAILURE_RETRY(write(mCtrlPipe[1], &c, 1));
    if (rc != 1) {
        BLOGD("Error writing to control pipe (%s)", strerror(errno));
        return -1;
    }

    void *ret;
    if (pthread_join(mThread, &ret)) {
        BLOGD("Error joining to listener thread (%s)", strerror(errno));
        return -1;
    }
    close(mCtrlPipe[0]);
    close(mCtrlPipe[1]);
    mCtrlPipe[0] = -1;
    mCtrlPipe[1] = -1;

    if ((mSocketName || mListen) && mSock > -1) {
        close(mSock);
        mSock = -1;
    }

    SocketClientCollection::iterator it;
    for (it = mClients->begin(); it != mClients->end();) {
        delete (*it);
        it = mClients->erase(it);
    }
    return 0;
}

void *SocketFrameListener::threadStart(void *obj) {
    SocketFrameListener *me = reinterpret_cast<SocketFrameListener *>(obj);
    me->runListener();
    pthread_exit(NULL);
    return NULL;
}

void SocketFrameListener::runListener() {

    SocketClientCollection pendingList;
    while(1) {
        SocketClientCollection::iterator it;
        fd_set read_fds;
        int rc = 0;
        int max = -1;

        FD_ZERO(&read_fds);
        if (mListen) {
            max = mSock;
            FD_SET(mSock, &read_fds);
        }

        FD_SET(mCtrlPipe[0], &read_fds);
        if (mCtrlPipe[0] > max)
            max = mCtrlPipe[0];

        pthread_mutex_lock(&mClientsLock);
        for (it = mClients->begin(); it != mClients->end(); ++it) {
            // NB: calling out to an other object with mClientsLock held (safe)
            int fd = (*it)->getSocket();
            FD_SET(fd, &read_fds);
            if (fd > max) {
                max = fd;
            }
        }
        pthread_mutex_unlock(&mClientsLock);
        BLOGD("mListen=%d, max=%d, mSocketName=%s", mListen, max, mSocketName);

        if ((rc = select(max + 1, &read_fds, NULL, NULL, NULL)) < 0) {
            if (errno == EINTR)
                continue;
            BLOGD("select failed (%s) mListen=%d, max=%d", strerror(errno), mListen, max);
            sleep(1);
            continue;
        } else if (!rc)
            continue;

        if (FD_ISSET(mCtrlPipe[0], &read_fds)) {
            char c = CtrlPipe_Shutdown;
            TEMP_FAILURE_RETRY(read(mCtrlPipe[0], &c, 1));
            if (c == CtrlPipe_Shutdown) {
                break;
            }
            continue;
        }

        if (mListen && FD_ISSET(mSock, &read_fds)) {
            struct sockaddr addr;
            socklen_t alen;
            int c;

            do {
                alen = sizeof(addr);
                c = accept(mSock, &addr, &alen);
                BLOGD("%s got %d from accept", mSocketName, c);
            } while (c < 0 && errno == EINTR);
            if (c < 0) {
                BLOGD("accept failed (%s)", strerror(errno));
                sleep(1);
                continue;
            }
            pthread_mutex_lock(&mClientsLock);
            mClients->push_back(new SocketFrameClient(c, true, mUseCmdNum));
            pthread_mutex_unlock(&mClientsLock);
        } 

        if (!mListen) {
            int state = connect(mSock, (struct sockaddr*)&mSrvAddr,sizeof(mSrvAddr));
            if (state) {
                close(mSock);
                mSock =  socket(PF_UNIX,SOCK_STREAM,0);

                while (connect(mSock, (struct sockaddr*)&mSrvAddr,sizeof(mSrvAddr)) != 0) {
                    sleep(5);
                }
            }
        }

        /* Add all active clients to the pending list first */
        pendingList.clear();
        pthread_mutex_lock(&mClientsLock);
        for (it = mClients->begin(); it != mClients->end(); ++it) {
            SocketFrameClient* c = *it;
            // NB: calling out to an other object with mClientsLock held (safe)
            int fd = c->getSocket();
            if (FD_ISSET(fd, &read_fds)) {
                pendingList.push_back(c);
                c->incRef();
            }
        }
        pthread_mutex_unlock(&mClientsLock);
        /* Process the pending list, since it is owned by the thread,
         * there is no need to lock it */
        while (!pendingList.empty()) {
            /* Pop the first item from the list */
            it = pendingList.begin();
            SocketFrameClient* c = *it;
            pendingList.erase(it);
            /* Process it, if false is returned, remove from list */
            if (!onDataAvailable(c)) {
                release(c, false);
            }
            c->decRef();
        }
    }
}

bool SocketFrameListener::onDataAvailable(SocketFrameClient *c) {

    ssize_t ret;
    int sock = c->getSocket();
    int MAXLEN = 1024;
    char buffer[MAXLEN];

    int readLength =  0;
    int first = 0;

    int start = 0;
    int length = 0;

    char *data = NULL;


    while ((ret = TEMP_FAILURE_RETRY(read(sock, buffer, MAXLEN))) > 0) {

        if (first == 0) {
            start = buffer[0];
            length = length | (start << 24);
            start = buffer[1];
            length = length | (start << 16);
            start = buffer[2];
            length = length | (start << 8);
            start = buffer[3];
            length = length | (start << 0);

            data = (char*)malloc(length);
            memset(data, 0, length);
            if (data == NULL) {
                BLOGD("Memory allocation failed");
                return false;
            }
            memcpy(data, buffer + 4, ret - 4);
            readLength = ret - 4;
        } else {
            memcpy(data + readLength, buffer, ret);
            readLength += ret;
        }

        if (readLength == length) {
            mCallBack.callbackData(data, length);
            free(data);
            return true;
        }

        first++;
    }

    if (ret <= 0) {
        return false;
    }

    return true;
}

bool SocketFrameListener::release(SocketFrameClient* c, bool wakeup) {
    bool ret = false;
    /* if our sockets are connection-based, remove and destroy it */
    if (mListen && c) {
        /* Remove the client from our array */
        BLOGD("going to zap %d for %s", c->getSocket(), mSocketName);
        pthread_mutex_lock(&mClientsLock);
        SocketClientCollection::iterator it;
        for (it = mClients->begin(); it != mClients->end(); ++it) {
            if (*it == c) {
                mClients->erase(it);
                ret = true;
                break;
            }
        }
        pthread_mutex_unlock(&mClientsLock);
        if (ret) {
            ret = c->decRef();
            if (wakeup) {
                char b = CtrlPipe_Wakeup;
                TEMP_FAILURE_RETRY(write(mCtrlPipe[1], &b, 1));
            }
        }
    }
    return ret;
}

int SocketFrameListener::sendData(void *data, unsigned int len) {
    SocketClientCollection safeList;

    unsigned char *p = (unsigned char*)malloc(sizeof(int) + len);
    memset(p, 0, sizeof(int) + len);
    p[0] =  len >> 24 & 0xff;
    p[1] =  len >> 16 & 0xff;
    p[2] =  len >> 8 & 0xff;
    p[3] =  len >> 0 & 0xff;

    memcpy(p+4, data, len);


    /* Add all active clients to the safe list first */
    safeList.clear();
    pthread_mutex_lock(&mClientsLock);
    SocketClientCollection::iterator i;

    for (i = mClients->begin(); i != mClients->end(); ++i) {
        SocketFrameClient* c = *i;
        c->incRef();
        safeList.push_back(c);
    }
    pthread_mutex_unlock(&mClientsLock);

    while (!safeList.empty()) {
        /* Pop the first item from the list */
        i = safeList.begin();
        SocketFrameClient* c = *i;
        safeList.erase(i);
        if (c->sendData(p, len + sizeof(int))) {
            BLOGD("Error sending data (%s)", strerror(errno));
            free(p);
            mCallBack.callbackState(-1);
            return -1;
        }
        c->decRef();
    }
    free(p);
    mCallBack.callbackState(0);
    return 0;
}


int SocketFrameListener::getLogSocket(const char *name, bool listener) {
    int sock = -1;
    if(listener){
        printf("...server init....\n");

        sock = socket(AF_INET,SOCK_STREAM,0);
        if (sock < 0) {
            printf("write error (%d)\n", sock);
             mCallBack.callbackState(sock);
             return -1;
        }
        bzero(&mSrvAddr,sizeof(mSrvAddr));
        mSrvAddr.sin_family = AF_INET;
        mSrvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        mSrvAddr.sin_port = htons(SER_PORT);
        bind(sock,(struct sockaddr *)&mSrvAddr,sizeof(mSrvAddr));
    } else if(name){
        printf("...client init....");
        sock = socket(AF_INET,SOCK_STREAM,0);
        if (sock < 0) {
            printf("write error client (%d)", sock);
             mCallBack.callbackState(sock);
             return -1;
        }

        bzero(&mSrvAddr,sizeof(mSrvAddr));
        mSrvAddr.sin_family = AF_INET;
        inet_pton(AF_INET, name, &mSrvAddr.sin_addr);
        mSrvAddr.sin_port = htons(SER_PORT);

        int state = connect(sock,(struct sockaddr*)&mSrvAddr,sizeof(mSrvAddr));
        if (state < 0) {
            printf("connet error:%s\n",strerror(errno));
            return -1;
        }
    }

    return sock;
}
