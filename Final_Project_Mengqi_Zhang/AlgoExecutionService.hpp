//
//  AlgoExecutionService.hpp
//  Final_Project_Mengqi_Zhang
//
//  Created by Mengqi Zhang on 12/21/16.
//  Copyright © 2016 Mengqi Zhang. All rights reserved.
//

#ifndef AlgoExecutionService_h
#define AlgoExecutionService_h

#include "executionservice.hpp"
#include "products.hpp"

/*class AlgoExecution : public ExecutionOrder<Bond>
{
public:
    AlgoExecution(const string& bondid, PricingSide _side, int _order_num, double _price, long quantity):ExecutionOrder(Bond(bondid), _side, "T"+to_string(_order_num), MARKET, _price, quantity, 0, "NULL", false){};
};*/


class BondAlgoExecutionService: public Service<string, OrderBook<Bond>>
{
private:
    BondExecutionService &execution_service;
    int ordernum;
    map<string, PricingSide> pricing_sides;
    vector<ServiceListener<OrderBook<Bond>>*> listener_list;
public:
    BondAlgoExecutionService(BondExecutionService& input): execution_service(input), ordernum(int(1)){}
    OrderBook<Bond>& GetData(string orderId) override {exit(-1);};
    void OnMessage(OrderBook<Bond>& orderbook) override;
    void AddListener(ServiceListener<OrderBook<Bond>>* listener) override;
    const vector<ServiceListener<OrderBook<Bond>>*>& GetListeners() const override;
};

void BondAlgoExecutionService::OnMessage(OrderBook<Bond>& orderbook)
{
    string productid = orderbook.GetProduct().GetProductId();
    auto iter = pricing_sides.find(productid);
    if(iter == pricing_sides.end())
    {
        Bond bond(productid);
        PricingSide side = BID;
        string orderId = "T" + to_string(ordernum);
        OrderType order_type = MARKET;
        double price = orderbook.GetOfferStack()[0].GetPrice();
        long quantity = orderbook.GetOfferStack()[0].GetQuantity();
        string parentorderId = "NULL";
        ExecutionOrder<Bond> new_order(bond, side, orderId, order_type, price, quantity, 0, parentorderId, false);
        execution_service.ExecuteOrder(new_order);
        execution_service.OnMessage(new_order);
        pricing_sides[productid] = OFFER;
        
        ++ordernum;
        
        //test
        //cout<< ordernum-1<<endl;
        return;
    }
    else
    {
        Bond bond(productid);
        PricingSide side = iter->second;
        string orderid = "T" + to_string(ordernum);
        double price = iter->second == BID? orderbook.GetOfferStack()[0].GetPrice(): orderbook.GetBidStack()[0].GetPrice();
        long quantity = iter->second==BID? orderbook.GetOfferStack()[0].GetQuantity(): orderbook.GetBidStack()[0].GetQuantity();
        ExecutionOrder<Bond> new_order(bond, side, orderid, MARKET, price, quantity, 0, "NULL", false);
        execution_service.ExecuteOrder(new_order);
        execution_service.OnMessage(new_order);
        
        ++ordernum;
        
        //test
        //cout<< ordernum-1<<endl;
    }
    if(iter->second == BID) iter->second = OFFER;
    else iter->second = BID;
}

void BondAlgoExecutionService::AddListener(ServiceListener<OrderBook<Bond> > *listener)
{
    listener_list.push_back(listener);
}

const vector<ServiceListener<OrderBook<Bond>>*>& BondAlgoExecutionService::GetListeners() const
{
    return listener_list;
}


#endif /* AlgoExecutionService_h */
