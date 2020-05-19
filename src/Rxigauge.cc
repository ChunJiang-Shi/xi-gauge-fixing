/*! \file
 *  \a test code for R_xi gauge (and Landau) gauge fixing by ShiChunJiang 
 * 
 * Adapted from Chroma!!!!
 */

/* The old INCLUDE PATH
#include "chromabase.h"
#include "meas/gfix/Rxigauge.h"
#include "meas/gfix/Rxigrelax.h"
#include "util/gauge/reunit.h"
*/

//in my folder PATH
#include <ctime>
#include "../include/chromabase.h"

#include "../include/Rxigauge.h"
#include "../include/Rxigrelax.h"
#include "../include/reunit.h"
#include "../include/convercriterion.h"
//#include "../include/Rxigaugeoptimization.h"


namespace Chroma {
using namespace QDP;

/********************** HACK ******************************/
// Primitive way for now to indicate the time direction
static int tDir() {return Nd-1;}
static Real xi_0() {return 1.0;}
/******************** END HACK ***************************/


//! a test code for R_xi gauge (and Landau) gauge fixing by ShiChunJiang 
/*!
 * \ingroup gfix
 *
 * Driver for gauge fixing to Coulomb gauge in slices perpendicular
 * to the direction "j_decay".
 * If j_decay >= Nd: fix to Landau gauge.
 * Note: as written this works only for SU(2) and SU(3)!

 * \param u        (gauge fixed) gauge field ( Modify )
 * \param lambda   the gauge condition Lambda(x) ( Read )
 * \param n_gf     number of gauge fixing iterations ( Write )
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
	       bool OrDo, const Real& OrPara)
{
  LatticeColorMatrix g;

  RxiGauge(u, lambda, g, n_gf, j_decay, GFAccu, GFMax, OrDo, OrPara);
}



//! a test code for R_xi gauge (and Landau) gauge fixing by ShiChunJiang 
/*!
 * \ingroup gfix
 *
 * Driver for gauge fixing to Coulomb gauge in slices perpendicular
 * to the direction "j_decay".
 * If j_decay >= Nd: fix to Landau gauge.
 * Note: as written this works only for SU(2) and SU(3)!

 * \param u        (gauge fixed) gauge field ( Modify )
 * \param lambda   the gauge condition Lambda(x) ( Read )
 * \param g        Gauge transformation matrices (Write)
 * \param n_gf     number of gauge fixing iterations ( Write )
 * \param j_decay  direction perpendicular to slices to be gauge fixed ( Read )    \\????
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
	       bool OrDo, const Real& OrPara)
{
  Double tgfold;
  Double tgfnew;
  Double tgf_t;
  Double tgf_s;
  Double norm;
  int num_sdir;
  bool tdirp;

  //MY HACK
  bool MyOrDo = OrDo;
  Real MyOrPara = OrPara;
  

  //LatticeColorMatrix total_g = 1;

  Double xigftheta = 100;//xigf theta init = 100

  /*for modify u_tmp[mu]*/  
  multi1d<LatticeColorMatrix> u_iter(Nd);
  LatticeColorMatrix ltmp;
  REAL rtmp;

  START_CODE();

  //Set xi parameter, 
  Real xi_sq = pow(xi_0(),2);

  //judge time and space number, get norm
  if( j_decay >= 0 && j_decay < Nd )
  {
    if( tDir() >= 0 && tDir() != j_decay ) //tDir means time direction
    {
      num_sdir = Nd - 2;                   //num_sdir means number of space direction
      tdirp = true;                        //time direction perpendicular
      norm = Double(Layout::vol()*Nc) * (Double(num_sdir)+Double(xi_sq));
    }
    else
    {
      num_sdir = Nd - 1;                   // I gauss Couloum gauge
      tdirp = false;
      norm = Double(Layout::vol()*Nc*num_sdir);
    }
  }
  else
  {
    if( tDir() >= 0 && tDir() < Nd )
    {
      num_sdir = Nd - 1;
      tdirp = true;
      norm = Double(Layout::vol()*Nc) * (Double(num_sdir)+Double(xi_sq));
    }
    else
    {
      num_sdir = Nd;
      tdirp = false;
      norm = Double(Layout::vol()*Nc*num_sdir);
    }
  }

      
  /* Compute initial gauge fixing term: sum(trace(U_spacelike)); */
  tgf_t = 0;
  tgf_s = 0;
  for(int mu=0; mu<Nd; ++mu)
    //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    if( mu != j_decay )
    {
      Double tgf_tmp = sum(real(trace(u[mu])));

      if( mu != tDir() )
	tgf_s += tgf_tmp;
      else
	tgf_t += tgf_tmp;
    }

