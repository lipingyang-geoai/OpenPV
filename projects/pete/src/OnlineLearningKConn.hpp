/*
 * OnlineLearningKConn.hpp
 *
 *  Created on: Sep 12, 2012
 *      Author: pschultz
 */

#ifndef ONLINELEARNINGKCONN_HPP_
#define ONLINELEARNINGKCONN_HPP_

#include <connections/KernelConn.hpp>

namespace PV {

class OnlineLearningKConn: public PV::KernelConn {
public:
   OnlineLearningKConn(const char * name, HyPerCol * hc);
   virtual ~OnlineLearningKConn();

protected:
   OnlineLearningKConn();
   int initialize(const char * name, HyPerCol * hc);

private:
   int initialize_base();


protected:
   HyPerLayer * sourceLayer;
   pvdata_t * postpostOuterProduct;
   pvdata_t * prepostOuterProduct;
};

} /* namespace PV */
#endif /* ONLINELEARNINGKCONN_HPP_ */