#ifndef __MY_INIT_H__
#define __MY_INIT_H__

namespace Chroma{
void myinitialize(int* argc, char ***argv);

void myinitialize(int* argc, char ***argv){
for(int i=0; i < *argc; i++) 
    {
      // Get argv[i] into a std::string
      std::string argv_i = std::string( (*argv)[i] );

      // Search for -i or --chroma-i
      if( argv_i == std::string("-h") || argv_i == std::string("--help") ) 
      {
	QDPIO::cerr << "Usage: " << (*argv)[0] << "  <options>" << std::endl
		    << "   -h           help\n"
		    << "   --help       help\n"
		    << "   -i           [" << getXMLInputFileName() << "]  xml input file name\n"
		    << "   --chroma-i   [" << getXMLInputFileName() << "]  xml input file name\n"
		    << "   -o           [" << getXMLOutputFileName() << "]  xml output file name\n"
		    << "   --chroma-p   [" << getXMLOutputFileName() << "]  xml output file name\n"
		    << "   -l           [" << getXMLLogFileName() << "]  xml output file name\n"
		    << "   --chroma-l   [" << getXMLLogFileName() << "]  xml log file name\n"
		    << "   -cwd         [" << getCWD() << "]  xml log file name\n"
		    << "   --chroma-cwd [" << getCWD() << "]  xml log file name\n"

		    
		    << std::endl;
	QDP_abort(0);
      }

      // Search for -i or --chroma-i
      if( argv_i == std::string("-i") || argv_i == std::string("--chroma-i") ) 
      {
	if( i + 1 < *argc ) 
	{
	  setXMLInputFileName(std::string( (*argv)[i+1] ));
	  // Skip over next
	  i++;
	}
	else 
	{
	  // i + 1 is too big
	  QDPIO::cerr << "Error: dangling -i specified. " << std::endl;
	  QDP_abort(1);
	}
      }
 
      // Search for -o or --chroma-o
      if( argv_i == std::string("-o") || argv_i == std::string("--chroma-o") ) 
      {
	if( i + 1 < *argc ) {
	  setXMLOutputFileName(std::string( (*argv)[i+1] ));
	  // Skip over next
	  i++;
	}
	else {
	  // i + 1 is too big
	  QDPIO::cerr << "Error: dangling -o specified. " << std::endl;
	  QDP_abort(1);
	}
      }
      
      // Search for -l or --chroma-l
      if( argv_i == std::string("-l") || argv_i == std::string("--chroma-l") ) 
      {
	if( i + 1 < *argc ) {
	  setXMLLogFileName(std::string( (*argv)[i+1] ));
	  // Skip over next
	  i++;
	}
	else {
	  // i + 1 is too big
	  QDPIO::cerr << "Error: dangling -l specified. " << std::endl;
	  QDP_abort(1);
	}
      }
      
      

    }


}
} //end namespace chroma
#endif