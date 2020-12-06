
CXX = g++
SRC_PATH = ./src

HEAD_PATH = -I ./include
HEAD_PATH_THIRD = -I ../../include

Lib_PATH_ASTRA = ../../lib
Lib_PATH_OPENCV = ./opencv-4.5/lib

SO_FILES = -lopencv_core -lastra_core -lastra_core_api -lastra -lpthread

SRC_CPP += $(wildcard $(SRC_PATH)/*.cpp)

3DPersonConstruct:
	$(CXX) -D LINUX -Wl,path=$(Lib_PATH_ASTRA),$(Lib_PATH_OPENCV) $(SRC_CPP) $(HEAD_PATH) $(HEAD_PATH_THIRD)  -L $(Lib_PATH_ASTRA) -L $(Lib_PATH_OPENCV) $(SO_FILES) -o 3DPersonConstruct 