//
//  InquiryListener.hpp
//  Final_Project_Mengqi_Zhang
//
//  Created by Mengqi Zhang on 12/22/16.
//  Copyright © 2016 Mengqi Zhang. All rights reserved.
//

#ifndef InquiryListener_h
#define InquiryListener_h
#include "historicaldataservice.hpp"

class InquiryListener: public ServiceListener<Inquiry<Bond>>
{
    BondHistoricalInquiryDataService & historical_inquiry;
public:
    InquiryListener(BondHistoricalInquiryDataService& input): historical_inquiry(input){}
    void ProcessAdd(Inquiry<Bond> & inquiry);
    void ProcessRemove(Inquiry<Bond> & inquiry) {}
    void ProcessUpdate(Inquiry<Bond> & inquiry) {}
};

void InquiryListener::ProcessAdd(Inquiry<Bond> & inquiry)
{
    historical_inquiry.OnMessage(inquiry);
}


#endif /* InquiryListener_h */
