#include "qdp.h"
#include <iostream>
#include <string>
#include <random>
//#include "../include/gaussian8d.h"
//#include "../include/gallmannmatrix.h"
#include "../include/getlambda.h"

#ifndef ONE_DEVIDE_SQRT3
#define ONE_DEVIDE_SQRT3 REAL(0.57735026918962576450914878050195744)
#endif

    namespace RxiGF{
        using namespace QDP;
void getlambda(LatticeColorMatrix& lambda, const REAL xi){
        multi1d<LatticeReal> randnum(8); 
        multi1d<LatticeReal> mean(8); 
        
  QDPIO::cout << "start generate Lambda ..." << std::endl;
  LatticeReal sqrt_xi = sqrt(xi);
  for(int i = 0; i < 8 ; ++i){
    gaussian(randnum[i]);
    mean[i] = sum(randnum[i])/Layout::vol();
    //using a linear transformation to keep sum 0 with the same exception and variance
    randnum[i] = sqrt(Layout::vol()/(Layout::vol()-1)) * (randnum[i] - mean[i]);
    randnum[i] *= sqrt_xi/2.0;
  }

  QDPIO::cout << "********** randnum[0].elem(0).elem().elem().elem() = " << randnum[0].elem(0).elem().elem().elem() << std::endl;
  QDPIO::cout << "********** randnum[1].elem(0).elem().elem().elem() = " << randnum[1].elem(0).elem().elem().elem() << std::endl;
  
  lambdafill(lambda, randnum);
  QDPIO::cout << "generate Lambda done..." << std::endl;

  //Debug:------------------------------- 
  /*     
        printf("Debug: lambda in lattice ilat = 7:\n");
        //LatticeColorMatrix debug = g * lambda_input; 
        for(int i = 0; i < Nc; i++){
          for (int j = 0; j < Nc; j++){
            std::cout << lambda.elem(7).elem().elem(i,j);
          }
          printf("\n");
        }
        printf("-----------------------------\n");
  */
  //end debug-------------------------------------
}

inline void fill_lambda(PColorMatrix< RComplex<REAL>, Nc>& lam , 
                                                    REAL r0,
                                                    REAL r1,
                                                    REAL r2,
                                                    REAL r3,
                                                    REAL r4,
                                                    REAL r5,
                                                    REAL r6,
                                                    REAL r7){
  lam.elem(0,0).real() = r2 + r7*ONE_DEVIDE_SQRT3;
  lam.elem(0,0).imag() = 0;

  lam.elem(1,1).real() = -r2 + r7*ONE_DEVIDE_SQRT3;
  lam.elem(1,1).imag() = 0;

  lam.elem(2,2).real() = -2 * r7 * ONE_DEVIDE_SQRT3;
  lam.elem(2,2).imag() = 0;

  lam.elem(1,0).real() = r0;
  lam.elem(1,0).imag() = r1;

  lam.elem(0,1).real() = r0;
  lam.elem(0,1).imag() = -r1;

  lam.elem(2,0).real() = r3;
  lam.elem(2,0).imag() = r4;

  lam.elem(0,2).real() = r3;
  lam.elem(0,2).imag() = -r4;

  lam.elem(1,2).real() = r5;
  lam.elem(1,2).imag() = -r6;

  lam.elem(2,1).real() = r5;
  lam.elem(2,1).imag() = r6;
}


void lambdafill(LatticeColorMatrix& d, const multi1d<LatticeReal>& r)
{
	lambdafill(d,all,r);
}

//! dest	= gaussian	 under a subset
void  lambdafill(LatticeColorMatrix& d, const Subset& s, const multi1d<LatticeReal>& r)
{
	const int *tab = s.siteTable().slice();
	const int nodeSites = s.numSiteTable();

#pragma omp parallel for
	for(int j=0; j < nodeSites; ++j)
	{
		int i = tab[j];
		fill_lambda(d.elem(i).elem(), r[0].elem(i).elem().elem().elem(),
                           r[1].elem(i).elem().elem().elem(),
                           r[2].elem(i).elem().elem().elem(),
                           r[3].elem(i).elem().elem().elem(),
                           r[4].elem(i).elem().elem().elem(),
                           r[5].elem(i).elem().elem().elem(), 
                           r[6].elem(i).elem().elem().elem(),
                           r[7].elem(i).elem().elem().elem());

    /*
    fill_lambda(d.elem(tab[nodeSites/2-1-j]).elem(), 
                           -r[0].elem(i).elem().elem().elem(),
                           -r[1].elem(i).elem().elem().elem(),
                           -r[2].elem(i).elem().elem().elem(),
                           -r[3].elem(i).elem().elem().elem(),
                           -r[4].elem(i).elem().elem().elem(),
                           -r[5].elem(i).elem().elem().elem(), 
                           -r[6].elem(i).elem().elem().elem(),
                           -r[7].elem(i).elem().elem().elem());
                           */
	}
}

}//End namespace RxiGF

