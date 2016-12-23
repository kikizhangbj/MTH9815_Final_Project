//
//  PositionDataListener.hpp
//  Final_Project_Mengqi_Zhang
//
//  Created by Mengqi Zhang on 12/22/16.
//  Copyright © 2016 Mengqi Zhang. All rights reserved.
//

#ifndef PositionDataListener_h
#define PositionDataListener_h
#include "historicaldataservice.hpp"

class PositionDataListener: public ServiceListener<Position<Bond>>
{
    BondHistoricalPositionDataService& historical_position;
public:
    PositionDataListener(BondHistoricalPositionDataService& input): historical_position(input){}
    
    void ProcessAdd(Position<Bond>& position);
    void ProcessRemove(Position<Bond>& position) {}
    void ProcessUpdate(Position<Bond>& position) {}
};

void PositionDataListener::ProcessAdd(Position<Bond> & position)
{
    historical_position.OnMessage(position);
}

#endif /* PositionDataListener_h */
