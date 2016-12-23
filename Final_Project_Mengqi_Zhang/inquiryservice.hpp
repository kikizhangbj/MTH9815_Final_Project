/**
 * inquiryservice.hpp
 * Defines the data types and Service for customer inquiries.
 *
 * @author Breman Thuraisingham
 */
#ifndef INQUIRY_SERVICE_HPP
#define INQUIRY_SERVICE_HPP

#include "soa.hpp"
#include "tradebookingservice.hpp"

// Various inqyury states
enum InquiryState { RECEIVED, QUOTED, DONE, REJECTED, CUSTOMER_REJECTED };

/**
 * Inquiry object modeling a customer inquiry from a client.
 * Type T is the product type.
 */
template<typename T>
class Inquiry
{

public:

  // ctor for an inquiry
  Inquiry(string _inquiryId, const T &_product, Side _side, long _quantity, double _price, InquiryState _state);

  // Get the inquiry ID
  const string& GetInquiryId() const;

  // Get the product
  const T& GetProduct() const;

  // Get the side on the inquiry
  Side GetSide() const;

  // Get the quantity that the client is inquiring for
  long GetQuantity() const;

  // Get the price that we have responded back with
  double GetPrice() const;

  // Get the current state on the inquiry
  InquiryState GetState() const;
    
  // Change state
  void ChangeState(InquiryState _state){state = _state;}
    
  // ChangePrice
  void ChangePrice(double _price){price = _price;}
    
private:
  string inquiryId;
  T product;
  Side side;
  long quantity;
  double price;
  InquiryState state;

};

/**
 * Service for customer inquirry objects.
 * Keyed on inquiry identifier (NOTE: this is NOT a product identifier since each inquiry must be unique).
 * Type T is the product type.
 */
template<typename T>
class InquiryService : public Service<string,Inquiry <T> >
{

public:

  // Send a quote back to the client
  virtual void SendQuote(const string &inquiryId, double price) = 0;

  // Reject an inquiry from the client
  virtual void RejectInquiry(const string &inquiryId) = 0;

};

class BondInquiryService: public InquiryService<Bond>
{
    vector<Inquiry<Bond>> bond_inquiry;
    vector<ServiceListener<Inquiry<Bond>>*> listener_list;
    
public:
    Inquiry<Bond>& GetData(string inquiryId) override;
    void SendQuote(const string & inquiryId, double price) override {};
    void RejectInquiry(const string& inquiryId) override {}
    void OnMessage(Inquiry<Bond>& inquiry) override;
    void AddListener(ServiceListener<Inquiry<Bond>>* listener);
    const vector<ServiceListener<Inquiry<Bond>>*>& GetListeners() const override;
};

class BondInquiryConnector: public Connector<Inquiry<Bond>>
{
    BondInquiryService& inquiry_service;
public:
    BondInquiryConnector(BondInquiryService& input): inquiry_service(input){}
    void ReadFile(string file);
    void Publish(Inquiry<Bond>& data);
};

Inquiry<Bond>& BondInquiryService::GetData(string inquiryId)
{
    if(bond_inquiry.size()==0) exit(-1);
    
    long i;
    for(i = 0; i<bond_inquiry.size(); ++i)
    {
        if(bond_inquiry[i].GetInquiryId() == inquiryId)
        {
            return bond_inquiry[i];
        }
    }
    exit(-1);
}

void BondInquiryService::OnMessage(Inquiry<Bond> &inquiry)
{
    if(inquiry.GetState() == RECEIVED)
    {
        Inquiry<Bond> new_inq(inquiry.GetInquiryId(), inquiry.GetProduct(), inquiry.GetSide(), inquiry.GetQuantity(), 100, inquiry.GetState());
        bond_inquiry.push_back(new_inq);
        //test
        //cout<<"New inquiry is added!\n";
    }
    else if(inquiry.GetState() == QUOTED)
    {
        for(long i = 0; i < bond_inquiry.size(); ++i)
        {
            if(bond_inquiry[i].GetInquiryId() == inquiry.GetInquiryId())
            {
                Inquiry<Bond> new_inq(inquiry.GetInquiryId(), inquiry.GetProduct(), inquiry.GetSide(), inquiry.GetQuantity(), inquiry.GetPrice(), DONE);
                bond_inquiry[i] = new_inq;
                //test
                //cout<<"Bond Inquiry is updated!\n";
                
                for(int i = 0; i < listener_list.size(); ++i) listener_list[i]->ProcessAdd(new_inq);
                break;
            }
        }
    }
}

void BondInquiryService::AddListener (ServiceListener<Inquiry<Bond>> * listener)
{
    listener_list.push_back(listener);
}

const vector<ServiceListener<Inquiry<Bond>>*>& BondInquiryService::GetListeners() const
{
    return listener_list;
}

void BondInquiryConnector::Publish(Inquiry<Bond>& inquiry)
{
    Inquiry<Bond> new_inq(inquiry.GetInquiryId(), inquiry.GetProduct(), inquiry.GetSide(), inquiry.GetQuantity(), inquiry.GetPrice(), QUOTED);
    inquiry_service.OnMessage(new_inq);
}

void BondInquiryConnector::ReadFile(string file)
{
    ifstream f(file);
    if(f.fail()){
        cout<<"File open failed!"<<endl;
        exit(-1);
    }
    //take the first line out
    string val;
    getline(f, val);
    
    vector<string> record;
    
    while(f)
    {
        string v;
        if(!getline(f, v)) break;
        stringstream ss;
        ss << v;
        while (ss){
            string temp;
            if(!getline(ss, temp, ',')) break;
            record.push_back(temp);
        }
        string inqId = record[0];
        Bond bond(record[1]);
        Side side;
        if(record[2][0] == 'B') side = BUY;
        else side = SELL;
        long quantity = stol(record[3]);
        double price = stol(record[4]);
        
        Inquiry<Bond> new_inq(inqId, bond, side, quantity, price, RECEIVED);
        
        inquiry_service.OnMessage(new_inq);
        
        if(inquiry_service.GetData(inqId).GetState() == RECEIVED)
        {
            this->Publish(inquiry_service.GetData(inqId));
        }
        else exit(-1);
        
        record.clear();
    }
    
    f.close();

}

template<typename T>
Inquiry<T>::Inquiry(string _inquiryId, const T &_product, Side _side, long _quantity, double _price, InquiryState _state) :
  product(_product)
{
  inquiryId = _inquiryId;
  side = _side;
  quantity = _quantity;
  price = _price;
  state = _state;
}

template<typename T>
const string& Inquiry<T>::GetInquiryId() const
{
  return inquiryId;
}

template<typename T>
const T& Inquiry<T>::GetProduct() const
{
  return product;
}

template<typename T>
Side Inquiry<T>::GetSide() const
{
  return side;
}

template<typename T>
long Inquiry<T>::GetQuantity() const
{
  return quantity;
}

template<typename T>
double Inquiry<T>::GetPrice() const
{
  return price;
}

template<typename T>
InquiryState Inquiry<T>::GetState() const
{
  return state;
}

#endif
