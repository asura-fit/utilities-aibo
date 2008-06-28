//
// RobotToTool.h
//
// Copyright (C) 2001 Sony Corporation
// All Rights Reserved.
//

#ifndef _RobotToTool_h_DEFINED
#define _RobotToTool_h_DEFINED

#include <OPENR/OObject.h>
#ifndef Apertos
#include <OPENR/OSubject.h>
#include <OPENR/OObserver.h>
#else // Apertos
#include <OPENR/OSubject.h>
#include <OPENR/OObserver.h>
#endif // Apertos
#include "def.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

class RobotToTool : public OObject {
  public:
    // OPEN-R Inter-Object Communication
    OSubject*  subject[numOfSubject];
    OObserver* observer[numOfObserver];

  public:
    // public methods
    RobotToTool(void);
    ~RobotToTool();

    // OPEN-R Object Common Interface
    virtual OStatus DoInit   (const OSystemEvent&);
    virtual OStatus DoStart  (const OSystemEvent&);
    virtual OStatus DoStop   (const OSystemEvent&);
    virtual OStatus DoDestroy(const OSystemEvent&);

    void Update(const ONotifyEvent&);
    void Ready(const OReadyEvent&);

  private:
    // private member
    bool active_;

  private:
    // will never be implemented
    RobotToTool(const RobotToTool&);
    RobotToTool& operator=(const RobotToTool&);

 protected:
    int sockfd;
    int new_sockfd;
    int port;
    struct sockaddr_in server_addr;
    virtual void CreateSocket();
    virtual void SetAddr();
    virtual void Bind();
    virtual void Listen();
    virtual void Accept();
	virtual int Write(int sockfd, void* ptr, size_t length);

public:
	void closeHandler(void);
};

#endif // _RobotToTool_h_DEFINED
