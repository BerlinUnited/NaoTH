/**
* @file BDRNaoQi.h
* 
* Declaration of class BDRNaoQi
*
* @author <a href="mailto:kaden@informatik.hu-berlin.de">Kaden, Steffen</a>
*/ 

#ifndef _BDR_NAO_QI_H
#define _BDR_NAO_QI_H

#include <Tools/DataStructures/Printable.h>

class BDRNaoQiRequest : public naoth::Printable
{
public:
    BDRNaoQiRequest():
        behavior(none)
    {}

    enum BDRNaoQiBehavior{
        entertainment,
        sit,
        none
    } behavior;

    static std::string getName(BDRNaoQiBehavior b){
        switch(b) {
            case entertainment : return "entertainment";
            case sit           : return "sit";
            case none          : return "none";
        }
        return "unknown";
    }

    virtual void print(std::ostream &stream) const
    {
        stream << "behavior:" << getName(behavior) << std::endl;
    }
};

class BDRNaoQiStatus : public BDRNaoQiRequest{
};

#endif //__BDR_NAO_QI_H
