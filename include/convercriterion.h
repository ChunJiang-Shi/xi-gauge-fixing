// -*- C++ -*-
/*! \file
 * \brief get R_xi gauge fixing convergence criterion
 *
 * Support for Rxi gauge fixing
 */

#ifndef __CONVERCRITERION_H__
#define __CONVERCRITERION_H__


namespace RxiGF{
    using namespace QDP;
    Double convercriterion(const multi1d<LatticeColorMatrix>& u, const LatticeColorMatrix& lambda);

    void getdelta(LatticeColorMatrix& delta, const multi1d<LatticeColorMatrix>& u, const LatticeColorMatrix& lambda);


    void traceless(LatticeColorMatrix& Ma);
}//end namespace RxiGF

#endif

