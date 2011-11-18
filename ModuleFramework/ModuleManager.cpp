/*
 * ModuleManager.cpp
 *
 *  Created on: Nov 12, 2011
 *      Author: dseifert
 */

#include "ModuleManager.h"

void ModuleManager::calculateExecutionList()
{

  bool changed = true;
  list<string>::iterator start=moduleExecutionList.begin();
  const int maxAttempts = moduleExecutionList.size()*10;
  int iterations = 0;

  while(changed && iterations < maxAttempts)
  {
    changed = false;
    iterations++;


    for(list<string>::iterator it1=start; it1 != moduleExecutionList.end(); it1++)
    {
      start = it1;
      if(!moduleExecutionMap[*it1]->isEnabled()) continue;
      Module* m1 = moduleExecutionMap[*it1]->getModule();

      for(list<string>::iterator it2=it1; it2 != moduleExecutionList.end(); it2++)
      {
        if(!moduleExecutionMap[*it2]->isEnabled()) continue;
        Module* m2 = moduleExecutionMap[*it2]->getModule();

        // m2 is currently executed after m1. Make sure that m2 does not provide a value
        // that m1 requires.
        for(std::list<Representation*>::const_iterator itReq = m1->getRequiredRepresentations().begin();
          itReq != m1->getRequiredRepresentations().end(); itReq++)
        {
          std::string repName = (*itReq)->getName();
          for(std::list<Representation*>::const_iterator r = m2->getProvidedRepresentations().begin();
            r != m2->getProvidedRepresentations().end(); ++r)
          {
            if((*r)->getName() == repName)
            {
              std::swap(*it1, *it2);
              changed = true;
              break;
            }
          }//end for
          if(changed) break;
        }//end for
        if(changed) break;

        // m2 is currently executed after m1. Make sure that m1 does not provide a value
        // that m2 recycles.
        for ( std::list<Representation*>::const_iterator itProv = m1->getProvidedRepresentations().begin();
              itProv != m1->getProvidedRepresentations().end();
              itProv++)
        {
          std::string repName = (*itProv)->getName();
          for ( std::list<Representation*>::const_iterator r = m2->getRecycledRepresentations().begin();
                r != m2->getRecycledRepresentations().end();
                ++r)
          {
            if ((*r)->getName() == repName)
            {
              std::swap(*it1, *it2);
              changed = true;
              break;
            }
          }//end for
          if(changed) break;
        }//end for
        if(changed) break;
      }//end for
      if(changed) break;
    }//end for

  }//end while

  if(iterations >= maxAttempts)
  {
    std::cerr << "WARNING: maximal number of iterations reached." << std::endl;
  }//end if


  // print execution list
  cout << "automatic module execution list" << endl;
  cout << "-------------------------------" << endl;
  for(list<string>::const_iterator itExec = moduleExecutionList.begin();
    itExec != moduleExecutionList.end(); itExec++
  )
  {
    cout << *itExec << endl;
  }
  cout << "-------------------------------" << endl;
  cout << endl;
}//end calculateExecutionListNew
