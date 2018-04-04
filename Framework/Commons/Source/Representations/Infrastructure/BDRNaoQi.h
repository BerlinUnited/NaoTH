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
        behavior(none),
        disable_DCM_writings(false)
    {}

    enum BDRNaoQiBehavior{
        none,
        entertainment,
        sit
    } behavior;

    bool disable_DCM_writings;

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
        stream << "disable_DCM_writing:" << disable_DCM_writings << std::endl;
    }
};

class BDRNaoQiStatus : public BDRNaoQiRequest{
public:
    void get(BDRNaoQiStatus& data) const {
        data = *this;
    }
};

#endif //__BDR_NAO_QI_H
