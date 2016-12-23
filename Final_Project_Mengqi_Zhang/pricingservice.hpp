/**
 * pricingservice.hpp
 * Defines the data types and Service for internal prices.
 *
 * @author Breman Thuraisingham
 */
#ifndef PRICING_SERVICE_HPP
#define PRICING_SERVICE_HPP

#include <string>
#include "soa.hpp"
#include <vector>
#include "products.hpp"

/**
 * A price object consisting of mid and bid/offer spread.
 * Type T is the product type.
 */
template<typename T>
class Price
{

public:

  // ctor for a price
  Price(const T &_product, double _mid, double _bidOfferSpread);

  // Get the product
  const T& GetProduct() const;

  // Get the mid price
  double GetMid() const;

  // Get the bid/offer spread around the mid
  double GetBidOfferSpread() const;

private:
  const T& product;
  double mid;
  double bidOfferSpread;

};

/**
 * Pricing Service managing mid prices and bid/offers.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class PricingService : public Service<string,Price <T> >
{
};

class BondPricingService: public PricingService<Bond>
{
private:
    vector<Price<Bond>> bond_price;
    vector<ServiceListener<Price<Bond>>*> listener_list;
public:
    BondPricingService(){cout<<"A BondPricingService is created!\n";}//ctor
    
    Price<Bond>& GetData(string cusip) override;
    
    void OnMessage(Price<Bond>& price) override;
    
    void AddListener(ServiceListener<Price<Bond>>* listener) override;
    
    const vector<ServiceListener<Price<Bond>>*>& GetListeners() const override;
    
};

class BondPricingConnector: public Connector<Price<Bond>>
{
private:
    BondPricingService &pricing_service;
public:
    BondPricingConnector(BondPricingService& _input);
    void ReadFile(string file);
    void Publish(Price<Bond>& data){};
};

//Definition of BondPricingService class
Price<Bond>& BondPricingService::GetData(string cusip){
    for(int i = 0; i< bond_price.size(); ++i){
        if(bond_price[i].GetProduct().GetProductId() == cusip) return bond_price[i];
    }
    exit(-1);
}

void BondPricingService::OnMessage(Price<Bond>& price){
    bond_price.push_back(price);
    
    for(int i = 0; i<listener_list.size(); ++i)
    {
        listener_list[i]->ProcessAdd(price);
    }
    //test
    //cout<<"New price information is added!\n";
}

void BondPricingService::AddListener(ServiceListener<Price<Bond>>* listener){
    listener_list.push_back(listener);
}

const vector<ServiceListener<Price<Bond>>*>& BondPricingService::GetListeners() const{
    return listener_list;
}

//Definition of BondPricingConnector class
BondPricingConnector::BondPricingConnector(BondPricingService& input):pricing_service(input)
{
    //cout<<"A BondPricingConnector is created!\n";
}

void BondPricingConnector::ReadFile(string file){
    ifstream f(file);
    if(f.fail()){
        cout<<"File open failed"<<endl;
        return;
    }
    string val;
    getline(f, val);
    
    vector<string> price;
    
    while(f)
    {
        string value;
        if(!getline(f, value)) break;
        stringstream ss;
        ss<<value;
        while(ss){
            string temp;
            if(!getline(ss, temp, ',')) break;
            price.push_back(temp);
        }
        Bond new_bond(price[0]);
        double mid_price = DecimalBondPrice(price[1]);
        double spread = DecimalBondPrice(price[2]);
        Price<Bond> new_price(new_bond, mid_price, spread);
        pricing_service.OnMessage(new_price);
        
        price.clear();
    }
    f.close();
}


template<typename T>
Price<T>::Price(const T &_product, double _mid, double _bidOfferSpread) :
  product(_product)
{
  mid = _mid;
  bidOfferSpread = _bidOfferSpread;
}

template<typename T>
const T& Price<T>::GetProduct() const
{
  return product;
}

template<typename T>
double Price<T>::GetMid() const
{
  return mid;
}

template<typename T>
double Price<T>::GetBidOfferSpread() const
{
  return bidOfferSpread;
}

#endif
