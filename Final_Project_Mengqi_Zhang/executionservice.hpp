/**
 * executionservice.hpp
 * Defines the data types and Service for executions.
 *
 * @author Breman Thuraisingham
 */
#ifndef EXECUTION_SERVICE_HPP
#define EXECUTION_SERVICE_HPP

#include <string>
#include "soa.hpp"
#include "marketdataservice.hpp"

enum OrderType { FOK, IOC, MARKET, LIMIT, STOP };

enum Market { BROKERTEC, ESPEED, CME };

/**
 * An execution order that can be placed on an exchange.
 * Type T is the product type.
 */
template<typename T>
class ExecutionOrder
{

public:

  // ctor for an order
  ExecutionOrder(const T &_product, PricingSide _side, string _orderId, OrderType _orderType, double _price, long _visibleQuantity, long _hiddenQuantity, string _parentOrderId, bool _isChildOrder);

  // Get the product
  const T& GetProduct() const;

  // Get the order ID
  const string& GetOrderId() const;

  // Get the order type on this order
  OrderType GetOrderType() const;

  // Get the price on this order
  double GetPrice() const;

  // Get the visible quantity on this order
  long GetVisibleQuantity() const;

  // Get the hidden quantity
  long GetHiddenQuantity() const;

  // Get the parent order ID
  const string& GetParentOrderId() const;

  // Is child order?
  bool IsChildOrder() const;

  PricingSide GetSide() const;
    
private:
  T product;
  PricingSide side;
  string orderId;
  OrderType orderType;
  double price;
  long visibleQuantity;
  long hiddenQuantity;
  string parentOrderId;
  bool isChildOrder;

};

/**
 * Service for executing orders on an exchange.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class ExecutionService : public Service<string,ExecutionOrder <T> >
{

public:

  // Execute an order on a market
  virtual void ExecuteOrder(const ExecutionOrder<T>& order) = 0;

};

class BondExecutionService : public ExecutionService<Bond>
{
private:
    vector<ExecutionOrder<Bond>> bond_order;
    vector<ServiceListener<ExecutionOrder<Bond>>*> listener_list;
public:
    ExecutionOrder<Bond>& GetData(string orderId) override;
    void OnMessage(ExecutionOrder<Bond>& order) override;
    void AddListener(ServiceListener<ExecutionOrder<Bond>>* listener) override;
    const vector<ServiceListener<ExecutionOrder<Bond>>*>& GetListeners() const override;
    void ExecuteOrder(const ExecutionOrder<Bond>& order) override;
};

ExecutionOrder<Bond>& BondExecutionService::GetData(string orderId)
{
    for(int i = 0; i < bond_order.size(); ++i)
    {
        if(bond_order[i].GetOrderId() == orderId)
        {
            return bond_order[i];
        }
    }
    exit(-1);
}

void BondExecutionService::AddListener(ServiceListener<ExecutionOrder<Bond>>* listener)
{
    listener_list.push_back(listener);
}

const vector<ServiceListener<ExecutionOrder<Bond>>*>& BondExecutionService::GetListeners() const
{
    return listener_list;
}

void BondExecutionService::OnMessage(ExecutionOrder<Bond>& order)
{
    ExecuteOrder(order);

    for(int i = 0; i<listener_list.size(); ++i)
    {
        listener_list[i]->ProcessAdd(order);
    }
    
    //test
    cout<<"An execution order is added!\n";
}

void BondExecutionService::ExecuteOrder(const ExecutionOrder<Bond> &order)
{
    bond_order.push_back(order);
}

template<typename T>
ExecutionOrder<T>::ExecutionOrder(const T &_product, PricingSide _side, string _orderId, OrderType _orderType, double _price, long _visibleQuantity, long _hiddenQuantity, string _parentOrderId, bool _isChildOrder) :
  product(_product)
{
  side = _side;
  orderId = _orderId;
  orderType = _orderType;
  price = _price;
  visibleQuantity = _visibleQuantity;
  hiddenQuantity = _hiddenQuantity;
  parentOrderId = _parentOrderId;
  isChildOrder = _isChildOrder;
}

template<typename T>
const T& ExecutionOrder<T>::GetProduct() const
{
  return product;
}

template<typename T>
const string& ExecutionOrder<T>::GetOrderId() const
{
  return orderId;
}

template<typename T>
OrderType ExecutionOrder<T>::GetOrderType() const
{
  return orderType;
}

template<typename T>
double ExecutionOrder<T>::GetPrice() const
{
  return price;
}

template<typename T>
long ExecutionOrder<T>::GetVisibleQuantity() const
{
  return visibleQuantity;
}

template<typename T>
long ExecutionOrder<T>::GetHiddenQuantity() const
{
  return hiddenQuantity;
}

template<typename T>
const string& ExecutionOrder<T>::GetParentOrderId() const
{
  return parentOrderId;
}

template<typename T>
bool ExecutionOrder<T>::IsChildOrder() const
{
  return isChildOrder;
}

template<typename T>
PricingSide ExecutionOrder<T>::GetSide() const
{
    return side;
}
#endif
