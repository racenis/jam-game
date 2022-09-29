##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=jam-game
ConfigurationName      :=Debug
WorkspaceConfiguration :=Debug
WorkspacePath          :=C:/Users/Poga/Desktop/painis/tram
ProjectPath            :=C:/Users/Poga/Desktop/painis/jam-game
IntermediateDirectory  :=../tram/build-$(WorkspaceConfiguration)/__/jam-game
OutDir                 :=$(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Poga
Date                   :=30/09/2022
CodeLitePath           :="C:/Program Files/CodeLite"
MakeDirCommand         :=mkdir
LinkerName             :=C:/mingw64/bin/g++.exe
SharedObjectLinkerName :=C:/mingw64/bin/g++.exe -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputDirectory        :=C:/Users/Poga/Desktop/painis/tram/build-$(WorkspaceConfiguration)/bin
OutputFile             :=..\tram\build-$(WorkspaceConfiguration)\bin\$(ProjectName).exe
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :=$(IntermediateDirectory)/ObjectsList.txt
PCHCompileFlags        :=
RcCmpOptions           := 
RcCompilerName         :=C:/mingw64/bin/windres.exe
LinkOptions            :=  -lglfw3 -lgdi32 -lopengl32 -static-libgcc -static-libstdc++ -static
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)../tram-sdk/src $(IncludeSwitch)../tram-sdk/libraries 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)BulletSoftBody $(LibrarySwitch)BulletDynamics $(LibrarySwitch)BulletCollision $(LibrarySwitch)LinearMath $(LibrarySwitch)glfw3 $(LibrarySwitch)OpenAL32.dll 
ArLibs                 :=  "libBulletSoftBody.a" "libBulletDynamics.a" "libBulletCollision.a" "libLinearMath.a" "libglfw3.a" "libOpenAL32.dll.a" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)../tram-sdk/libraries 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overridden using an environment variable
##
AR       := C:/mingw64/bin/ar.exe rcu
CXX      := C:/mingw64/bin/g++.exe
CC       := C:/mingw64/bin/gcc.exe
CXXFLAGS :=  -g -O0 -std=c++20 -Wall $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := C:/mingw64/bin/as.exe


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files\CodeLite
WXWIN:=C:\Users\Poga\Desktop\miskaste\wxwidgets
WXCFG:=gcc_dll\mswu
Objects0=$(IntermediateDirectory)/src_pickup.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_tram-sdk.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_moshkis.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_moshkiscomponent.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: MakeIntermediateDirs $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@if not exist "$(IntermediateDirectory)" $(MakeDirCommand) "$(IntermediateDirectory)"
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@if not exist "$(IntermediateDirectory)" $(MakeDirCommand) "$(IntermediateDirectory)"
	@if not exist "$(OutputDirectory)" $(MakeDirCommand) "$(OutputDirectory)"

$(IntermediateDirectory)/.d:
	@if not exist "$(IntermediateDirectory)" $(MakeDirCommand) "$(IntermediateDirectory)"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/src_pickup.cpp$(ObjectSuffix): src/pickup.cpp $(IntermediateDirectory)/src_pickup.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/jam-game/src/pickup.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_pickup.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_pickup.cpp$(DependSuffix): src/pickup.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_pickup.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_pickup.cpp$(DependSuffix) -MM src/pickup.cpp

$(IntermediateDirectory)/src_pickup.cpp$(PreprocessSuffix): src/pickup.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_pickup.cpp$(PreprocessSuffix) src/pickup.cpp

$(IntermediateDirectory)/src_tram-sdk.cpp$(ObjectSuffix): src/tram-sdk.cpp $(IntermediateDirectory)/src_tram-sdk.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/jam-game/src/tram-sdk.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_tram-sdk.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_tram-sdk.cpp$(DependSuffix): src/tram-sdk.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_tram-sdk.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_tram-sdk.cpp$(DependSuffix) -MM src/tram-sdk.cpp

$(IntermediateDirectory)/src_tram-sdk.cpp$(PreprocessSuffix): src/tram-sdk.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_tram-sdk.cpp$(PreprocessSuffix) src/tram-sdk.cpp

$(IntermediateDirectory)/src_moshkis.cpp$(ObjectSuffix): src/moshkis.cpp $(IntermediateDirectory)/src_moshkis.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/jam-game/src/moshkis.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_moshkis.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_moshkis.cpp$(DependSuffix): src/moshkis.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_moshkis.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_moshkis.cpp$(DependSuffix) -MM src/moshkis.cpp

$(IntermediateDirectory)/src_moshkis.cpp$(PreprocessSuffix): src/moshkis.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_moshkis.cpp$(PreprocessSuffix) src/moshkis.cpp

$(IntermediateDirectory)/src_moshkiscomponent.cpp$(ObjectSuffix): src/moshkiscomponent.cpp $(IntermediateDirectory)/src_moshkiscomponent.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/jam-game/src/moshkiscomponent.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_moshkiscomponent.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_moshkiscomponent.cpp$(DependSuffix): src/moshkiscomponent.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_moshkiscomponent.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_moshkiscomponent.cpp$(DependSuffix) -MM src/moshkiscomponent.cpp

$(IntermediateDirectory)/src_moshkiscomponent.cpp$(PreprocessSuffix): src/moshkiscomponent.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_moshkiscomponent.cpp$(PreprocessSuffix) src/moshkiscomponent.cpp

$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix): src/main.cpp $(IntermediateDirectory)/src_main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/jam-game/src/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_main.cpp$(DependSuffix): src/main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_main.cpp$(DependSuffix) -MM src/main.cpp

$(IntermediateDirectory)/src_main.cpp$(PreprocessSuffix): src/main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_main.cpp$(PreprocessSuffix) src/main.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r $(IntermediateDirectory)


