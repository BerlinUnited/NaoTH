/**
* @file GroundContactModel.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
*/

#ifndef _GroundContactModel_h_
#define _GroundContactModel_h_

class GroundContactModel
{
public:
  GroundContactModel() : leftGroundContact(false), rightGroundContact(false) {}

  bool leftGroundContact;
  bool rightGroundContact;
};

#endif // _GroundContactModel_h_
