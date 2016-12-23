/**
 * streamingservice.hpp
 * Defines the data types and Service for price streams.
 *
 * @author Breman Thuraisingham
 */
#ifndef STREAMING_SERVICE_HPP
#define STREAMING_SERVICE_HPP

#include "soa.hpp"
#include "marketdataservice.hpp"

/**
 * A price stream order with price and quantity (visible and hidden)
 */
class PriceStreamOrder
{

public:

  // ctor for an order
  PriceStreamOrder(double _price, long _visibleQuantity, long _hiddenQuantity, PricingSide _side);

  // The side on this order
  PricingSide GetSide() const;

  // Get the price on this order
  double GetPrice() const;

  // Get the visible quantity on this order
  long GetVisibleQuantity() const;

  // Get the hidden quantity on this order
  long GetHiddenQuantity() const;

private:
  double price;
  long visibleQuantity;
  long hiddenQuantity;
  PricingSide side;

};

/**
 * Price Stream with a two-way market.
 * Type T is the product type.
 */
template<typename T>
class PriceStream
{

public:

  // ctor
    PriceStream(){}
    
  PriceStream(const T &_product, const PriceStreamOrder &_bidOrder, const PriceStreamOrder &_offerOrder);

  // Get the product
  const T& GetProduct() const;

  // Get the bid order
  const PriceStreamOrder& GetBidOrder() const;

  // Get the offer order
  const PriceStreamOrder& GetOfferOrder() const;

private:
  T product;
  PriceStreamOrder bidOrder;
  PriceStreamOrder offerOrder;

};

/**
 * Streaming service to publish two-way prices.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class StreamingService : public Service<string,PriceStream <T> >
{

public:

  // Publish two-way prices
  virtual void PublishPrice(PriceStream<T>& priceStream) = 0;

};

class BondStreamingService: public StreamingService<Bond>
{
private:
    vector<PriceStream<Bond>> bond_price_stream;
    vector<ServiceListener<PriceStream<Bond>>*> listener_list;
public:
    PriceStream<Bond>& GetData(string cusip) override;
    void OnMessage(PriceStream<Bond>& price_stream) override;
    void AddListener(ServiceListener<PriceStream<Bond>>* listener) override;
    const vector<ServiceListener<PriceStream<Bond>>*>& GetListeners() const override;
    void PublishPrice(PriceStream<Bond>& price_stream) override;
};

PriceStream<Bond>& BondStreamingService::GetData(string cusip)
{
    if(bond_price_stream.size()==0) exit(-1);
    long i;
    for(i = bond_price_stream.size() -1; i>=0; --i)
    {
        if(bond_price_stream[i].GetProduct().GetProductId() == cusip) return bond_price_stream[i];
    }
    exit(-1);
}

void BondStreamingService::OnMessage(PriceStream<Bond>& price_stream)
{
    PublishPrice(price_stream);
}

void BondStreamingService::AddListener(ServiceListener<PriceStream<Bond> > *listener)
{
    listener_list.push_back(listener);
}

const vector<ServiceListener<PriceStream<Bond>>*>& BondStreamingService::GetListeners() const
{
    return listener_list;
}

void BondStreamingService::PublishPrice(PriceStream<Bond>& price_stream)
{
    bond_price_stream.push_back(price_stream);
    for(int i = 0; i < listener_list.size(); ++i) listener_list[i]->ProcessAdd(price_stream);
    //test
    //cout<< "A bond price stream is added!\n";
}

PriceStreamOrder::PriceStreamOrder(double _price, long _visibleQuantity, long _hiddenQuantity, PricingSide _side)
{
  price = _price;
  visibleQuantity = _visibleQuantity;
  hiddenQuantity = _hiddenQuantity;
  side = _side;
}

double PriceStreamOrder::GetPrice() const
{
  return price;
}

long PriceStreamOrder::GetVisibleQuantity() const
{
  return visibleQuantity;
}

long PriceStreamOrder::GetHiddenQuantity() const
{
  return hiddenQuantity;
}

template<typename T>
PriceStream<T>::PriceStream(const T &_product, const PriceStreamOrder &_bidOrder, const PriceStreamOrder &_offerOrder) :
  product(_product), bidOrder(_bidOrder), offerOrder(_offerOrder)
{
}

template<typename T>
const T& PriceStream<T>::GetProduct() const
{
  return product;
}

template<typename T>
const PriceStreamOrder& PriceStream<T>::GetBidOrder() const
{
  return bidOrder;
}

template<typename T>
const PriceStreamOrder& PriceStream<T>::GetOfferOrder() const
{
  return offerOrder;
}

#endif
