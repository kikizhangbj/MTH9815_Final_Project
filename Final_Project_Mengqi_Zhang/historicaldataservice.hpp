/**
 * historicaldataservice.hpp
 * historicaldataservice.hpp
 *
 * @author Breman Thuraisingham
 * Defines the data types and Service for historical data.
 *
 * @author Breman Thuraisingham
 */
#ifndef HISTORICAL_DATA_SERVICE_HPP
#define HISTORICAL_DATA_SERVICE_HPP
#include "positionservice.hpp"
#include "executionservice.hpp"

//convert PricingSide to string
string PricingSideOutput(PricingSide side);

//convert OrderType to string
string OrderTypeOutput(OrderType type);

//convert Side to string
string SideOutput(Side side);

//convert InquiryState to string
string StateOutput(InquiryState state);

//return bucket sectors giving its CUSIP
string BondExpity(string cusip);

ofstream f1;
ofstream f2;

/**
 * Service for processing and persisting historical data to a persistent store.
 * Keyed on some persistent key.
 * Type T is the data type to persist.
 */
template<typename T>
class HistoricalDataService : public virtual Service<string,T>
{
    
public:
    
    // Persist data to a store
    virtual void PersistData(string persistKey, T& data) = 0;
    
};

string PricingSideOutput(PricingSide side)
{
    if (side == BID)
        return "BID";
    else return "OFFER";
}

string OrderTypeOutput(OrderType type)
{
    if (type == FOK)
        return "FOK";
    else if (type == IOC)
        return "IOC";
    else if (type == MARKET)
        return "MARKET";
    else if (type == LIMIT)
        return "LIMIT";
    else return "STOP";
}

string SideOutput(Side side)
{
    if (side == BUY)
        return "BUY";
    else return "SELL";
}

string StateOutput(InquiryState state)
{
    if (state == RECEIVED)
        return "RECEIVED";
    else if (state == QUOTED)
        return "QUOTED";
    else if (state == DONE)
        return "DONE";
    else if (state == REJECTED)
        return "REJECTED";
    else return "CUSTOMER_REJECTED";
}

string BondExpiry(string cusip)
{
    if (cusip == "912828M72" || cusip == "912828N22" || cusip == "912828M98")
    {
        return "FrontEnd";
    }
    else if (cusip == "912828M80" || cusip == "912828M56")
    {
        return "Belly";
    }
    else if (cusip == "912810RP5")
    {
        return "LongEnd";
    }
    return "Other";
}


class BondHistoricalPositionDataConnector : public Connector< Position<Bond> >
{
public:
    void Publish(Position<Bond>& data);
};

class BondHistoricalPositionDataService : public HistoricalDataService< Position<Bond> >
{
    int num;//key; keep track of the number of output
    BondHistoricalPositionDataConnector conn;
public:
    //constructors
    BondHistoricalPositionDataService():num(1){}
    BondHistoricalPositionDataService(BondHistoricalPositionDataConnector _input) : conn(_input), num(1){}
    
    //the objects the class received are persisted back into txt files through connector
    //no stored listeners
    Position<Bond>& GetData(string key) override {exit(-1);}
    void AddListener(ServiceListener< Position<Bond> >* listener) override {}
    const vector< ServiceListener< Position<Bond> >*>& GetListeners() const override {exit(-1);}
    
    void OnMessage(Position<Bond>& data) override;
    void PersistData(string persistKey, Position<Bond>& data) override;
};

void BondHistoricalPositionDataService::OnMessage(Position<Bond> &data)
{
    string persistKey = to_string(num);
    this->PersistData(persistKey, data);
}

void BondHistoricalPositionDataService::PersistData(string persistKey, Position<Bond>& data)
{
    if (num == 1)
    {
        f2.close();
        f2.open("position.txt");
        f2 << setw(5) << "Key"
        << setw(13) << "productID"
        << setw(10) << "Coupon"
        << setw(15) << "Maturity Date"
        << setw(20) << "Aggregate Position"
        << setw(10) << "TRSY1"
        << setw(10) << "TRSY2"
        << setw(10) <<  "TRSY3"
        << endl;
    }
    f2 << setw(5) << persistKey;
    num++;
    conn.Publish(data);
}

void BondHistoricalPositionDataConnector::Publish(Position<Bond>& data)
{
    vector<string> book = {"TRSY1", "TRSY2", "TRSY3"};
    f2 << setw(13) << data.GetProduct().GetProductId()
    << setw(10) << data.GetProduct().GetCoupon()
    << "    " << data.GetProduct().GetMaturityDate()
    << setw(20) << data.GetAggregatePosition()
    << setw(10) << data.GetPosition(book[0])
    << setw(10) << data.GetPosition(book[1])
    << setw(10) << data.GetPosition(book[2])
    << endl;
}


class BondHistoricalRiskDataConnector : public Connector< vector< PV01<Bond> > >
{
public:
    void Publish(vector< PV01<Bond> >& data);
};

