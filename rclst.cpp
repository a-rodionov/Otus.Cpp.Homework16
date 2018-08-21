#include "realty.h"
#include "functions.h"

using namespace dlib;

static const double GAMMA_DEFAULT = 0.000625;

int main(int argc, char const* argv[])
{
  try
  {
    unsigned long long cluster_num;
    std::string filename;
    try
    {
      if(3 != argc) {
        throw std::invalid_argument("");
      }

      std::string digit_str{argv[1]};
      if(!std::all_of(std::cbegin(digit_str),
                      std::cend(digit_str),
                      [](unsigned char symbol) { return std::isdigit(symbol); } )) {
        throw std::invalid_argument("");
      }
      cluster_num = std::stoull(digit_str);
      if(0 == cluster_num) {
        throw std::invalid_argument("");
      }

      filename = std::string{argv[2]};
    }
    catch(...)
    {
      std::string error_msg = "The programm must be started with 2 parameters:\n"
                              "1. Number of clusters. The value must be in range 1 - "
                              + std::to_string(std::numeric_limits<unsigned long long>::max())
                              +"\n2. Output filename.";
      throw std::invalid_argument(error_msg);
    }

    std::vector<RealtyData> realty_data;
    std::vector<double> labels;

    std::string encodedData;
    while( std::getline(std::cin, encodedData) )
    {
      try
      {
        realty_data.push_back(static_cast<RealtyData>(Realty(encodedData)));
      }
      catch(std::exception& exc)
      {
//        std::cerr << exc.what() << std::endl;
        continue;
      }
    }

    // Предварительно удалим одноименные файлы
    std::string cmd{"rm -f "};
    cmd += filename + '*';
    std::system(cmd.c_str());

    std::vector<RealtyData> normalized_realty_data;
    normalized_realty_data.reserve(realty_data.size());

    vector_normalizer<RealtyData> normalizer;
    normalizer.train(realty_data);
    std::transform(realty_data.cbegin(),
                   realty_data.cend(),
                   std::back_inserter(normalized_realty_data),
                   [&normalizer] (const auto& data)
                   {
                    return normalizer(data);
                   });

    Clusterize(kernel_type(0.01), 0.01, 8, cluster_num, normalized_realty_data, labels);

    std::vector<std::unique_ptr<std::ofstream>> clusterFiles(cluster_num);
    for(size_t i{0}; i < realty_data.size(); ++i) {
      if(!clusterFiles[labels[i]]) {
        std::string clusterFilename = filename + '.' + std::to_string(static_cast<int>(labels[i]));
        clusterFiles[labels[i]] = std::make_unique<std::ofstream>(clusterFilename, std::ofstream::out | std::ofstream::trunc);
      }
      *clusterFiles[labels[i]] << Realty(realty_data[i]);
    }

    realty_data.clear();
    clusterFiles.clear();

    randomize_samples(normalized_realty_data, labels);

    ovo_trainer trainer;
    krr_trainer<rbf_kernel> rbf_trainer;
    rbf_trainer.set_kernel(rbf_kernel(GAMMA_DEFAULT));
    trainer.set_trainer(rbf_trainer);

    NormalizedFunction decisionFunction;
    decisionFunction.normalizer = normalizer;
    decisionFunction.function = trainer.train(normalized_realty_data, labels);
    serialize(filename) << decisionFunction;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
