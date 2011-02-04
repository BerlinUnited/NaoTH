/* 
 * File:   CognitionThread.h
 * Author: Oliver Welter
 *
 * Created on 12. Juli 2008, 21:14
 */

#ifndef _COGNITIONTHREAD_H
#define	_COGNITIONTHREAD_H

#include <rttools/rtthread.h>

namespace naoth
{
class CognitionThread : public RtThread
{
	public:
  	CognitionThread();
	  ~CognitionThread();
	  virtual void *execute();
 		virtual void postExecute();
		virtual void preExecute();

    void stop() { stopping = true; }

private:
  bool stopping;
};
} // end namespace naoth;
#endif	/* _COGNITIONTHREAD_H */

