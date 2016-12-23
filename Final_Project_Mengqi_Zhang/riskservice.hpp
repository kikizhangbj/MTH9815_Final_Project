/**
 * riskservice.hpp
 * Defines the data types and Service for fixed income risk.
 *
 * @author Breman Thuraisingham
 */
#ifndef RISK_SERVICE_HPP
#define RISK_SERVICE_HPP


#include <vector>
#include <map>
#include "soa.hpp"
#include "positionservice.hpp"


/**
 * PV01 risk.
 * Type T is the product type.
 */
template<typename T>
class PV01
{

public:

  // ctor for a PV01 value
    PV01();
    PV01(const T &_product, double _pv01, long _quantity);

  // Get the product on this PV01 value
    const T& GetProduct() const;

  // Get the PV01 value
    double GetPV01() const;

  // Get the quantity that this risk value is associated with
    long GetQuantity() const;
    
    void AddQuantity(long q);
    
private:
    T product;
    double pv01;
    long quantity;

};

//A function returns the corresponding PV01 value given CUSIP
double BondPV01(string cusip){
    if (cusip == "912828M72") return 0.01974732;
    else if (cusip == "912828N22") return 0.029349458;
    else if (cusip == "912828M98") return 0.047720509;
    else if (cusip == "912828M80") return 0.06495714;
    else if (cusip == "912828M56") return 0.089254107;
    else if (cusip == "912810RP5") return 0.198018642;
    return 0;
}

/**
 * A bucket sector to bucket a group of securities.
 * We can then aggregate bucketed risk to this bucket.
 * Type T is the product type.
 */
template<typename T>
class BucketedSector
{

public:

  // ctor for a bucket sector
  BucketedSector(const vector<T> &_products, string _name);

  // Get the products associated with this bucket
  const vector<T>& GetProducts() const;

  // Get the name of the bucket
  const string& GetName() const;

private:
  vector<T> products;
  string name;

};

/**
 * Risk Service to vend out risk for a particular security and across a risk bucketed sector.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class RiskService : public Service<string,PV01 <T> >
{

public:

  // Add a position that the service will risk
  virtual void AddPosition(Position<T> &position) = 0;

  // Get the bucketed risk for the bucket sector
  virtual double GetBucketedRisk(const BucketedSector<T> &sector) const = 0;

};

class BondRiskService: public RiskService<Bond>
{
private:
    vector<ServiceListener<PV01<Bond>>*> listener_list;
    vector<ServiceListener<vector<PV01<Bond>>>*> historical_data_listener_list;
    vector<PV01<Bond>> risk_position;
public:
    BondRiskService(){}
    void AddPosition(Position<Bond>& position) override;
    double GetBucketedRisk(const BucketedSector<Bond>& sector) const override;
    
    PV01<Bond>& GetData(string cusip) override;
    void OnMessage(PV01<Bond>& data) override;
    void AddListener(ServiceListener<PV01<Bond>>* listener) override;
    void AddHistoricalDataListener(ServiceListener<vector<PV01<Bond>>>* listener);
    const vector<ServiceListener<PV01<Bond>>*>& GetListeners() const override;
};


template<typename T>
PV01<T>::PV01(){}

template<typename T>
PV01<T>::PV01(const T &_product, double _pv01, long _quantity) :
  product(_product)
{
  pv01 = _pv01;
  quantity = _quantity;
}

template<typename T>
double PV01<T>::GetPV01() const
{
    return pv01;
}

template<typename T>
long PV01<T>::GetQuantity() const
{
    return quantity;
}

template<typename T>
void PV01<T>::AddQuantity(long q){
    quantity += q;
}

template<typename T>
const T& PV01<T>::GetProduct() const
{
    return product;
}

template<typename T>
const vector<T>& BucketedSector<T>::GetProducts() const
{
    return products;
}

template<typename T>
const string& BucketedSector<T>::GetName() const
{
    return name;
}

template<typename T>
BucketedSector<T>::BucketedSector(const vector<T>& _products, string _name) :
  products(_products)
{
  name = _name;
}

void BondRiskService::AddPosition(Position<Bond>& position)
{
    string cusip = position.GetProduct().GetProductId();
    auto iter = risk_position.begin();
    for(; iter!= risk_position.end(); ++iter)
    {
        if(cusip == iter->GetProduct().GetProductId())
        {
            long new_quantity = position.GetAggregatePosition() + iter->GetQuantity();
            PV01<Bond> new_pv01(iter->GetProduct(), iter->GetPV01(), new_quantity);
            *iter = new_pv01;
            //test
            //cout << "An existing risk position is updated!\n";
            break;
        }
    }
    if(iter == risk_position.end()){
        PV01<Bond> new_pv01(position.GetProduct(), BondPV01(position.GetProduct().GetProductId()), position.GetAggregatePosition());
        risk_position.push_back(new_pv01);
        //test
        //cout << "A new risk position is created!\n";
    }
    for(int i = 0; i < historical_data_listener_list.size(); ++i)
        historical_data_listener_list[i]->ProcessAdd(risk_position);
}

double BondRiskService::GetBucketedRisk(const BucketedSector<Bond>& sector) const
{
    vector<Bond> bondlist = sector.GetProducts();
    double result = 0;
    for(auto iter_bl = bondlist.begin(); iter_bl!=bondlist.end(); ++iter_bl)
    {
        string product_id = iter_bl->GetProductId();
        for(auto iter_cp = risk_position.begin(); iter_cp != risk_position.end(); ++iter_cp)
        {
            if(product_id == iter_cp->GetProduct().GetProductId())
            {
                result += (BondPV01(product_id) * iter_cp->GetQuantity());
                break;
            }
        }
    }
    return result;
}

PV01<Bond>& BondRiskService::GetData(string cusip)
{
    if(!risk_position.size())
    {
        cout<<"Empty"<<endl;
    }
    for(auto iter = risk_position.begin(); iter!= risk_position.end(); ++iter)
    {
        if(cusip == iter->GetProduct().GetProductId()){
            return *iter;
        }
    }
    cout<<"No match!\n";
    exit(-1);
}

void BondRiskService::AddListener(ServiceListener<PV01<Bond>>* listener){
    listener_list.push_back(listener);
}

void BondRiskService::AddHistoricalDataListener(ServiceListener<vector<PV01<Bond> > > *listener){
    historical_data_listener_list.push_back(listener);
}

const vector<ServiceListener<PV01<Bond>>*>& BondRiskService::GetListeners() const
{
    return listener_list;
}

void BondRiskService::OnMessage(PV01<Bond> &data){}

#endif
