#include <iostream>
#include <cstdio>
#include <ctime>

#include "chroma.h"
#include "qdp_iogauge.h"
#include "./include/Rxigfix.h"
#include "./include/getlambda.h"
#include "./include/wilsonloop.h"
#include "./include/inputxml.h"
#include "./include/myinitialize.h"



int main(int argc, char** argv){
  // Put the machine into a known state
  QDP_initialize(&argc, &argv);
  

  { //record start time
    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf("Running start time : %s \n", asctime(timeinfo));
  }

//this block is to define my lattce
#if 0
  //  Begin by specifying the lattice sizes, of dimension Nd, defined in QDP
  int Nd = 4;  
  multi1d<int> nsize(Nd);	// Note that the number of dimensions

  // defined in params.h

  //  For those new to C++, note that the vector class contains more 
  //  information than a simple array of elements, and in particular its size

 

  nsize = foo;
  // Initialise the layout
  Layout::setLattSize(nsize);
  Layout::create();

  int j_decay = Nd-1;
  int length = Layout::lattSize()[j_decay]; // Define the temporal direction

  
  
  
  multi1d<LatticeColorMatrix> u(Nd);

    /*
   *  Create a Random gauge field
   */
  std::cout << "u.size " << u.size() << std::endl;
  for(int mu = 0; mu < u.size(); mu++)
    gaussian(u[mu]);
#endif

//this block is to read a gauge field decleared in input xml file.
#if 1
   /*
   *  Read in a gauge field decleared in input xml file. 
   *  GTF: moved filename decl into scope to avoid compiler warning when #if 0
   */

    //char filename[100] = "/public/home/liuzf/confs/genwang/rbc_ken/2plus1_32nt64_IWASAKI_b2p25_ls16_M1p8_ms0p03_mu0p004_rhmc_H_R_G/rbc_conf_3264_m0.004_0.03_%06d.01_hyp";

    myinitialize(&argc, &argv); //There is a problem in my chroma on 419 machine, so a add init again. 
    std::string inputxmlpath = getXMLInputFileName();
    //char logxmlpath[100] = "/public/home/shicj/new_RxiGF/test/outputlog.xml";

    
    //QDPIO::cout << "Reading in NERSC file..." << std::endl;
    
    QDPIO::cout << "Reading in XML file..." << std::endl;
    QDPIO::cout << "Reading filename: " << inputxmlpath << std::endl;

  QDPIO::cout << "Linkage = " << linkageHack() <<  std::endl;  //Hack
  StopWatch snoop;
  snoop.reset();
  snoop.start();

  XMLReader xml_in;

  // Input parameter structure
  Inline_input_t  input;
  try
  {
    //xml_in.open(Chroma::getXMLInputFileName());
    //QDPIO::cout << "Chroma::getXMLInputFileName(): " << Chroma::getXMLInputFileName() << std::endl;
    xml_in.open(inputxmlpath);
    read(xml_in, "/chroma", input);
  }
  catch(const std::string& e) 
  {
    QDPIO::cerr << "CHROMA: Caught Exception reading XML: " << e << std::endl;
    QDP_abort(1);
  }
  catch(std::exception& e) 
  {
    QDPIO::cerr << "CHROMA: Caught standard library exception: " << e.what() << std::endl;
    QDP_abort(1);
  }
  catch(...)
  {
    QDPIO::cerr << "CHROMA: caught generic exception reading XML" << std::endl;
    QDP_abort(1);
  }

  XMLFileWriter& xml_out = Chroma::getXMLOutputInstance();
  //XMLFileWriter xml_out;
  //xml_out.open(logxmlpath);

  push(xml_out, "chroma");

  // Write out the input
  write(xml_out, "Input", xml_in);

  Layout::setLattSize(input.param.nrow);
  Layout::create();

  proginfo(xml_out);    // Print out basic program info


  
  // Initialise the RNG
  QDP::RNG::setrn(input.rng_seed);
  write(xml_out,"RNG", input.rng_seed);

  // Start up the config
  StopWatch swatch;
  swatch.reset();
  multi1d<LatticeColorMatrix> u(Nd);
  LatticeColorMatrix g = 1;
  multi1d<LatticeColorMatrix> save_g(1);
  XMLReader gauge_file_xml, gauge_xml;


  // Start up the gauge field
  QDPIO::cout << "Attempt to read gauge field" << std::endl;
  swatch.start();
  try 
  {
    std::istringstream  xml_c(input.cfg.xml);
    XMLReader  cfgtop(xml_c);
    QDPIO::cout << "Gauge initialization: cfg_type = " << input.cfg.id << std::endl;

    Handle< GaugeInit >
      gaugeInit(TheGaugeInitFactory::Instance().createObject(input.cfg.id,
							     cfgtop,
							     input.cfg.path));
    (*gaugeInit)(gauge_file_xml, gauge_xml, u);
  }
  catch(std::bad_cast) 
  {
    QDPIO::cerr << "CHROMA: caught cast error" << std::endl;
    QDP_abort(1);
  }
  catch(std::bad_alloc) 
  { 
    // This might happen on any node, so report it
    QDPIO::cerr << "CHROMA: caught bad memory allocation" << std::endl;
    QDP_abort(1);
  }
  catch(const std::string& e) 
  {
    QDPIO::cerr << "CHROMA: Caught Exception: " << e << std::endl;
    QDP_abort(1);
  }
  catch(std::exception& e) 
  {
    QDPIO::cerr << "CHROMA: Caught standard library exception: " << e.what() << std::endl;
    QDP_abort(1);
  }
  catch(...)
  {
    // This might happen on any node, so report it
    QDPIO::cerr << "CHROMA: caught generic exception during gaugeInit" << std::endl;
    QDP_abort(1);
  }
  swatch.stop();

  QDPIO::cout << "Gauge field successfully read: time= " 
	      << swatch.getTimeInSeconds() 
	      << " secs" << std::endl;

  XMLBufferWriter config_xml;
  config_xml << gauge_xml;

  // Write out the config header
  write(xml_out, "Config_info", gauge_xml);
#endif

  // R_xi gauge fixing paras
    const REAL xi = 2*Nc*input.param.xi/input.param.beta; //define real xi without beta
    const REAL beta = input.param.beta;
    int n_gf = 0;
    const Real GFAccu = input.param.gfaccu;
    int GFMax = input.param.gfmax;
    bool OrDo = input.param.ordo;
    int j_decay2 = -1;  //default t_dir = Nd-1 
                        //j_decay for Coulomb GF
                        //j_decay2 for Landau GF
    const Real OrPara = input.param.orpara;
  //----------------------------------------------------
  QDPIO::cout<<"xi = "<< input.param.xi <<"\n";
  QDPIO::cout<<"beta = "<< beta <<"\n";
  QDPIO::cout<<"GFAccu = " << GFAccu<<"\n";
  QDPIO::cout<<"GFMax = " << GFMax<<"\n";
  QDPIO::cout<<"Do Overrelaxlation ? = " << OrDo<<"\n";
  QDPIO::cout<<"OrPara = " << OrPara<<"\n";
  
  QDPIO::cout<<"Cfg save PATH = " << input.param.cfg_savepath<<"\n";
  QDPIO::cout<<"g(x) save PATH = " << input.param.gaugetrans_savepath<<"\n";
  QDPIO::cout<<"Lambda save PATH = " << input.param.lambda_savepath<<"\n";
  


  //generate gauge condition Lambda
  LatticeColorMatrix lambda;
  RxiGF::getlambda(lambda, xi);

  QDPIO::cout<<"Layout::sitesOnNode() = " <<Layout::sitesOnNode()<<"\n";
  QDPIO::cout<<"Layout::vol() = " <<Layout::vol()<<"\n";

  Double Up = RxiGF::getUp(u);
  QDPIO::cout<<"Wilson loop Up = " << Up <<"\n";

  //do Rxi gauge fixing 
  //Chroma::RxiGauge(u, lambda, n_gf, j_decay2, 1e-14, GFMax, false, OrPara);
  Chroma::RxiGauge(u, lambda, g, n_gf, j_decay2, GFAccu, GFMax, OrDo, OrPara);
  
  Up = RxiGF::getUp(u);
  QDPIO::cout<<"Wilson loop Up = " << Up <<"\n";

  QDPIO::cout<<"Saving ...  " <<"\n";
  Chroma::writeKYU(u, input.param.cfg_savepath);
  QDPIO::cout<<"Saving cfg done..... " <<"\n";
  save_g[0] = g; 
  Chroma::writeKYU(save_g, input.param.gaugetrans_savepath);
  QDPIO::cout<<"Saving g(x) done..... " <<"\n";
  save_g[0] = lambda; 
  Chroma::writeKYU(save_g, input.param.lambda_savepath);
  QDPIO::cout<<"Saving Lambda done..... " <<"\n";

  

// Time to bolt
  QDP_finalize();
  QDPIO::cout << "**********************************\n";
  QDPIO::cout<<"The END of CODE\n";
  exit(0);
}