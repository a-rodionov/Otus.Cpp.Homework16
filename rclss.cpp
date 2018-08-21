#include "realty.h"
#include "functions.h"

using namespace dlib;

int main(int argc, char const* argv[])
{
  try
  {
    std::string filename;
    try
    {
      if(2 != argc) {
        throw std::invalid_argument("");
      }

      filename = std::string{argv[1]};
      std::ifstream file(filename, std::ios::in);
      if(!file) {
        std::string error_msg{"Error! Can't open file "};
        error_msg += filename + ".\n";
        throw std::runtime_error(error_msg);
      }
    }
    catch(std::exception& exc)
    {
      std::string error_msg = exc.what();
      error_msg += "The programm must be started with 1 parameter, which means the filename.";
      throw std::invalid_argument(error_msg);
    }

    NormalizedFunction decisionFunction;
    deserialize(filename) >> decisionFunction;

    std::string encodedData;
    while( std::getline(std::cin, encodedData) ) {
      try
      {
        RealtyData data = Realty(encodedData);

        int cluster = decisionFunction(data);

        std::ifstream clusterlFile;
        std::string clusterFilename = filename + '.' + std::to_string(cluster);
        if(!IsFileExist(clusterFilename)) {
          auto clustersCount = decisionFunction.number_of_classes();
          int lowerCluster{cluster-1}, upperCluster{cluster+1};

          for(; lowerCluster >= 0; --lowerCluster) {
            clusterFilename = filename + '.' + std::to_string(lowerCluster);
            if(IsFileExist(clusterFilename)) {
              break;
            }
          }

          for(; upperCluster < clustersCount; ++upperCluster) {
            clusterFilename = filename + '.' + std::to_string(upperCluster);
            if(IsFileExist(clusterFilename)) {
              break;
            }
          }

          if(lowerCluster < 0) {
            if(upperCluster == clustersCount) {
              throw std::runtime_error("Error! Couldn't find any filewith clustered data.");
            }
            clusterFilename = filename + '.' + std::to_string(upperCluster);
          }
          else {
            if(upperCluster == clustersCount) {
              clusterFilename = filename + '.' + std::to_string(lowerCluster);
            }
            else {
              clusterFilename = (cluster - lowerCluster) < (upperCluster - cluster)
                                ? filename + '.' + std::to_string(lowerCluster)
                                : filename + '.' + std::to_string(upperCluster);
            }
          }
        }
        clusterlFile.open(clusterFilename, std::ifstream::in);

        std::list<Realty> clusterized_data;
        while( std::getline(clusterlFile, encodedData) ) {
          clusterized_data.push_back(Realty(encodedData));
        }

        clusterized_data.sort([&data] (const auto& lhs, const auto& rhs)
                              {
                                return PointsDistance(data(0), data(1), lhs.GetLongitude(), lhs.GetLatitude())
                                  < PointsDistance(data(0), data(1), rhs.GetLongitude(), rhs.GetLatitude());
                              });

        std::copy(clusterized_data.cbegin(),
                  clusterized_data.cend(),
                  infix_ostream_iterator<Realty>{std::cout, ""});
      }
      catch(std::exception& exc)
      {
        std::cerr << exc.what() << std::endl;
        continue;
      }
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
