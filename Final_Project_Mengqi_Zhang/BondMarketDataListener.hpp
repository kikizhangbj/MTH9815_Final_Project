//
//  BondMarketDataListener.hpp
//  Final_Project_Mengqi_Zhang
//
//  Created by Mengqi Zhang on 12/21/16.
//  Copyright © 2016 Mengqi Zhang. All rights reserved.
//

#ifndef BondMarketDataListener_h
#define BondMarketDataListener_h

#include "marketdataservice.hpp"
#include "AlgoExecutionService.hpp"

class MarketDataListener: public ServiceListener<OrderBook<Bond>>
{
private:
    BondAlgoExecutionService & algo_execution_service;
public:
    MarketDataListener(BondAlgoExecutionService& input): algo_execution_service(input){}
    void ProcessAdd(OrderBook<Bond>& orderbook);
    void ProcessRemove(OrderBook<Bond>& orderbook){}
    void ProcessUpdate(OrderBook<Bond>& orderbook){}
};

void MarketDataListener::ProcessAdd(OrderBook<Bond>& orderbook)
{
    algo_execution_service.OnMessage(orderbook);
}

#endif /* BondMarketDataListener_h */
