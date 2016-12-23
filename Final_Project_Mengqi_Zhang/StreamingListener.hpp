//
//  StreamingListener.hpp
//  Final_Project_Mengqi_Zhang
//
//  Created by Mengqi Zhang on 12/22/16.
//  Copyright © 2016 Mengqi Zhang. All rights reserved.
//

#ifndef StreamingListener_h
#define StreamingListener_h
#include "historicaldataservice.hpp"

class StreamingListener: public ServiceListener<PriceStream<Bond>>
{
    BondHistoricalStreamingDataService &historical_stream;
public:
    StreamingListener(BondHistoricalStreamingDataService& input): historical_stream(input){}
    
    void ProcessAdd(PriceStream<Bond> & price_stream);
    void ProcessRemove(PriceStream<Bond> & price_stream){}
    void ProcessUpdate(PriceStream<Bond> & price_stream){}
};

void StreamingListener::ProcessAdd(PriceStream<Bond>& price_stream)
{
    historical_stream.OnMessage(price_stream);
}

#endif /* StreamingListener_h */
