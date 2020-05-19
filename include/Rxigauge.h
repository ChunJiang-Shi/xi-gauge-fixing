// -*- C++ -*-
/*! \file
 *  \a test code for R_xi gauge (and Landau) gauge fixing by ShiChunJiang 
 * 
 * Adapted from Chroma!!!!
 */

#ifndef __Rxigauge_h__
#define __Rxigauge_h__

using namespace QDP;

namespace Chroma {
//! R_xi (and Landau) gauge fixing
/*!
 * \ingroup gfix
 *
 * Driver for gauge fixing to R_xi gauge in slices perpendicular
 * to the direction "j_decay".
 * If j_decay >= Nd: fix to Landau gauge.
 * Note: as written this works only for SU(2) and SU(3)!

 * \param u        (gauge fixed) gauge field ( Modify )
 * \param lambda   the gauge condition Lambda(x) ( Read )
 * \param g        Gauge transformation matrices (Write)
 * \param n_gf     number of gauge fixing iterations ( Write )
 * \param j_decay  direction perpendicular to slices to be gauge fixed ( Read )
 * \param GFAccu   desired accuracy for gauge fixing ( Read )
 * \param GFMax    maximal number of gauge fixing iterations ( Read )
 * \param OrDo     use overrelaxation or not ( Read )
 * \param OrPara   overrelaxation parameter ( Read )
 */


void RxiGauge(multi1d<LatticeColorMatrix>& u, 
           const LatticeColorMatrix& lambda,
	       LatticeColorMatrix& g,
	       int& n_gf, 
	       int j_decay, const Real& GFAccu, int GFMax, 
	       bool OrDo, const Real& OrPara);
		   

//! R_xi (and Landau) gauge fixing
/*!
 * \ingroup gfix
 *
 * Driver for gauge fixing to R_xi gauge in slices perpendicular
 * to the direction "j_decay".
 * If j_decay >= Nd: fix to Landau gauge.
 * Note: as written this works only for SU(2) and SU(3)!

 * \param u        (gauge fixed) gauge field ( Modify )
 * \param n_gf     number of gauge fixing iterations ( Write )
 * \param lambda   the gauge condition Lambda(x) ( Read )
 * \param j_decay  direction perpendicular to slices to be gauge fixed ( Read )
 * \param GFAccu   desired accuracy for gauge fixing ( Read )
 * \param GFMax    maximal number of gauge fixing iterations ( Read )
 * \param OrDo     use overrelaxation or not ( Read )
 * \param OrPara   overrelaxation parameter ( Read )
 */

void RxiGauge(multi1d<LatticeColorMatrix>& u, 
           const LatticeColorMatrix& lambda,
	       int& n_gf, 
	       int j_decay, const Real& GFAccu, int GFMax, 
	       bool OrDo, const Real& OrPara);

}; // End namespace Chroma

#endif
