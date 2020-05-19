#include "qdp.h"
#include "../include/convercriterion.h"

namespace RxiGF{
    using namespace QDP;
    Double convercriterion(const multi1d<LatticeColorMatrix>& u, const LatticeColorMatrix& lambda){
        LatticeColorMatrix delta;
        getdelta(delta , u , lambda);
        Double ret;
        ret = sum(real(trace(delta * adj(delta))) );
        ret /= Double(Layout::vol()*Nc);
        
        //debug: imag part == 0 , correct! So don't warry about real() here.
        /*
        Double impart;
        impart = sum(imag(trace(delta * adj(delta))) ) / Double(Layout::vol()*Nc);
        QDPIO:: cout << "imag part = " << impart << std::endl; 
        */
        /*
        if(toBool(ret < 1e-8)){
          //Debug:-------------------------------
            printf("Debug: the convergence delta in lattice ilat = 7:\n");
            //LatticeColorMatrix debug = g * lambda_input; 
            for(int i = 0; i < Nc; i++){
                for (int j = 0; j < Nc; j++){
                std::cout << delta.elem(7).elem().elem(i,j);
                }
                printf("\n");
            }
            printf("-----------------------------\n");
            //-------------------------------------
        }
        */
        return ret;
    }

    void getdelta(LatticeColorMatrix& delta, const multi1d<LatticeColorMatrix>& u, const LatticeColorMatrix& lambda){
        delta = 0;
        LatticeColorMatrix delta_tmp; 
        for (int mu = 0; mu < Nd; mu++){
            delta_tmp = u[mu] - adj(u[mu]);
            traceless(delta_tmp);
            delta = delta + delta_tmp - shift(delta_tmp, BACKWARD, mu);
        }

        //times -i/2
        LatticeReal t1 = 0, t2 = -0.5;
        delta *= cmplx(t1, t2); 
        
        
        /*
        //Debug:-------------------------------
            printf("Debug: no lambda delta in lattice ilat = 7:\n");
            //LatticeColorMatrix debug = g * lambda_input; 
            for(int i = 0; i < Nc; i++){
                for (int j = 0; j < Nc; j++){
                std::cout << delta.elem(7).elem().elem(i,j);
                }
                printf("\n");
            }
            printf("-----------------------------\n");
            //-------------------------------------
        */

        delta = delta - lambda;
        
        
            
        
    }

    void traceless(LatticeColorMatrix& Ma){
        LatticeComplex mytrace;
        LatticeReal t1 = 1/3.0;
        mytrace = trace(Ma) * t1;
        Ma -= mytrace;
        /*
        REAL tracere, traceim;
        for(int ilat = 0; ilat < Layout::vol(); ilat++){
            tracere = 0; traceim = 0;
            for (int i = 0; i < Nc; i++)
            {
                tracere += Ma.elem(ilat).elem().elem(i,i).real();
                traceim += Ma.elem(ilat).elem().elem(i,i).imag();
            }
            tracere /= 3.0; traceim /= 3.0;
            for (int i = 0; i < Nc; i++)
            {
                Ma.elem(ilat).elem().elem(i,i).real() -= tracere;
                Ma.elem(ilat).elem().elem(i,i).imag() -= traceim;
            }
        }
        */
    }
}//end namespace RxiGF