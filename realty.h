#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iomanip>
#include <boost/algorithm/string.hpp>

#include "types.h"
#include "infix_iterator.h"

const size_t realtyFeaturesCount = 7;

const size_t idxLongitude = 0;
const size_t idxLatitude = 1;
const size_t idxRooms = 2;
const size_t idxPrice = 3;
const size_t idxTotalArea = 4;
const size_t idxKitchensArea = 5;
const size_t idxFloor = 6;
const size_t idxTotalFloors = 7;

class Realty
{
public:

  Realty(const std::string& encodedData)
  {
    std::vector<std::string> featuresStr;

    boost::algorithm::split(featuresStr, encodedData, boost::is_any_of(";"));

    if((realtyFeaturesCount > featuresStr.size())
      || ((realtyFeaturesCount+1) < featuresStr.size()))
    {
      throw std::invalid_argument("Error! Input line contains wrong number of separated values.");
    }

    std::vector<double> featuresTmp;
    try
    {
      for(const auto& featureStr : featuresStr)
      {
        featuresTmp.push_back(std::stod(featureStr));
      }
    }
    catch(...)
    {
      throw std::invalid_argument("Error! Failed to parse one of input double values.");
    }

    features.reserve(realtyFeaturesCount);
    if((realtyFeaturesCount+1) == featuresTmp.size()) 
    {
      if((0 == featuresTmp[idxFloor]) || (0 == featuresTmp[idxTotalFloors]))
      {
        throw std::invalid_argument("Error! Current floor or top floor equal zero.");
      }
      std::copy(featuresTmp.cbegin(),
                featuresTmp.cbegin() + 6,
                std::back_inserter(features));
      if((1 == featuresTmp[idxFloor])
        || (featuresTmp[idxFloor] == featuresTmp[idxTotalFloors]))
      {
        features.push_back(0);
      }
      else
      {
        features.push_back(1);
      }
    }
    else
    {
      features = std::move(featuresTmp);
    }
  }

  Realty(const RealtyData& data)
    : features(realtyFeaturesCount)
  {
    for(size_t i = 0; i < realtyFeaturesCount; ++i) {
      features[i] = data(i);
    }
  }

  operator RealtyData() const
  {
    RealtyData data;
    for(size_t i{0}; i < features.size(); ++i)
    {
      data(i) = features[i];
    }
    return data; 
  }

  double GetLongitude() const
  {
    return features[idxLongitude];
  }

  double GetLatitude() const
  {
    return features[idxLatitude];
  }

  friend std::ostream& operator<<(std::ostream&, const Realty&);

private:
  std::vector<double> features;
};

std::ostream& operator<<(std::ostream& out, const Realty& realty)
{
  out << std::setprecision(std::numeric_limits<long double>::digits10 + 1);
  std::copy(realty.features.cbegin(),
            realty.features.cend(),
            infix_ostream_iterator<double>{out, ";"});
  out << std::endl;
}
