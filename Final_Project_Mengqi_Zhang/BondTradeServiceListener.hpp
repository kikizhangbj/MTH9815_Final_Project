//
//  BondTradeServiceListener.hpp
//  Final_Project_Mengqi_Zhang
//
//  A listener class inherited from ServiceListener<Trade<Bond>>
//  that linked BondTradeBookingService to BondPositionService in ProcessAdd Method
//
//  Created by Mengqi Zhang on 12/17/16.
//  Copyright © 2016 Mengqi Zhang. All rights reserved.
//

#ifndef BondTradeServiceListener_h
#define BondTradeServiceListener_h

#include "positionservice.hpp"

class BondTradeServiceListener: public ServiceListener<Trade<Bond>>
{
private:
    BondPositionService &position_service;
public:
    BondTradeServiceListener(BondPositionService& input);
    void ProcessAdd(Trade<Bond> & trade);
    void ProcessRemove(Trade<Bond> & trade){};
    void ProcessUpdate(Trade<Bond> & trade){};
};

BondTradeServiceListener::BondTradeServiceListener(BondPositionService & input):position_service(input){}

void BondTradeServiceListener::ProcessAdd(Trade<Bond> & trade){
    position_service.AddTrade(trade);
}
#endif /* BondTradeServiceListener_h */
