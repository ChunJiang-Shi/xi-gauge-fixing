#ifndef __INPUTXML_H__
#define __INPUTXML_H__

using namespace Chroma;
extern "C" { 
 void _mcleanup();
};

bool linkageHack(void)
{
  bool foo = true;

  // Inline Measurements
  foo &= InlineAggregateEnv::registerAll();
  foo &= GaugeInitEnv::registerAll();
  //foo &= InlineMyMeasEnv::registerAll();

  return foo;
}

struct Params_t
{
  multi1d<int>    nrow;
  std::string     inline_measurement_xml;

  /*add xi gauge fixing parameters*/
  REAL          xi;
  REAL          beta;
  Real          gfaccu;
  int           gfmax;

  bool          ordo;
  Real          orpara;
  int           orstart;

  
  std::string     cfg_savepath;
  std::string     gaugetrans_savepath;
  std::string     lambda_savepath;
  
};

struct Inline_input_t
{
  Params_t        param;
  GroupXML_t      cfg;
  QDP::Seed       rng_seed;
};


void read(XMLReader& xml, const std::string& path, Inline_input_t& p) ;
void read(XMLReader& xml, const std::string& path, Params_t& p);

void read(XMLReader& xml, const std::string& path, Inline_input_t& p) 
{
  try {
    XMLReader paramtop(xml, path);
    
    read(paramtop, "Param", p.param);
    
    p.cfg = readXMLGroup(paramtop, "Cfg", "cfg_type");

    if (paramtop.count("RNG") > 0)
      read(paramtop, "RNG", p.rng_seed);
    else
      p.rng_seed = 11;     // default seed
      
  }
  catch( const std::string& e ) 
  {
    QDPIO::cerr << "Error reading XML : " << e << std::endl;
    QDP_abort(1);
  }
}


void read(XMLReader& xml, const std::string& path, Params_t& p) 
{
  XMLReader paramtop(xml, path);
  read(paramtop, "nrow", p.nrow);
  
  /*add R xi gauge fixing params*/
  read(paramtop, "xi", p.xi);
  read(paramtop, "beta", p.beta);
  read(paramtop, "GFAccu", p.gfaccu);
  read(paramtop, "GFMax", p.gfmax);
  read(paramtop, "OrDo", p.ordo);
  read(paramtop, "OrPara", p.orpara);
  read(paramtop, "OrStart", p.orstart);

  
  read(paramtop, "Cfg_savepath", p.cfg_savepath);
  read(paramtop, "gaugetrans_savepath", p.gaugetrans_savepath);
  read(paramtop, "Lambda_savepath", p.lambda_savepath);
  
  

  XMLReader measurements_xml(paramtop, "InlineMeasurements");
  std::ostringstream inline_os;
  measurements_xml.print(inline_os);
  p.inline_measurement_xml = inline_os.str();
  QDPIO::cout << "InlineMeasurements are: " << std::endl;
  QDPIO::cout << p.inline_measurement_xml << std::endl;
}


#endif