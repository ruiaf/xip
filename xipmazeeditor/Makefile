SRC = ArrowTool.cpp Beacon.cpp BeaconPropertiesBox.cpp BeaconTool.cpp DefaultConfs.cpp ExportDialog.cpp MazeCanvas.cpp MazeCanvasPropertiesBox.cpp MazeFileDialog.cpp MazeFileHandler.cpp MazeXMLTag.cpp MyApp.cpp MyFrame.cpp Polygon.cpp StartingGrid.cpp StartingGridPropertiesBox.cpp StartingGridTool.cpp Wall.cpp WallPropertiesBox.cpp WallTool.cpp
BIN = MazeEditor.exe




#######################################################################

SRCDIR= .
BINDIR = bin
TMPDIR = tmp
DATADIR = data

LDFLAGS_GUI = -mwindows
CXX = g++
CXXFLAGS = -g -O0 -mthreads -Wall -Wundef -Wno-ctor-dtor-privacy
CPPFLAGS = -DWX_PRECOMP -DNO_GCC_PRAGMA -D__WXDEBUG__ `wx-config --cppflags`
LDFLAGS =  `wx-config --libs`
TOOLKIT = MSW
TOOLKIT_LOWERCASE = msw
EXTRALIBS =  -mthreads  -lrpcrt4 -loleaut32 -lole32 -luuid -lwinspool -lwinmm -lshell32 -lcomctl32 -lcomdlg32 -lctl3d32 -ladvapi32 -lwsock32 -lgdi32
EXTRALIBS_OPENGL =  -lopengl32 -lglu32


SRCWITHPATH = $(addprefix $(SRCDIR)/, $(SRC))

all: $(SRCWITHPATH:.cpp=.o)
	$(CXX) -o $(BINDIR)/$(BIN) $(addprefix $(TMPDIR)/,$(notdir $(SRC:.cpp=.o))) $(LDFLAGS)
	# cp -r $(DATADIR)/* $(BINDIR)/
	@echo -e "\n\n--- Finished compilation process ---\n"


.cpp.o: $(SRCWITHPATH)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -c -o $(TMPDIR)/$(notdir $@)


clean:
	rm -rvf $(TMPDIR)/*.o $(BINDIR)/*




