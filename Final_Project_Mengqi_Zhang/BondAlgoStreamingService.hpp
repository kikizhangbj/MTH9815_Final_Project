//
//  BondAlgoStreamingService.hpp
//  Final_Project_Mengqi_Zhang
//
//  Created by Mengqi Zhang on 12/21/16.
//  Copyright © 2016 Mengqi Zhang. All rights reserved.
//

#ifndef BondAlgoStreamingService_h
#define BondAlgoStreamingService_h

#include "streamingservice.hpp"
#include "pricingservice.hpp"

class BondAlgoStreamingService: public Service<string, Price<Bond>>
{
    BondStreamingService& streaming_service;
    vector<ServiceListener<Price<Bond>>*> listener_list;
public:
    BondAlgoStreamingService(BondStreamingService& input): streaming_service(input){}
    Price<Bond>& GetData(string id) override {exit(-1);};
    void OnMessage(Price<Bond>& price) override;
    void AddListener(ServiceListener<Price<Bond>>* listener) override;
    const vector<ServiceListener<Price<Bond>>*>& GetListeners() const override;
};

void BondAlgoStreamingService::OnMessage(Price<Bond>& price)
{
    PriceStreamOrder bid_order(double(price.GetMid() - price.GetBidOfferSpread()/2), 10000000, 0, BID);
    PriceStreamOrder offer_order(double(price.GetMid()+price.GetBidOfferSpread()/2), 10000000, 0, OFFER);
    PriceStream<Bond> new_price_stream(price.GetProduct(), bid_order, offer_order);
    streaming_service.PublishPrice(new_price_stream);
}

void BondAlgoStreamingService::AddListener(ServiceListener<Price<Bond>>* listener)
{
    listener_list.push_back(listener);
}

const vector<ServiceListener<Price<Bond>>*>& BondAlgoStreamingService::GetListeners() const
{
    return listener_list;
}

#endif /* BondAlgoStreamingService_h */