class BondHistoricalRiskDataService : public HistoricalDataService< vector< PV01<Bond> > >
{
    int num;//key number
    BondHistoricalRiskDataConnector conn;
public:
    //constructors
    BondHistoricalRiskDataService():num(1){}
    BondHistoricalRiskDataService(BondHistoricalRiskDataConnector _input) : conn(_input), num(1){}
    
    //the objects the class received are persisted back into txt files through connector
    //no stored listeners
    vector<PV01<Bond>>& GetData(string key) override {exit(-1);}
    void AddListener(ServiceListener<vector<PV01<Bond>>>* listener) override {}
    const vector<ServiceListener<vector<PV01<Bond>>>*>& GetListeners() const override {exit(-1);}
    
    void OnMessage(vector<PV01<Bond>>& data);
    void PersistData(string persistKey, vector<PV01<Bond>>& data);
};
    
void BondHistoricalRiskDataService::OnMessage(vector< PV01<Bond> >& data)
{
    string persistKey = to_string(num);
    this->PersistData(persistKey, data);
}

void BondHistoricalRiskDataService::PersistData(string persistKey, vector< PV01<Bond> >& data)
{
    if (num == 1)
    {
        f1.close();
        f1.open("risk.txt");
        f1 << setw(5) << "Key"
        << setw(20) << "FrontEnd Risk"
        << setw(20) << "Belly Risk"
        << setw(20) << "LongEnd Risk"
        << "    " << "ProductID    Coupon      Maturity Date  Total Risk"
        << endl;
    }
    f1 << setw(5) << persistKey;
    ++num;
    conn.Publish(data);
}

void BondHistoricalRiskDataConnector::Publish(vector< PV01<Bond> >& data)
{
    double front_end_risk(0.0), belly_risk(0.0), long_end_risk(0.0);
    
    for (long i = 0; i < data.size(); i++)
    {
        if (BondExpiry(data[i].GetProduct().GetProductId()) == "FrontEnd")
        {
            front_end_risk += data[i].GetQuantity() * data[i].GetPV01();
        }
        else if (BondExpiry(data[i].GetProduct().GetProductId()) == "Belly")
        {
            belly_risk += data[i].GetQuantity() * data[i].GetPV01();
        }
        else if (BondExpiry(data[i].GetProduct().GetProductId()) == "LongEnd")
        {
            long_end_risk += data[i].GetQuantity() * data[i].GetPV01();
        }
    }
    
    f1 << fixed
    << setw(20) << front_end_risk
    << setw(20) << belly_risk
    << setw(20) << long_end_risk;
    
    for (long i = 0; i < data.size(); i++)
    {
        if (i == 0)
        {
            f1 << "    ";
        }
        else
        {
            f1 << setw(78);
        }
        f1 << fixed  << data[i].GetProduct().GetProductId()
        << "    " << data[i].GetProduct().GetCoupon()
        << "    " << data[i].GetProduct().GetMaturityDate()
        << "    " <<  data[i].GetQuantity() * data[i].GetPV01()
        << endl;
    }
    
}


class BondHistoricalExecutionDataConnector : public Connector< ExecutionOrder<Bond> >
{
public:
    void Publish(ExecutionOrder<Bond>& data);
};

class BondHistoricalExecutionDataService : public HistoricalDataService< ExecutionOrder<Bond> >
{
    int num;//Key number
    BondHistoricalExecutionDataConnector conn;

public:
    //constructors
    BondHistoricalExecutionDataService():num(1){}
    BondHistoricalExecutionDataService(BondHistoricalExecutionDataConnector _input) : conn(_input), num(1){}
    
    //the objects the class received are persisted back into txt files through connector
    //no stored listeners
    ExecutionOrder<Bond>& GetData(string key) override {exit(-1);}
    void AddListener(ServiceListener< ExecutionOrder<Bond> >* listener) override {}
    const vector< ServiceListener< ExecutionOrder<Bond> >*>& GetListeners() const override {exit(-1);}

    void OnMessage(ExecutionOrder<Bond>& data);
    void PersistData(string persistKey, ExecutionOrder<Bond>& data);
};

void BondHistoricalExecutionDataService::OnMessage(ExecutionOrder<Bond> &data)
{
    string persistKey = to_string(num);
    this->PersistData(persistKey, data);
}

void BondHistoricalExecutionDataService::PersistData(string persistKey, ExecutionOrder<Bond>& data)
{
    if (num == 1)
    {
        f1.close();
        f1.open("executions.txt");
        f1 << setw(5) << "Key"
        << setw(15) << "ProductID"
        << setw(10) << "Side"
        << setw(10) << "OrderID"
        << setw(13) << "OrderType"
        << setw(10) << "Price"
        << setw(18) << "VisibleQuantity"
        << setw(18) << "HiddenQuantity"
        << setw(18) << "ParentOrderID"
        << setw(15) << "IsChildOrder"
        << endl;
    }
    
    f1 << setw(5) << persistKey;
    ++num;
    conn.Publish(data);
}

