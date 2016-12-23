//
//  main.cpp
//  Final_Project_Mengqi_Zhang
//
//  Created by Mengqi Zhang on 12/9/16.
//  Copyright © 2016 Mengqi Zhang. All rights reserved.
//

#include <iostream>
#include "tradebookingservice.hpp"
#include "positionservice.hpp"
#include "BondTradeServiceListener.hpp"
#include "BondPositionServiceListener.hpp"
#include "riskservice.hpp"
#include "BondPositionServiceListener.hpp"
#include "pricingservice.hpp"
#include "BondAlgoStreamingService.hpp"
#include "BondPricingListener.hpp"
#include "BondMarketDataListener.hpp"
#include "inquiryservice.hpp"
#include "historicaldataservice.hpp"
#include "PositionDataListener.hpp"
#include "RiskListener.hpp"
#include "ExecutionListener.hpp"
#include "StreamingListener.hpp"
#include "InquiryListener.hpp"

int main()
{
    //A.Test tradebookservice & positionservice & riskservice
    BondTradeBookingService trade_srv;
    //input data to trade_srv through trade_conn
    BondTradeBookingConnector trade_conn(trade_srv);
    
    BondPositionService position_srv;
    //input data to position_srv through trade_listener
    BondTradeServiceListener trade_listener(position_srv);
    trade_srv.AddListener(&trade_listener);
    
    BondRiskService risk_srv;
    //input data through position_listener
    BondPositionServiceListener position_listener(risk_srv);
    position_srv.AddListener(&position_listener);
    
    //read trades.txt into trade_srv
    //trade_conn.ReadFile("/Users/kikizhang/Desktop/Input_Files/trades.txt");
    
    
    //B.Test pricingservice & streaming service
    BondPricingService price_srv;
    //input data to price_srv through price_conn
    BondPricingConnector price_conn(price_srv);
    
    BondStreamingService streaming_srv;
    BondAlgoStreamingService algo_streaming_srv(streaming_srv);
    BondPricingListener position_srv_listener(algo_streaming_srv);
    price_srv.AddListener(&position_srv_listener);
    
    //flow data from prices.txt into price_srv
    //price_conn.ReadFile("/Users/kikizhang/Desktop/Input_Files/prices.txt");
    
    //C. Test marketdataservice & execution service
    BondMarketDataService market_data_srv;
    BondMarketDataConnector market_data_conn(market_data_srv);
    
    BondExecutionService execution_srv;
    BondAlgoExecutionService algo_execution_srv(execution_srv);
    //input data through marketdatalistener
    MarketDataListener market_data_listener(algo_execution_srv);
    market_data_srv.AddListener(&market_data_listener);
    
    //flow data from marketdata.txt into market_data_srv
    //market_data_conn.ReadFile("/Users/kikizhang/Desktop/Input_Files/marketdata.txt");
    
    //D. Test inquiryservice
    BondInquiryService inquiry_srv;
    BondInquiryConnector inquiry_conn(inquiry_srv);
    //inquiry_conn.ReadFile("/Users/kikizhang/Desktop/Input_Files/inquiries.txt");
    
    //E. Test historicaldataservice
    BondHistoricalPositionDataConnector his_position_conn;
    BondHistoricalPositionDataService his_position_serv(his_position_conn);
    PositionDataListener his_position_listener(his_position_serv);
    position_srv.AddListener(&his_position_listener);
    
    BondHistoricalRiskDataConnector his_risk_conn;
    BondHistoricalRiskDataService his_risk_srv(his_risk_conn);
    RiskListener his_risk_listener(his_risk_srv);
    risk_srv.AddHistoricalDataListener(&his_risk_listener);
    
    BondHistoricalExecutionDataConnector his_execution_conn;
    BondHistoricalExecutionDataService his_execution_svr(his_execution_conn);
    ExecutionListener his_execution_listener(his_execution_svr);
    execution_srv.AddListener(&his_execution_listener);
    
    BondHistoricalStreamingDataConnector his_streaming_conn;
    BondHistoricalStreamingDataService his_streaming_srv(his_streaming_conn);
    StreamingListener his_streaming_listener(his_streaming_srv);
    streaming_srv.AddListener(&his_streaming_listener);
    
    BondHistoricalInquiryDataConnector his_inquiry_conn;
    BondHistoricalInquiryDataService his_inquiry_srv(his_inquiry_conn);
    InquiryListener his_inquiry_listener(his_inquiry_srv);
    inquiry_srv.AddListener(&his_inquiry_listener);
    
    market_data_conn.ReadFile("marketdata.txt");
    trade_conn.ReadFile("trades.txt");
    price_conn.ReadFile("prices.txt");
    inquiry_conn.ReadFile("inquiries.txt");
    
    return 0;
}






