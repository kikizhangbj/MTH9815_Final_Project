//
//  BondPositionServiceListener.hpp
//  Final_Project_Mengqi_Zhang
//
//  Created by Mengqi Zhang on 12/18/16.
//  Copyright © 2016 Mengqi Zhang. All rights reserved.
//

#ifndef BondPositionServiceListener_h
#define BondPositionServiceListener_h

#include "soa.hpp"
#include "riskservice.hpp"

class BondPositionServiceListener: public ServiceListener<Position<Bond>>
{
private:
    BondRiskService &risk_service;
public:
    BondPositionServiceListener(BondRiskService& input): risk_service(input){}
    void ProcessAdd(Position<Bond> &position);
    void ProcessRemove(Position<Bond> &position){}
    void ProcessUpdate(Position<Bond> &position){}
};

void BondPositionServiceListener::ProcessAdd(Position<Bond> &position)
{
    risk_service.AddPosition(position);
}

#endif /* BondPositionServiceListener_h */
