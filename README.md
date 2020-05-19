### README
This code is an implement on renomalizable-xi gauge in Lattice QCD.

Brief introduction for RxiGF , c++ files used to do R_xi gauge fixing on a gauge field by [Chunjiang Shi](shichunjiang16@mails.ucas.ac.cn) .  

-  Based on QDP++,  the  ``` PATH/qdp++-config ``` should be modified in ```Makefile```, and ```make``` to compile all. 
-  Files ``` Rxigauge.cc``` / ```Rxigrelex.cc``` modified from ``` chroma```, but you DON'T need install or compile ```chroma``` . 
-  ```*.cc``` files put in ```./src``` ; ```*.h``` in ./```include``` 

------------------------------------------------------------------------------------

#### File  Documentation

- ```main_RxiGF.cc``` 
    - define lattice 
    - generate, write or read a configuration file
    - generate xi gauge condition  Lambda(x)  on the latt.
    - do xi gauge fixing iteration
- ```getlambda.h``` generates xi gauge condition Lambda(x). At each site it generates 8 gaussian number using Box-Muller transform, defines Gall-Mann Matrix and combines into Lambda(x).
- ```Rxigauge.cc``` mainly do xi gauge fixing iteration. Note that  g(x) and Lambda(x) were  always modified in each iteration.
- ```Rxigrelex.cc``` uses  relaxation methods to get new g(x). Lambda(x) was finally delivered to here to do with SU(3) extract.
- ```convercriterion.cc``` gets convergence criterion theta.
-  ```chromabase.h``` / ```su2extract.h``` / ```sunfill.h``` support  for my ```namespace Chroma```.