  if( tdirp )
  {
    tgfold = (xi_sq*tgf_t+tgf_s )/norm;
    tgf_s = tgf_s/(Double(Layout::vol()*Nc*num_sdir));
    tgf_t = tgf_t/(Double(Layout::vol()*Nc));
  }
  else
  {
    tgf_s = tgf_s/(Double(Layout::vol()*Nc*num_sdir));
    tgfold = tgf_s;
  }

  // Rxi gauge fixing convergence criterion
  //xigftheta = RxiGF::convercriterion(u, lambda);
  
  // Gauge transf. matrices always start from identity
  g = 1; 

  /* Gauge fix until converged or too many iterations */
  n_gf = 0;
  bool wrswitch = true;    /* switch for writing of gauge fixing term */
  Double conver = 1;        /*old Coulomb gauge fixing convergence criterion */

  /*record for debug time */
  time_t t_gfstart, t_iteration;
  time(&t_gfstart);

  while( toBool(xigftheta > GFAccu)  &&  n_gf < GFMax )
  {
    n_gf = n_gf + 1;
    if( GFMax - n_gf < 11 ) 
      wrswitch = true;

    /*
    if(toBool(xigftheta < 1e-6) && OrDo){
      MyOrPara = 1.7;
      MyOrDo = true;
    }
    */
    


    /* Loop over checkerboards for gauge fixing */
    int N_cb = 2;
    for(int cb=0; cb<N_cb; ++cb)
    {
      if (Nc > 1)
      {
        /* Loop over SU(2) subgroup index */
        for(int su2_index=0; su2_index < Nc*(Nc-1)/2; ++su2_index)
        {
          /* Now do a gauge fixing relaxation step */
          //Rxigrelax(g, lambda, u, j_decay, su2_index, cb, OrDo, OrPara);
          Rxigrelax(g, lambda, u, j_decay, su2_index, cb, MyOrDo, MyOrPara); //HACK CHROMA
          //reunit(g);
        }   /* end su2_index loop */
      }
      else // Nc==1 case 
      {
        int su2_index = -1;
        /* Now do a gauge fixing relaxation step */
        //Rxigrelax(g, lambda, u, j_decay, su2_index, cb, OrDo, OrPara);
        Rxigrelax(g , lambda, u , j_decay, su2_index, cb, OrDo, OrPara); 
      }
    }     /* end cb loop */

    /* Reunitarize */
    reunit(g);
    /* now we get new iteration g(x) */


  //Debug:-------------------------------
  if (toBool(n_gf % 1000 ==0))
  {
    printf("Debug: the ColorMatrix (g) in lattice ilat = 7:\n");
    //LatticeColorMatrix debug = g * lambda_input; 
    for(int i = 0; i < Nc; i++){
      for (int j = 0; j < Nc; j++){
        std::cout << g.elem(7).elem().elem(i,j);
      }
      printf("\n");
    }
    printf("-----------------------------\n");
  }
  //-------------------------------------


  //???? question ????
  //total_g *= g;
  //total_g = g * total_g;

  //Debug:-------------------------------
  /*
  if (toBool(n_gf % 1000 == 0))
  {
    printf("Debug: the total rot total_g in lattice ilat = 7:\n");
    //LatticeColorMatrix debug = g * lambda_input; 
    for(int i = 0; i < Nc; i++){
      for (int j = 0; j < Nc; j++){
        std::cout << total_g.elem(7).elem().elem(i,j);
      }
      printf("\n");
    }
    printf("-----------------------------\n");
  }
  */
  //-------------------------------------

    /* Compute new gauge fixing term: sum(trace(U_spacelike)): */
    tgf_t = 0;
    tgf_s = 0;
    for(int mu=0; mu<Nd; ++mu)
      if( mu != j_decay )
      {
	Double tgf_tmp = sum(real(trace(g * u[mu] * shift(adj(g), FORWARD, mu))) );  

	if( mu != tDir() )
	  tgf_s += tgf_tmp;
	else
	  tgf_t += tgf_tmp;
      }

    if( tdirp )
    {
      tgfnew = (xi_sq*tgf_t+tgf_s + sum(imag(trace(g * lambda))) )/norm;
      //RxiGF:modified XXXXXXXXXXXXXXXXXXXXXXXXXX
      tgf_s = tgf_s/(Double(Layout::vol()*Nc*num_sdir));
      tgf_t = tgf_t/(Double(Layout::vol()*Nc));
    }
    else
    {
      tgf_s = tgf_s/(Double(Layout::vol()*Nc*num_sdir));
      /*Debug : Rxi gauge should not do this.*/
      printf("Error: tdirp == false !!! Rxi gauge should not do this \n");
      tgfnew = tgf_s;
    }

    //debug:
    //QDPIO::cout << "normed trace of(i*g*lambda)= " << (sum(imag(trace(g * lambda))) )/norm<<std::endl;
    
    /*Old convergence criterion, do not use it in RxiGf*/
    /* Normalized convergence criterion: */
    //conver = fabs((tgfnew - tgfold) / tgfnew);
    conver = fabs(tgfnew - tgfold);


  /*
  //Debug:-------------------------------
  printf("Debug: after rot, u[0] in lattice ilat = 7:\n");
  LatticeColorMatrix debug2 = u[0]; 
  for(int i = 0; i < Nc; i++){
    for (int j = 0; j < Nc; j++){
      std::cout << debug2.elem(7).elem().elem(i,j);
    }
    printf("\n");
  }
  printf("-----------------------------\n");
  //End debug-------------------------------------
  */

  // Rxi gauge fixing convergence criterion
  //all begin with xigf
  for(int mu = 0; mu < Nd; ++mu)
  {
    LatticeColorMatrix u_tmp1 = g * u[mu];
    u_iter[mu] = u_tmp1 * shift(adj(g), FORWARD, mu);
  }
  xigftheta = RxiGF::convercriterion(u_iter, lambda);

    time(&t_iteration);

    if( wrswitch && toBool(n_gf % 100 == 0)) 
    printf("\n");
      QDPIO::cout << "R_xiGAUGE: iter= " << n_gf
      << "  OrDo= " << MyOrDo 
      << "  OrPara= " << MyOrPara
      << "  theta= " << xigftheta 
      << "  delta= " << tgfnew - tgfold
      << "  time(sec)= " << (t_iteration-t_gfstart)
		  << "  tgfold= " << tgfold 
		  << "  tgfnew= " << tgfnew
		  << "  tgf_s= " << tgf_s 
		  << "  tgf_t= " << tgf_t << std::endl;

    
    tgfold = tgfnew;
  }       /* end while loop */

      
  if( wrswitch && toBool(n_gf % 500 == 0)) 
      QDPIO::cout << "R_xiGAUGE: iter= " << n_gf
      << "  theta= " << xigftheta  
      << "  OrDo= " << MyOrDo 
      << "  delta= " << tgfnew - tgfold
      << "  time(sec)= " << (t_iteration-t_gfstart)
		  << "  tgfold= " << tgfold 
		  << "  tgfnew= " << tgfnew
		  << "  tgf_s= " << tgf_s 
		  << "  tgf_t= " << tgf_t << std::endl;


  // Finally, gauge rotate the original matrices and overwrite them
    //！！！！！！！！！！！！！！！！！！！！！BUG！！
  for(int mu = 0; mu < Nd; ++mu)
  {
    LatticeColorMatrix u_tmp2 = g * u[mu];
    u[mu] = u_tmp2 * shift(adj(g), FORWARD, mu);
  }
    
#if 0
  /*+ debugging */
  XMLBufferWriter xml_out;
  push(xml_out,"Final_trace_max_in_R_xiGauge");
  write(xml_out, "j_decay", j_decay);
  write(xml_out, "t_dir", tDir());
  write(xml_out, "n_gf",n_gf);
  write(xml_out, "tgfold", tgfold);
  write(xml_out, "tgf_s", tgf_s);
  write(xml_out, "tgf_t", tgf_t);
  pop(xml_out);
#endif

  END_CODE();
}


} // Namespace Chroma