void BondHistoricalExecutionDataConnector::Publish(ExecutionOrder<Bond>& data)
{
    f1 << setw(15) << data.GetProduct().GetProductId()
    << setw(10) << PricingSideOutput(data.GetSide())
    << setw(10) << data.GetOrderId()
    << setw(13) << OrderTypeOutput(data.GetOrderType())
    << setw(10) << FractionalBondPrice(data.GetPrice())
    << setw(18) << data.GetVisibleQuantity()
    << setw(18) << data.GetHiddenQuantity()
    << setw(18) << data.GetParentOrderId()
    << setw(15) << "FALSE"
    << endl;
}


class BondHistoricalStreamingDataConnector : public Connector<PriceStream<Bond>>
{
public:
    void Publish(PriceStream<Bond>& data);
};

class BondHistoricalStreamingDataService : public HistoricalDataService< PriceStream<Bond> >
{
    int num;//Key number
    BondHistoricalStreamingDataConnector conn;
public:
    //ctor
    BondHistoricalStreamingDataService(): num(1){}
    BondHistoricalStreamingDataService(BondHistoricalStreamingDataConnector _input) : conn(_input),num(1){}
    
    //the objects the class received are persisted back into txt files through connector
    //no stored listeners
    PriceStream<Bond>& GetData(string key) override {exit(-1);}
    void AddListener(ServiceListener< PriceStream<Bond> >* listener) override {}
    const vector< ServiceListener< PriceStream<Bond> >*>& GetListeners() const override {exit(-1);}

    void OnMessage(PriceStream<Bond>& data);
    void PersistData(string persistKey, PriceStream<Bond>& data);
};

void BondHistoricalStreamingDataService::OnMessage(PriceStream<Bond> &data)
{
    string persistKey = to_string(num);
    this->PersistData(persistKey, data);
}

void BondHistoricalStreamingDataService::PersistData(string persistKey, PriceStream<Bond>& data)
{
    if (num == 1)
    {
        f1.close();
        f1.open("streaming.txt");
        f1 << setw(5) << "Key"
        << setw(15) << "ProductID"
        << setw(15) << "Bid Price"
        << setw(15) << "Quantity"
        << setw(15) << "Offer Price"
        << setw(15) << "Quantity"
        << endl;
    }
    f1 << setw(5) << persistKey;
    ++num;
    conn.Publish(data);
}

void BondHistoricalStreamingDataConnector::Publish(PriceStream<Bond>& data)
{
    f1 << setw(15) << data.GetProduct().GetProductId()
    << setw(15) << FractionalBondPrice(data.GetBidOrder().GetPrice())
    << setw(15) << data.GetBidOrder().GetVisibleQuantity()
    << setw(15) << FractionalBondPrice(data.GetOfferOrder().GetPrice())
    << setw(15) << data.GetOfferOrder().GetVisibleQuantity()
    << endl;
}


class BondHistoricalInquiryDataConnector : public Connector< Inquiry<Bond> >
{
public:
    void Publish(Inquiry<Bond>& data);
};

class BondHistoricalInquiryDataService : public HistoricalDataService< Inquiry<Bond> >
{
    int num;//key number
    BondHistoricalInquiryDataConnector conn;
public:
    //constructors
    BondHistoricalInquiryDataService():num(1){}
    BondHistoricalInquiryDataService(BondHistoricalInquiryDataConnector _input) : conn(_input), num(1){}

    //the objects the class received are persisted back into txt files through connector
    //no stored listeners
    Inquiry<Bond>& GetData(string key) override {exit(-1);}
    void AddListener(ServiceListener< Inquiry<Bond> >* listener) override {}
    const vector< ServiceListener< Inquiry<Bond> >*>& GetListeners() const override {exit(-1);}
    
    void OnMessage(Inquiry<Bond>& data);
    void PersistData(string persistKey, Inquiry<Bond>& data);
};

void BondHistoricalInquiryDataService::OnMessage(Inquiry<Bond>& data)
{
    string persistKey = to_string(num);
    this->PersistData(persistKey, data);
}

void BondHistoricalInquiryDataService::PersistData(string persistKey, Inquiry<Bond>& data)
{
    if (num == 1)
    {
        f1.close();
        f1.open("allinquires.txt");
        f1 << setw(5) << "Key"
        << setw(13) << "InquiryID"
        << setw(15) << "ProductID"
        << setw(10) << "Side"
        << setw(15) << "Quantity"
        << setw(15) << "Price"
        << setw(10) << "State"
        << endl;
    }
    f1 << setw(5) << persistKey;
    ++num;
    conn.Publish(data);
}

void BondHistoricalInquiryDataConnector::Publish(Inquiry<Bond>& data)
{
    f1 << setw(13) << data.GetInquiryId()
    << setw(15) << data.GetProduct().GetProductId()
    << setw(10) << SideOutput(data.GetSide())
    << setw(15) << data.GetQuantity()
    << setw(15) << FractionalBondPrice(data.GetPrice())
    << setw(10) << StateOutput(data.GetState())
    << endl;
}

#endif
