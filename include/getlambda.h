
#ifndef __GETLAMBDA_H__
#define __GETLAMBDA_H__

    namespace RxiGF{
        using namespace QDP;
    void getlambda(LatticeColorMatrix& lambda, const REAL xi);
    
    inline void fill_lambda(PColorMatrix< RComplex<REAL>, Nc>& lam , REAL r0,
                                                     REAL r1,
                                                     REAL r2,
                                                     REAL r3,
                                                     REAL r4,
                                                     REAL r5,
                                                     REAL r6,
                                                     REAL r7);


    void lambdafill(LatticeColorMatrix& d, const multi1d<LatticeReal>& r);

    void  lambdafill(LatticeColorMatrix& d, const Subset& s, const multi1d<LatticeReal>& r);
    }  //end namespace RxiGF

#endif