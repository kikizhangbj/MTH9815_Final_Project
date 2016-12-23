//
//  BondPricingListener.hpp
//  Final_Project_Mengqi_Zhang
//
//  Created by Mengqi Zhang on 12/21/16.
//  Copyright © 2016 Mengqi Zhang. All rights reserved.
//

#ifndef BondPricingListener_h
#define BondPricingListener_h

#include "BondAlgoStreamingService.hpp"

class BondPricingListener : public ServiceListener<Price<Bond>>
{
    BondAlgoStreamingService& algo_streaming_service;
public:
    BondPricingListener(BondAlgoStreamingService& input): algo_streaming_service(input){}
    void ProcessAdd(Price<Bond>& price);
    void ProcessRemove(Price<Bond>& data){}
    void ProcessUpdate(Price<Bond>& data){}
};

void BondPricingListener::ProcessAdd(Price<Bond>& price)
{
    algo_streaming_service.OnMessage(price);
}

#endif /* BondPricingListener_h */
