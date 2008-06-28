//
// StdoutHub.h
//
// $Log: StdoutHub.h,v $
// Revision 1.2  2003/06/09 09:48:06  ken
// add: AiboTools, the integration of AiboProxy and AiboTermServ
//
// Revision 1.1.2.1  2003/05/21 21:34:49  ken
// add: integrate AiboProxy and AiboTermServ
//
// Revision 1.2  2002/11/28 17:56:34  isshi
// Merge sdk-firt-release to HEAD
//
// Revision 1.1.2.1  2002/10/18 07:23:28  ken
// AiboTermServ introduced
//
// Revision 1.3  2002/03/20 20:35:30  ken
//  Append Log macro
//
//

#ifndef _StdoutHub_h_DEFINED
#define _StdoutHub_h_DEFINED

#include <OPENR/OObject.h>
#ifndef Apertos
#include <OPENR/OSubject.h>
#include <OPENR/OObserver.h>
#else // Apertos
#include <OPENR/OSubject.h>
#include <OPENR/OObserver.h>
#endif // Apertos
#include "def.h"


class StdoutHub : public OObject {
  public:
    // OPEN-R Inter-Object Communication
    OSubject*  subject[numOfSubject];
    OObserver* observer[numOfObserver];
    int        mFD;

  public:
    // public methods
    StdoutHub(void);
    ~StdoutHub();

    // OPEN-R Object Common Interface
    virtual OStatus DoInit   (const OSystemEvent&);
    virtual OStatus DoStart  (const OSystemEvent&);
    virtual OStatus DoStop   (const OSystemEvent&);
    virtual OStatus DoDestroy(const OSystemEvent&);

    void Update(const ONotifyEvent&);
    void UpdateToFile(const ONotifyEvent&);
    void Ready(const OReadyEvent&);

  private:
    // private member
    bool active_;

  private:
    // will never be implemented
    StdoutHub(const StdoutHub&);
    StdoutHub& operator=(const StdoutHub&);
};


#endif // _StdoutHub_h_DEFINED
