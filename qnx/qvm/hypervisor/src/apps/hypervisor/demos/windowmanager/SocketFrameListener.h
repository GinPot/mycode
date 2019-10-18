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

/*
 * E.g
 * data is the obtained data pointer, len is the length
 * void getData(void* data, int len);
 *
 * state value is 0 and the failure is -1.
 * int getState(int state);
 *
 * Callback is a custom callback function
 * SocketCallback callback;
 * callback.callbackData = getData;
 * callback.callbackState = getState;
 *
 * TESTSOCKET is the domain name to establish the local socket; listener is false for the client, true for the server
 * SocketFrameListener *testClient = new SocketFrameListener("TESTSOCKET", false, callback);
 * if (testClient->startListener()) {
 *     exit(1);
 * }
 *
 * the return value is 0 and the failure is -1.
 * int state = testClient->sendData(p, strlen(p));
 * 
 * stop: testClient->stopListener()
 *
 */

#ifndef _SOCKET_FRAME_LISTENER_H
#define _SOCKET_FRAME_LISTENER_H

#include <pthread.h>

#include "SocketFrameClient.h"
#include <sys/un.h>
#include<netinet/in.h>

typedef void (*CALLBACK_DATA)(void* data, int len);
typedef int  (*CALLBACK_STATE)(int state);

struct SocketCallback{
    CALLBACK_DATA   callbackData;
    CALLBACK_STATE  callbackState;
};

class SocketFrameListener {
    bool                    mListen;
    const char              *mSocketName;
    int                     mSock;
    SocketClientCollection  *mClients;
    pthread_mutex_t         mClientsLock;
    int                     mCtrlPipe[2];
    pthread_t               mThread;
    bool                    mUseCmdNum;
    SocketCallback          mCallBack;
    struct sockaddr_in      mSrvAddr;

public:
    SocketFrameListener(const char *socketName, bool listen, SocketCallback callback);
    virtual     ~SocketFrameListener();

    int         startListener(int backlog = 4);
    int         stopListener();

    int         sendData(void *data, unsigned int len);

    bool        release(SocketFrameClient *c) { return release(c, true); }

protected:
    bool        onDataAvailable(SocketFrameClient *c);

private:
    static void *threadStart(void *obj);
    bool        release(SocketFrameClient *c, bool wakeup);
    void        runListener();
    void        init(const char *socketName, int socketFd, bool listen, SocketCallback callback);
    int         getLogSocket(const char *name, bool listener);
};
#endif
