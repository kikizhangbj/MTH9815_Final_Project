/**
 * tradebookingservice.hpp
 * Defines the data types and Service for trade booking.
 *
 * @author Breman Thuraisingham
 */
#ifndef TRADE_BOOKING_SERVICE_HPP
#define TRADE_BOOKING_SERVICE_HPP

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include "soa.hpp"
#include "products.hpp"

using namespace std;

// Trade sides
enum Side { BUY, SELL };

/**
 * Trade object with a price, side, and quantity on a particular book.
 * Type T is the product type.
 */
template<typename T>
class Trade
{

public:

  // ctor for a trade
  Trade(const T &_product, string _tradeId, string _book, long _quantity, Side _side);

  // Get the product
  const T& GetProduct() const;

  // Get the trade ID
  const string& GetTradeId() const;

  // Get the book
  const string& GetBook() const;

  // Get the quantity
  long GetQuantity() const;

  // Get the side
  Side GetSide() const;

private:
  T product;
  string tradeId;
  string book;
  long quantity;
  Side side;

};

/**
 * Trade Booking Service to book trades to a particular book.
 * Keyed on product identifier.
 * Type T is the product type.
 */

template<typename T>
class TradeBookingService : public Service<string, Trade<T> >
{
  // Book the trade
    void BookTrade(const Trade<T> &trade){};

};

class BondTradeBookingService: public TradeBookingService<Bond>
{
private:
    vector<Trade<Bond>> Trades_Book;
    vector<ServiceListener<Trade<Bond>>*> Listeners_List;
public:
    BondTradeBookingService();
    Trade<Bond>& GetData(string _tradeId) override;
    void OnMessage(Trade<Bond> &trades) override;
    void AddListener(ServiceListener< Trade<Bond> > * listeners) override;
    const vector< ServiceListener<Trade<Bond>>*>& GetListeners() const override;
    // Book the trade
    void BookTrade(const Trade<Bond> &trade);
};

class BondTradeBookingConnector : public Connector<Trade<Bond>>
{
private:
    BondTradeBookingService &Trade_Service;
public:
    BondTradeBookingConnector (BondTradeBookingService&input):Trade_Service(input){/*cout<<"A trade booking connector is created!\n";*/}
    void ReadFile(string file);
    void Publish(Trade<Bond> &data){}
};

//Default constructor
BondTradeBookingService::BondTradeBookingService()
{
    /*cout<<"BondTradeBookingService is created!\n";*/
}

//Get data of the tradebook given the trade ID
Trade<Bond>& BondTradeBookingService::GetData(string _tradeId)
{
    for (auto itr = Trades_Book.begin(); itr != Trades_Book.end(); ++itr)
    {
        if ((*itr).GetTradeId() == _tradeId ) return (*itr);
    }
    exit(-1);
}

//The callback that a Connector should invoke for any new or updated data
void BondTradeBookingService::OnMessage(Trade<Bond> &trades)
{
    BookTrade(trades);
    
    for (int i = 0; i < Listeners_List.size(); ++i){
        Listeners_List[i]->ProcessAdd(trades);
    }
    //test
    //cout<<"new trade added: "<<trades.GetProduct().GetProductId()<<endl;
}

//Add a listener to the Service for callbacks on add, remove, and update events for data to the Service.
void BondTradeBookingService::AddListener(ServiceListener<Trade<Bond>> *listeners)
{
    Listeners_List.push_back(listeners);
};


const vector<ServiceListener<Trade<Bond>>*>& BondTradeBookingService::GetListeners() const
{
    return Listeners_List;
}

//book the trade
void BondTradeBookingService::BookTrade(const Trade<Bond> &trade)
{
    Trades_Book.push_back(trade);
}

//flow data from a file into bond trade booking service
void BondTradeBookingConnector::ReadFile(string file){
    ifstream f(file);
    if(f.fail()){
        cout<<"File open failed!"<<endl;
        exit(-1);
    }
    //take the first line out
    string val;
    getline(f, val);
    
    vector<string> record;
    
    while(f){
        string v;
        if(!getline(f, v)) break;
        stringstream ss;
        ss << v;
        while (ss){
            string temp;
            if(!getline(ss, temp, ',')) break;
            record.push_back(temp);
        }
        Bond new_b(record[0]);//initialize a bond product by its CUSIP
        string id = record[1];
        string book = record[2];
        long quantity = stol(record[3]);
        string side = record[4];
        Side _side;
        if(side[0] == 'S'){
            _side = SELL;
        }
        else{
            _side = BUY;
        }
        record.clear();
        Trade<Bond> new_t(new_b, id, book, quantity, _side);
        
        Trade_Service.OnMessage(new_t);
    }
    
    f.close();
    
}

template<typename T>
Trade<T>::Trade(const T &_product, string _tradeId, string _book, long _quantity, Side _side) :
  product(_product)
{
  tradeId = _tradeId;
  book = _book;
  quantity = _quantity;
  side = _side;
}

template<typename T>
const T& Trade<T>::GetProduct() const
{
  return product;
}

template<typename T>
const string& Trade<T>::GetTradeId() const
{
  return tradeId;
}

template<typename T>
const string& Trade<T>::GetBook() const
{
  return book;
}

template<typename T>
long Trade<T>::GetQuantity() const
{
  return quantity;
}

template<typename T>
Side Trade<T>::GetSide() const
{
  return side;
}



#endif
