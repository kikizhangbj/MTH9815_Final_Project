/**
 * positionservice.hpp
 * Defines the data types and Service for positions.
 *
 * @author Breman Thuraisingham
 */
#ifndef POSITION_SERVICE_HPP
#define POSITION_SERVICE_HPP

#include <string>
#include <map>
#include <vector>
#include "soa.hpp"
#include "tradebookingservice.hpp"

using namespace std;

/**
 * Position class in a particular book.
 * Type T is the product type.
 */
template<typename T>
class Position
{

public:

  // ctor for a position
  Position();
  Position(const T &_product);
  Position(const Trade<T> trade);

  // Get the product
  const T& GetProduct() const;

  // Get the position quantity
  long GetPosition(string &book) const;

  // Get the aggregate position
  long GetAggregatePosition() const;

  void AddTrade(Trade<Bond> & trade);

private:
  T product;
  map<string,long> positions;//map from book to its position

};

/**
 * Position Service to manage positions across multiple books and secruties.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class PositionService : public Service<string,Position <T> >
{

public:

  // Add a trade to the service
  virtual void AddTrade(Trade<T> &trade) = 0;

};

class BondPositionService: public PositionService<Bond>
{
private:
    map<string, Position<Bond>> Current_Position;//map from cusip to its position
    vector<ServiceListener<Position<Bond>>*> Listener_List;
public:
    BondPositionService(){cout<<"A BondPositionService is created!\n";}
    Position<Bond>& GetData(string cusip) override;
    void OnMessage(Position<Bond>& position) override;
    void AddListener(ServiceListener<Position<Bond>>* listener) override;
    const vector<ServiceListener<Position<Bond>>*>& GetListeners() const override;
    void AddTrade(Trade<Bond>& trade) override;
};

//Definition of the Position class
template<typename T>
Position<T>::Position(){}

template<typename T>
Position<T>::Position(const T &_product) : product(_product){}

template<typename T>
Position<T>::Position(const Trade<T> trade) : product(trade.GetProduct()){
    if(trade.GetSide() == BUY) positions[trade.GetBook()] = trade.GetQuantity();
    else positions[trade.GetBook()] = -trade.GetQuantity();
}

template<typename T>
const T& Position<T>::GetProduct() const
{
  return product;
}

template<typename T>
long Position<T>::GetPosition(string &book) const
{
    for(auto iter = positions.begin(); iter != positions.end(); ++iter){
        if(iter->first == book) return iter->second;
    }
  return 0;
}

template<typename T>
long Position<T>::GetAggregatePosition() const
{
    long result;
    for(auto iter = positions.begin(); iter != positions.end(); ++iter)
    {
        result += iter->second;
    }
  return result;
}

template<typename T>
    void Position<T>::AddTrade(Trade<Bond> & trade)
    {
        auto iter = positions.begin();
        for(; iter != positions.end(); ++iter){
            if(iter->first == trade.GetBook()){
                if(trade.GetSide() == BUY) positions[trade.GetBook()]+=trade.GetQuantity();
                else positions[trade.GetBook()] -= trade.GetQuantity();
                break;
            }
        }
        
        if(iter == positions.end())
        {
            if(trade.GetSide()==BUY) positions[trade.GetBook()] = trade.GetQuantity();
            else positions[trade.GetBook()] = -trade.GetQuantity();
        }
    }
    
//Definition of BondPositionService class
Position<Bond>& BondPositionService::GetData(string CUSIP)
{
    return Current_Position[CUSIP];
}
    
void BondPositionService::OnMessage(Position<Bond>& position)
{
    for (int i = 0; i < Listener_List.size(); ++i)
        Listener_List[i]->ProcessAdd(position);
    
}
    
void BondPositionService::AddListener(ServiceListener<Position<Bond>> * listener)
{
    Listener_List.push_back(listener);
}
    
const vector<ServiceListener<Position<Bond>>*>& BondPositionService::GetListeners() const
{
    return Listener_List;
}

void BondPositionService::AddTrade(Trade<Bond>& trade)
{
    auto iter = Current_Position.begin();
    for(; iter != Current_Position.end(); ++iter){
        if(trade.GetProduct().GetProductId()==iter->first){
            iter->second.AddTrade(trade);
            Position<Bond> temp(trade);
            this->OnMessage(temp);
            //test
            //cout<<"A position of exiting product is added! "<<trade.GetProduct().GetProductId()<<endl;
            break;
        }
    }
    if(iter == Current_Position.end()){
        Position<Bond> temp(trade);
        Current_Position[trade.GetProduct().GetProductId()] = temp;
        this->OnMessage(temp);
        //test
        //cout<<"A position of a new product is added! "<<trade.GetProduct().GetProductId()<<endl;
    }
    
}
#endif
