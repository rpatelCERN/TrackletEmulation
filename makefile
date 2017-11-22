CC=g++
CFLAGS=
LIBS=`root-config --cflags --glibs` -L $(ROOTSYS)/lib -I$(ROOTSYS)/include
INCLUDES=-I$(CMSSW_BASE)/src/L1Trigger/TrackFindingTracklet/test/TrackletEmulation/ -I$(CMSSW_BASE)/src -I$(CMSSW_BASE)/src/cxxopts/include/
EXECS=tracklet_em_L1L2
$(EXECS):L1_cluster.cpp L2_cluster.cpp tracklet_em_L1L2.cpp
all:$(EXECS) 
#tracklet_em_L1L2:
#	$(CC) $(CFLAGS) $(LIBS) $(INCLUDES) tracklet_em_L1L2.cpp -o $@
