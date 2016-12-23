//
//  ExecutionListener.hpp
//  Final_Project_Mengqi_Zhang
//
//  Created by Mengqi Zhang on 12/22/16.
//  Copyright © 2016 Mengqi Zhang. All rights reserved.
//

#ifndef ExecutionListener_h
#define ExecutionListener_h

#include "historicaldataservice.hpp"

class ExecutionListener: public ServiceListener<ExecutionOrder<Bond>>
{
    BondHistoricalExecutionDataService & historical_execution;
public:
    ExecutionListener(BondHistoricalExecutionDataService& input): historical_execution(input){}
    
    void ProcessAdd(ExecutionOrder<Bond>& order) override;
    void ProcessRemove(ExecutionOrder<Bond>& order) override {}
    void ProcessUpdate(ExecutionOrder<Bond>& order) override {}
};

void ExecutionListener::ProcessAdd(ExecutionOrder<Bond>& order)
{
    historical_execution.OnMessage(order);
}

#endif /* ExecutionListener_h */
