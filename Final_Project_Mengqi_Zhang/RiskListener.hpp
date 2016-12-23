//
//  RiskListener.hpp
//  Final_Project_Mengqi_Zhang
//
//  Created by Mengqi Zhang on 12/22/16.
//  Copyright © 2016 Mengqi Zhang. All rights reserved.
//

#ifndef RiskListener_h
#define RiskListener_h
#include "historicaldataservice.hpp"

class RiskListener: public ServiceListener<vector<PV01<Bond>>>
{
    BondHistoricalRiskDataService& historical_risk;
public:
    RiskListener(BondHistoricalRiskDataService& input): historical_risk(input){}
    
    void ProcessAdd(vector<PV01<Bond>> &v);
    void ProcessRemove(vector<PV01<Bond>> &v){}
    void ProcessUpdate(vector<PV01<Bond>> &v){}
};

void RiskListener::ProcessAdd(vector<PV01<Bond>> &v)
{
    historical_risk.OnMessage(v);
}

#endif /* RiskListener_h */
