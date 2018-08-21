#pragma once

#include <dlib/clustering.h>
#include <dlib/any.h>
#include <dlib/svm.h>
#include <dlib/svm_threaded.h>

// Here we declare that our samples will be 7 dimensional column vectors.  
// (Note that if you don't know the dimensionality of your vectors at compile time
// you can change the 7 to a 0 and then set the size at runtime)
using RealtyData = dlib::matrix<double,7,1>;

// Now we are making a typedef for the kind of kernel we want to use.  I picked the
// radial basis kernel because it only has one parameter and generally gives good
// results without much fiddling.
using kernel_type = dlib::radial_basis_kernel<RealtyData>;

// In this example program we will work with a one_vs_one_trainer object which stores any 
// kind of trainer that uses our RealtyData samples.
using ovo_trainer = dlib::one_vs_one_trainer<dlib::any_trainer<RealtyData>>;

// Next, we will make two different binary classification trainer objects.  One
// which uses kernel ridge regression and RBF kernels and another which uses a
// support vector machine and polynomial kernels.  The particular details don't matter.
// The point of this part of the example is that you can use any kind of trainer object
// with the one_vs_one_trainer.
using poly_kernel = dlib::polynomial_kernel<RealtyData>;
using rbf_kernel = dlib::radial_basis_kernel<RealtyData>;

using DecisionFunction = dlib::one_vs_one_decision_function<ovo_trainer, dlib::decision_function<kernel_type>>;

using NormalizedFunction = dlib::normalized_function<DecisionFunction>;
