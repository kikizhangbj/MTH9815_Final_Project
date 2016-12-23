/**
 * marketdataservice.hpp
 * Defines the data types and Service for order book market data.
 *
 * @author Breman Thuraisingham
 */
#ifndef MARKET_DATA_SERVICE_HPP
#define MARKET_DATA_SERVICE_HPP

#include <string>
#include <vector>
#include <map>
#include "products.hpp"
#include "soa.hpp"

using namespace std;

// Side for market data
enum PricingSide { BID, OFFER };

/**
 * A market data order with price, quantity, and side.
 */
class Order
{

public:

  // ctor for an order
  Order(double _price, long _quantity, PricingSide _side);

  // Get the price on the order
  double GetPrice() const;

  // Get the quantity on the order
  long GetQuantity() const;

  // Get the side on the order
  PricingSide GetSide() const;

private:
  double price;
  long quantity;
  PricingSide side;

};

/**
 * Class representing a bid and offer order
 */
class BidOffer
{

public:

  // ctor for bid/offer
  BidOffer(const Order &_bidOrder, const Order &_offerOrder);

  // Get the bid order
  const Order& GetBidOrder() const;

  // Get the offer order
  const Order& GetOfferOrder() const;

private:
  Order bidOrder;
  Order offerOrder;

};

/**
 * Order book with a bid and offer stack.
 * Type T is the product type.
 */
template<typename T>
class OrderBook
{

public:

  // ctor for the order book
  OrderBook(const T &_product, const vector<Order> &_bidStack, const vector<Order> &_offerStack);

  // Get the product
  const T& GetProduct() const;

  // Get the bid stack
  const vector<Order>& GetBidStack() const;

  // Get the offer stack
  const vector<Order>& GetOfferStack() const;

private:
  T product;
  vector<Order> bidStack;
  vector<Order> offerStack;

};

/**
 * Market Data Service which distributes market data
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class MarketDataService : public Service<string,OrderBook <T> >
{

public:

  // Get the best bid/offer order
  virtual const BidOffer& GetBestBidOffer(const string &productId) = 0;

  // Aggregate the order book
    const OrderBook<T>& AggregateDepth(const string &productId){};

};

class BondMarketDataService: public MarketDataService<Bond>
{
private:
    vector<OrderBook<Bond>> bond_orderbook;
    vector<ServiceListener<OrderBook<Bond>>*> listener_list;
public:
    OrderBook<Bond>& GetData(string cusip) override;
    
    void OnMessage(OrderBook<Bond>& order_book) override;
    
    void AddListener(ServiceListener<OrderBook<Bond>>* listener) override;
    
    const vector<ServiceListener<OrderBook<Bond>>*>& GetListeners() const override;
    
    const BidOffer& GetBestBidOffer(const string& cusip) override;
};

class BondMarketDataConnector: public Connector<OrderBook<Bond>>
{
private:
    BondMarketDataService &market_data_service;
public:
    BondMarketDataConnector(BondMarketDataService& input): market_data_service(input){}
    void ReadFile(string file);
    void Publish(OrderBook<Bond>& data) override {}
};

void BondMarketDataConnector::ReadFile(string file)
{
    ifstream f(file);
    if(f.fail()) exit(-1);
    
    string value;
    getline(f, value);
    
    vector<string> orderbook;
    while(f)
    {
        string value;
        if(!getline(f, value)) break;
        stringstream ss;
        ss << value;
        while(ss)
        {
            string temp;
            if(!getline(ss, temp, ',')) break;
            orderbook.push_back(temp);
        }
        Bond b(orderbook[0]);
        double mid_price = DecimalBondPrice(orderbook[1]);
        double spread = double(1/256);
        vector<Order> bid_order, offer_order;
        
        for(int i = 1; i < 6; ++i)
        {
            Order o_order(mid_price+spread*i, 10000000*i, OFFER);
            Order b_order(mid_price-spread*i, 10000000*i, BID);
            offer_order.push_back(o_order);
            bid_order.push_back(b_order);
        }
        
        OrderBook<Bond> new_orderbook(b, bid_order, offer_order);
        market_data_service.OnMessage(new_orderbook);
        
        orderbook.clear();
    }
    f.close();
}


Order::Order(double _price, long _quantity, PricingSide _side)
{
  price = _price;
  quantity = _quantity;
  side = _side;
}

double Order::GetPrice() const
{
  return price;
}
 
long Order::GetQuantity() const
{
  return quantity;
}
 
PricingSide Order::GetSide() const
{
  return side;
}

BidOffer::BidOffer(const Order &_bidOrder, const Order &_offerOrder) :
  bidOrder(_bidOrder), offerOrder(_offerOrder)
{
}

const Order& BidOffer::GetBidOrder() const
{
  return bidOrder;
}

const Order& BidOffer::GetOfferOrder() const
{
  return offerOrder;
}

template<typename T>
OrderBook<T>::OrderBook(const T &_product, const vector<Order> &_bidStack, const vector<Order> &_offerStack) :
  product(_product), bidStack(_bidStack), offerStack(_offerStack)
{
}

template<typename T>
const T& OrderBook<T>::GetProduct() const
{
  return product;
}

template<typename T>
const vector<Order>& OrderBook<T>::GetBidStack() const
{
  return bidStack;
}

template<typename T>
const vector<Order>& OrderBook<T>::GetOfferStack() const
{
  return offerStack;
}

//definition BondMarketDataService class
OrderBook<Bond>& BondMarketDataService::GetData (string cusip)
{
    for(auto iter = bond_orderbook.begin(); iter != bond_orderbook.end(); ++iter)
    {
        if(iter->GetProduct().GetProductId() == cusip)
        {
            return *iter;
        }
    }
    exit(-1);
}

void BondMarketDataService::OnMessage(OrderBook<Bond> &order_book)
{
    bond_orderbook.push_back(order_book);
    for(int i = 0; i < listener_list.size(); ++i){
        listener_list[i]->ProcessAdd(order_book);
    }
    //test
    //cout<<"An orderbook is added!\n";
    
}

void BondMarketDataService::AddListener(ServiceListener<OrderBook<Bond>>* listener)
{
    listener_list.push_back(listener);
}

const vector<ServiceListener<OrderBook<Bond>>*>& BondMarketDataService::GetListeners() const
{
    return listener_list;
}

const BidOffer& BondMarketDataService::GetBestBidOffer(const string &cusip)
{
    double best_bid = 0;
    double best_offer = 0;
    bool found = false;
    for(int i = 0; i < bond_orderbook.size(); ++i)
    {
        if(bond_orderbook[i].GetProduct().GetProductId() == cusip)
        {
            found = true;
            double current_bid = bond_orderbook[i].GetBidStack()[0].GetPrice();
            double current_offer = bond_orderbook[i].GetOfferStack()[0].GetPrice();
            best_bid = current_bid > best_bid ? current_bid : best_bid;
            best_offer = current_offer < best_offer ? current_offer : best_offer;
        }
    }
    
    if(found){
        Order bid_order(best_bid, 10000000, BID);
        Order offer_order(best_offer, 10000000, OFFER);
        BidOffer best_bidoffer(bid_order, offer_order);
        return best_bidoffer;
    }
    cout<<"No match for "<<cusip<<endl;
    exit(-1);
}





#endif
