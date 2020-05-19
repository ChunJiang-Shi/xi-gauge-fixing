//test wilson loop U_p for a gauge field

#ifndef __WILSONLOOP_H__
#define __WILSONLOOP_H__

#include "chromabase.h"

namespace RxiGF{
    using namespace QDP;

Double getUp(const multi1d<LatticeColorMatrix>& u);
    
Double getUp(const multi1d<LatticeColorMatrix>& u){
    Double ret = 0;
    LatticeColorMatrix utmp = 0;
    for (int mu1 = 0; mu1 < Nd; mu1++){
        for (int mu2 = 0; mu2 < Nd; mu2++){   
            if(mu1 != mu2){
                utmp = u[mu1] * shift(u[mu2], FORWARD, mu1) * shift( adj(u[mu1])  , FORWARD, mu2) * adj(u[mu2]);
                ret += sum(real(trace(utmp)));

                //utmp = u[mu2] * shift( u[mu1]  , FORWARD, mu2) * shift(adj(u[mu2]), FORWARD, mu1) * adj(u[mu1])   ;;
                //ret += sum(real(trace(utmp)));
            }
        }
    }
    
    ret /= 12 * Nc * Layout::vol();
    return ret;
}

}//end namespace RxiGF
#endif