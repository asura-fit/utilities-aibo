//
// RobotToTool.cc
//
// Copyright (C) 2001 Sony Corporation
// All Rights Reserved.
//

#ifndef Apertos
#include <OPENR/core_macro.h>
#else // Apertos
#include <OPENR/core_macro.h>
#endif // Apertos

#include "RobotToTool.h"

#ifdef _OPENR_DEBUG
#include <iostream.h>
#endif // _OPENR_DEBUG

#include <vector>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#define AIMON_PORT_ENV	"AIMON_PORT"
#define IP "131.206.93.202"
#define PORT 8000

#if LOGOUTPUT
#define HEADER true
#else
#define HEADER false
#endif

#define HSIZE 4

static RobotToTool* sThisObject;

typedef std::vector<class Socket*> SocketPool;

void handleSIGPIPE(int signal) {
	cout << "catch SIGPIPE" << endl;
	sThisObject->closeHandler();
}

RobotToTool::RobotToTool(void) : active_(false) {
	cout << "RobotToTool" << endl;
	sThisObject = this;

	if(signal(SIGPIPE, handleSIGPIPE) == SIG_ERR)
		cout << "signal failed." << endl;
}

RobotToTool::~RobotToTool() {}


OStatus
RobotToTool::DoInit(const OSystemEvent&)
{
#ifdef _OPENR_DEBUG
    cout << "RobotToTool::DoInit()" << endl;
#endif // _OPENR_DEBUG

    NewComData;
    SetComData;

    //init socket
	char* portEnv = getenv(AIMON_PORT_ENV);
	if(portEnv == NULL) {
	    port = PORT;
	} else {
		port = atoi(portEnv);
		if(port < 0)
			port = PORT;
	}
	cout << "default listen port = " << port << endl;
    CreateSocket();
    cout << "create socket" << endl;
    SetAddr();
    cout << "setaddr" << endl;
    Bind();
    cout << "bind" << endl;
    Listen();
    cout << "listen" << endl;
    Accept();
    cout << "accept" << endl;

    return oSUCCESS;
}


OStatus
RobotToTool::DoStart(const OSystemEvent&)
{
#ifdef _OPENR_DEBUG
    cout << "RobotToTool::DoStart()" << endl;
#endif // _OPENR_DEBUG

    /*
	  //init socket
	  CreateSocket();
	  SetAddr();
	  Bind();
	  Listen();
	  Accept();
    */

    active_ = true;
    ASSERT_TO_ALL_OBSERVER;
    
    return oSUCCESS;
}


OStatus
RobotToTool::DoStop(const OSystemEvent&)
{
#ifdef _OPENR_DEBUG
    cout << "RobotToTool::DoStop()" << endl;
#endif // _OPENR_DEBUG

    DEASSERT_TO_ALL_OBSERVER;
    active_ = false;

    return oSUCCESS;
}


OStatus
RobotToTool::DoDestroy(const OSystemEvent&)
{
#ifdef _OPENR_DEBUG
    cout << "RobotToTool::DoDestroy()" << endl;
#endif // _OPENR_DEBUG

    //close socket
    close(sockfd);

    DeleteComData;

    return oSUCCESS;
}


void
RobotToTool::Ready(const OReadyEvent& event)
{
#ifdef _OPENR_DEBUG
    cout << "RobotToTool::Ready() ";

    if (event.IsAssert()) {
		cout << "ASSERT_READY";
    } else if (event.IsDeassert()) {
		cout << "DEASSERT_READY";
    } else {
		cout << "UNKNOWN COMMAND";
    }
    cout << endl;
#endif // _OPENR_DEBUG

    if (active_) {
		; // do something if necessary
    } else {
		; // better to ignore
    }

    return;
}


void
RobotToTool::Update(const ONotifyEvent& event)
{
	bool close_flag = false;
#ifdef _OPENR_DEBUG
	cout << "RobotToTool::Update() received "
		 << event.NumOfData() << " data."<< endl;
#endif

	if (active_) {
      
		for(int i=0;i<event.NumOfData();i++) {
			RCRegion* region = event.RCData(i);
	
			char* data = (char *)region->Base();
			//int sizeofdata = region->Size();
			int sizeofdata = strlen(data);
	
#ifdef _OPENR_DEBUG
			cout << "length=" << sizeofdata << endl;
			cout << "data=" << data << endl;
#endif 
	
			if (HEADER) { 
				//write size of data
				char length[HSIZE];
				sprintf(length, "%d", sizeofdata);
				if(Write(sockfd, length, HSIZE) == 0)
					close_flag = true;
			}

			//write data
			if(close_flag == false) {
				if(Write(new_sockfd, data, sizeofdata) == 0)
					close_flag = true;
			}
      
		}
		observer[event.ObsIndex()]->AssertReady(event.SenderID());
	}

	if(close_flag)
		Accept();
    
	return;
}


void
RobotToTool::CreateSocket()
{
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("create socket");
		exit(1);
	}
}


void
RobotToTool::SetAddr()
{
	bzero((char *)&server_addr, sizeof(server_addr));
	server_addr.sin_family = PF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port);
}

void
RobotToTool::Bind()
{
	while (bind(sockfd, (struct sockaddr *)&server_addr,
				sizeof(server_addr)) < 0) {
		perror("bind");
		port++;
		CreateSocket();
		SetAddr();
	}
	cout << "Bind PORT=" << port << endl;
}


void 
RobotToTool::Listen()
{
	if (listen(sockfd, 5) < 0) {
		perror("listen");
		close(sockfd);
		exit(1);
	}
}


void
RobotToTool::Accept()
{
	struct sockaddr_in writer_addr;
	int writer_len;

	memset(&writer_addr, 0, sizeof(sockaddr_in));

	/*  
	if ((new_sockfd = accept(sockfd,(struct sockaddr *)&writer_addr,
							 (socklen_t *)&writer_len)) < 0) {
	*/
	if ((new_sockfd = accept(sockfd,NULL,NULL)) < 0) {
		perror("accept");
		exit(1);
	}
	printf("connected");
}

int 
RobotToTool::Write(int sockfd, void* ptr, size_t length)
{
	cout << "RobotToTool::Write()" << endl;
	int left;
	int n;
	int rc;

	left = length;
	n = 0;
	do {
		rc = write(sockfd, ptr, left);
		if(sockfd < 0) {
			cout << "SIGPIPE handler called" << endl;
			return 0;
		}

		//printf("write sys-call : %d\n", rc);
		cout << "Write sys-call: " << rc << endl;
		if(rc < 0) {                        
			if(errno == EPIPE) {
				// 途中でコネクションが切れた場合は SIGPIPE が
				// 発生して, errno == EPIPE で write システムコール
				// が終了
				return 0;
			} else if(errno != EINTR) {
				cout << "Errno = " << (unsigned int )errno << endl;
				exit(1);//return -1;
			}
		} else if(rc == 0) {
			return 0;
		} else {                  
			n += rc;
			left -= rc;
			ptr = (void *) ((char* )ptr + rc);         
		}      
	} while(left > 0);

	return length;
}

void
RobotToTool::closeHandler(void) {
	cout << "closeHandler" << endl;
	new_sockfd = -1;
}
