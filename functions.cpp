#include "functions.h"
#include <math.h>
#include <thread>

using namespace dlib;

bool IsFileExist(const std::string& fileName)
{
  std::ifstream file(fileName);
  return file.good();
}

double PointsDistance(double x1,
                      double y1,
                      double x2,
                      double y2)
{
  x1 -= x2;
  x1 *= x1;
  y1 -= y2;
  y1 *= y1;
  x1 += y1;
  return sqrt(x1);
}

void Clusterize(const kernel_type& kernel,
                const double tolerance,
                const unsigned long max_dictionary_size,
                const unsigned long cluster_num,
                const std::vector<RealtyData>& realty_data,
                std::vector<double>& labels)
{
  // Here we declare an instance of the kcentroid object.  It is the object used to 
  // represent each of the centers used for clustering.  The kcentroid has 3 parameters 
  // you need to set.  The first argument to the constructor is the kernel we wish to 
  // use.  The second is a parameter that determines the numerical accuracy with which 
  // the object will perform part of the learning algorithm.  Generally, smaller values 
  // give better results but cause the algorithm to attempt to use more dictionary vectors 
  // (and thus run slower and use more memory).  The third argument, however, is the 
  // maximum number of dictionary vectors a kcentroid is allowed to use.  So you can use
  // it to control the runtime complexity.
  kcentroid<kernel_type> kc(kernel, tolerance, max_dictionary_size);

  // Now we make an instance of the kkmeans object and tell it to use kcentroid objects
  // that are configured with the parameters from the kc object we defined above.
  kkmeans<kernel_type> kkmeansObj(kc);

  // tell the kkmeans object we made that we want to run k-means with k set to cluster_num. 
  // (i.e. we want cluster_num clusters)
  kkmeansObj.set_number_of_centers(cluster_num);

  std::vector<RealtyData> initial_centers;

  // You need to pick some initial centers for the k-means algorithm.  So here
  // we will use the dlib::pick_initial_centers() function which tries to find
  // n points that are far apart (basically).  
  pick_initial_centers(cluster_num, initial_centers, realty_data, kkmeansObj.get_kernel());

  // now run the k-means algorithm on our set of samples.  
  kkmeansObj.train(realty_data, initial_centers);

  for(const auto& data : realty_data) {
    labels.push_back(kkmeansObj(data));
  }
}

static const long FOLD_COUNT_DEFAULT = 3;

size_t GetMinItemsInCluster(const std::vector<double>& labels)
{
  std::vector<size_t> counts(labels.size(), 0);
  for(const auto& label : labels) {
    ++counts[label];
  }
  return *std::min_element(counts.cbegin(), counts.cend());
}

double FindBestGammaKRR(std::vector<RealtyData>& realty_data,
                        std::vector<double>& labels)
{
  double bestAccuracy{0}, bestGamma{0};
  long foldCount{FOLD_COUNT_DEFAULT};
  auto minItemsInCluster = GetMinItemsInCluster(labels);
  foldCount = foldCount < minItemsInCluster
              ? foldCount
              : minItemsInCluster;
  ovo_trainer trainer;
  trainer.set_num_threads(std::thread::hardware_concurrency());

  for (double gamma = 0.000001; gamma <= 1; gamma *= 5)
  {
    krr_trainer<rbf_kernel> rbf_trainer;
    rbf_trainer.use_classification_loss_for_loo_cv();
    rbf_trainer.set_kernel(rbf_kernel(gamma));
    trainer.set_trainer(rbf_trainer);

    const matrix<double> confusion_matrix = cross_validate_multiclass_trainer(trainer, realty_data, labels, foldCount);
    double accuracy = sum(diag(confusion_matrix)) / sum(confusion_matrix);
    if(bestAccuracy < accuracy) {
      bestGamma = gamma;
    }
  }
  return bestGamma;
}
