#include "types.h"

bool IsFileExist(const std::string& fileName);

double PointsDistance(double x1,
                      double y1,
                      double x2,
                      double y2);

void Clusterize(const kernel_type& kernel,
                const double tolerance,
                const unsigned long max_dictionary_size,
                const unsigned long cluster_num,
                const std::vector<RealtyData>& realty_data,
                std::vector<double>& labels);

double FindBestGammaKRR(std::vector<RealtyData>& realty_data,
                        std::vector<double>& labels);
