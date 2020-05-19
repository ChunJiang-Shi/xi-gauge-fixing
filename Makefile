#TOPDIR=/public/home/ybyang/opt/chroma_code
CHROMA=/public/home/shicj/soft/chroma_cpu_double/bin
CONFIG=$(CHROMA)/chroma-config

#QDPXX=/public/home/shicj/soft/qdpxx_double/bin
#CONFIG=$(QDPXX)/qdp++-config


CXX=$(shell $(CONFIG) --cxx) 
CXXFLAGS=$(shell $(CONFIG) --cxxflags) -I./include/Eigen
LDFLAGS=$(shell $(CONFIG) --ldflags) 
LIBS=$(shell $(CONFIG) --libs) 

HDRS =  ./include/chromabase.h \
		./include/getlambda.h \
	    ./include/Rxigauge.h \
	    ./include/Rxigrelax.h \
	    ./include/reunit.h \
		./include/su2extract.h \
		./include/sunfill.h \
		./include/convercriterion.h
		#./include/gfoperation.h \
		# ./include/gaussian8d.h \
		#./include/gallmannmatrix.h \ a bug: not include this, but it works indeed

%.o: %.cc $(HDRS)
	$(CXX) $(CXXFLAGS) -c $< 

OBJS =  main_RxiGF.o \
		./src/getlambda.o \
		./src/Rxigauge.o \
	    ./src/Rxigrelax.o \
	    ./src/reunit.o \
		./src/su2extract.o \
		./src/sunfill.o \
		./src/convercriterion.o
#./src/gallmannmatrix.o \

run_Debug: $(OBJS)
	$(CXX) -o $@ $(CXXFLAGS) *.o $(LDFLAGS) $(LIBS)


clean:
	rm -rf run $(OBJS) *~ *.o
