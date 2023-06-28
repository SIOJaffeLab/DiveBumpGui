##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=PCAM
ConfigurationName      :=Release
WorkspacePath          := "/home/nvidia/Software/BUMP_divergui"
ProjectPath            := "/home/nvidia/Software/BUMP_divergui/PCAM"
IntermediateDirectory  :=./obj
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=
Date                   :=28/06/23
CodeLitePath           :="/home/nvidia/.codelite"
LinkerName             :=/usr/bin/g++
SharedObjectLinkerName :=/usr/bin/g++ -shared -fPIC
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
OutputFile             :=./bin/Release/$(ProjectName)
Preprocessors          :=$(PreprocessorSwitch)NDEBUG 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="PCAM.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)include $(IncludeSwitch)include/mvIMPACT_Aquire $(IncludeSwitch)/usr/local/include 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)pthread-2.23 $(LibrarySwitch)PocoFoundation $(LibrarySwitch)PocoUtil $(LibrarySwitch)PocoXML $(LibrarySwitch)opencv_core $(LibrarySwitch)opencv_highgui $(LibrarySwitch)opencv_imgproc $(LibrarySwitch)opencv_imgcodecs $(LibrarySwitch)mvDeviceManager $(LibrarySwitch)mvGenTLProducer $(LibrarySwitch)mvVirtualDevice $(LibrarySwitch)mvGenTLConsumer $(LibrarySwitch)mvPropHandling 
ArLibs                 :=  "libpthread-2.23.so" "libPocoFoundation.so" "libPocoUtil.so" "libPocoXML.so" "libopencv_core.so" "libopencv_highgui.so" "libopencv_imgproc.so" "libopencv_imgcodecs.so" "libmvDeviceManager.so" "libmvGenTLProducer.so" "libmvVirtualDevice.so" "libmvGenTLConsumer.so" "libmvPropHandling.so" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)/lib/ $(LibraryPathSwitch)/usr/local/lib/ $(LibraryPathSwitch)/lib/aarch64-linux-gnu/ $(LibraryPathSwitch)/opt/mvIMPACT_Acquire/lib/arm64/ $(LibraryPathSwitch)/usr/lib/ $(LibraryPathSwitch)/usr/local/lib/ $(LibraryPathSwitch)/lib/x86_64-linux-gnu $(LibraryPathSwitch)/lib/aarch64-linux-gnu/ $(LibraryPathSwitch)/home/nvidia/Software/mvIMPACT_Acquire/lib/arm64/ 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS := -std=c++11 -O2 -Wall $(Preprocessors)
CFLAGS   :=  -O2 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/src_CameraImage.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_DataEntry.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_DataProcessor.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Image.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_kbhit.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_ofThread.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_RawFileWriter.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_RoiImageProcessor.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_serialib.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_MVIMPACTCamera.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Camera.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Log.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_SimCamera.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_PCAM.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_SerialDevice.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_RawCameraFileWriter.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_CameraGUI.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_BUMPControl.cpp$(ObjectSuffix) $(IntermediateDirectory)/common_PGMFileFunctions.cpp$(ObjectSuffix) \
	



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@test -d ./obj || $(MakeDirCommand) ./obj


$(IntermediateDirectory)/.d:
	@test -d ./obj || $(MakeDirCommand) ./obj

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/src_CameraImage.cpp$(ObjectSuffix): src/CameraImage.cpp $(IntermediateDirectory)/src_CameraImage.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nvidia/Software/BUMP_divergui/PCAM/src/CameraImage.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_CameraImage.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_CameraImage.cpp$(DependSuffix): src/CameraImage.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_CameraImage.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_CameraImage.cpp$(DependSuffix) -MM "src/CameraImage.cpp"

$(IntermediateDirectory)/src_CameraImage.cpp$(PreprocessSuffix): src/CameraImage.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_CameraImage.cpp$(PreprocessSuffix) "src/CameraImage.cpp"

$(IntermediateDirectory)/src_DataEntry.cpp$(ObjectSuffix): src/DataEntry.cpp $(IntermediateDirectory)/src_DataEntry.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nvidia/Software/BUMP_divergui/PCAM/src/DataEntry.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_DataEntry.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_DataEntry.cpp$(DependSuffix): src/DataEntry.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_DataEntry.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_DataEntry.cpp$(DependSuffix) -MM "src/DataEntry.cpp"

$(IntermediateDirectory)/src_DataEntry.cpp$(PreprocessSuffix): src/DataEntry.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_DataEntry.cpp$(PreprocessSuffix) "src/DataEntry.cpp"

$(IntermediateDirectory)/src_DataProcessor.cpp$(ObjectSuffix): src/DataProcessor.cpp $(IntermediateDirectory)/src_DataProcessor.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nvidia/Software/BUMP_divergui/PCAM/src/DataProcessor.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_DataProcessor.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_DataProcessor.cpp$(DependSuffix): src/DataProcessor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_DataProcessor.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_DataProcessor.cpp$(DependSuffix) -MM "src/DataProcessor.cpp"

$(IntermediateDirectory)/src_DataProcessor.cpp$(PreprocessSuffix): src/DataProcessor.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_DataProcessor.cpp$(PreprocessSuffix) "src/DataProcessor.cpp"

$(IntermediateDirectory)/src_Image.cpp$(ObjectSuffix): src/Image.cpp $(IntermediateDirectory)/src_Image.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nvidia/Software/BUMP_divergui/PCAM/src/Image.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Image.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Image.cpp$(DependSuffix): src/Image.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Image.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Image.cpp$(DependSuffix) -MM "src/Image.cpp"

$(IntermediateDirectory)/src_Image.cpp$(PreprocessSuffix): src/Image.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Image.cpp$(PreprocessSuffix) "src/Image.cpp"

$(IntermediateDirectory)/src_kbhit.cpp$(ObjectSuffix): src/kbhit.cpp $(IntermediateDirectory)/src_kbhit.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nvidia/Software/BUMP_divergui/PCAM/src/kbhit.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_kbhit.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_kbhit.cpp$(DependSuffix): src/kbhit.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_kbhit.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_kbhit.cpp$(DependSuffix) -MM "src/kbhit.cpp"

$(IntermediateDirectory)/src_kbhit.cpp$(PreprocessSuffix): src/kbhit.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_kbhit.cpp$(PreprocessSuffix) "src/kbhit.cpp"

$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix): src/main.cpp $(IntermediateDirectory)/src_main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nvidia/Software/BUMP_divergui/PCAM/src/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_main.cpp$(DependSuffix): src/main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_main.cpp$(DependSuffix) -MM "src/main.cpp"

$(IntermediateDirectory)/src_main.cpp$(PreprocessSuffix): src/main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_main.cpp$(PreprocessSuffix) "src/main.cpp"

$(IntermediateDirectory)/src_ofThread.cpp$(ObjectSuffix): src/ofThread.cpp $(IntermediateDirectory)/src_ofThread.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nvidia/Software/BUMP_divergui/PCAM/src/ofThread.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_ofThread.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_ofThread.cpp$(DependSuffix): src/ofThread.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_ofThread.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_ofThread.cpp$(DependSuffix) -MM "src/ofThread.cpp"

$(IntermediateDirectory)/src_ofThread.cpp$(PreprocessSuffix): src/ofThread.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_ofThread.cpp$(PreprocessSuffix) "src/ofThread.cpp"

$(IntermediateDirectory)/src_RawFileWriter.cpp$(ObjectSuffix): src/RawFileWriter.cpp $(IntermediateDirectory)/src_RawFileWriter.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nvidia/Software/BUMP_divergui/PCAM/src/RawFileWriter.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_RawFileWriter.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_RawFileWriter.cpp$(DependSuffix): src/RawFileWriter.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_RawFileWriter.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_RawFileWriter.cpp$(DependSuffix) -MM "src/RawFileWriter.cpp"

$(IntermediateDirectory)/src_RawFileWriter.cpp$(PreprocessSuffix): src/RawFileWriter.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_RawFileWriter.cpp$(PreprocessSuffix) "src/RawFileWriter.cpp"

$(IntermediateDirectory)/src_RoiImageProcessor.cpp$(ObjectSuffix): src/RoiImageProcessor.cpp $(IntermediateDirectory)/src_RoiImageProcessor.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nvidia/Software/BUMP_divergui/PCAM/src/RoiImageProcessor.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_RoiImageProcessor.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_RoiImageProcessor.cpp$(DependSuffix): src/RoiImageProcessor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_RoiImageProcessor.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_RoiImageProcessor.cpp$(DependSuffix) -MM "src/RoiImageProcessor.cpp"

$(IntermediateDirectory)/src_RoiImageProcessor.cpp$(PreprocessSuffix): src/RoiImageProcessor.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_RoiImageProcessor.cpp$(PreprocessSuffix) "src/RoiImageProcessor.cpp"

$(IntermediateDirectory)/src_serialib.cpp$(ObjectSuffix): src/serialib.cpp $(IntermediateDirectory)/src_serialib.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nvidia/Software/BUMP_divergui/PCAM/src/serialib.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_serialib.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_serialib.cpp$(DependSuffix): src/serialib.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_serialib.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_serialib.cpp$(DependSuffix) -MM "src/serialib.cpp"

$(IntermediateDirectory)/src_serialib.cpp$(PreprocessSuffix): src/serialib.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_serialib.cpp$(PreprocessSuffix) "src/serialib.cpp"

$(IntermediateDirectory)/src_MVIMPACTCamera.cpp$(ObjectSuffix): src/MVIMPACTCamera.cpp $(IntermediateDirectory)/src_MVIMPACTCamera.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nvidia/Software/BUMP_divergui/PCAM/src/MVIMPACTCamera.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_MVIMPACTCamera.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_MVIMPACTCamera.cpp$(DependSuffix): src/MVIMPACTCamera.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_MVIMPACTCamera.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_MVIMPACTCamera.cpp$(DependSuffix) -MM "src/MVIMPACTCamera.cpp"

$(IntermediateDirectory)/src_MVIMPACTCamera.cpp$(PreprocessSuffix): src/MVIMPACTCamera.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_MVIMPACTCamera.cpp$(PreprocessSuffix) "src/MVIMPACTCamera.cpp"

$(IntermediateDirectory)/src_Camera.cpp$(ObjectSuffix): src/Camera.cpp $(IntermediateDirectory)/src_Camera.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nvidia/Software/BUMP_divergui/PCAM/src/Camera.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Camera.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Camera.cpp$(DependSuffix): src/Camera.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Camera.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Camera.cpp$(DependSuffix) -MM "src/Camera.cpp"

$(IntermediateDirectory)/src_Camera.cpp$(PreprocessSuffix): src/Camera.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Camera.cpp$(PreprocessSuffix) "src/Camera.cpp"

$(IntermediateDirectory)/src_Log.cpp$(ObjectSuffix): src/Log.cpp $(IntermediateDirectory)/src_Log.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nvidia/Software/BUMP_divergui/PCAM/src/Log.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Log.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Log.cpp$(DependSuffix): src/Log.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Log.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Log.cpp$(DependSuffix) -MM "src/Log.cpp"

$(IntermediateDirectory)/src_Log.cpp$(PreprocessSuffix): src/Log.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Log.cpp$(PreprocessSuffix) "src/Log.cpp"

$(IntermediateDirectory)/src_SimCamera.cpp$(ObjectSuffix): src/SimCamera.cpp $(IntermediateDirectory)/src_SimCamera.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nvidia/Software/BUMP_divergui/PCAM/src/SimCamera.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_SimCamera.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_SimCamera.cpp$(DependSuffix): src/SimCamera.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_SimCamera.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_SimCamera.cpp$(DependSuffix) -MM "src/SimCamera.cpp"

$(IntermediateDirectory)/src_SimCamera.cpp$(PreprocessSuffix): src/SimCamera.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_SimCamera.cpp$(PreprocessSuffix) "src/SimCamera.cpp"

$(IntermediateDirectory)/src_PCAM.cpp$(ObjectSuffix): src/PCAM.cpp $(IntermediateDirectory)/src_PCAM.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nvidia/Software/BUMP_divergui/PCAM/src/PCAM.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_PCAM.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_PCAM.cpp$(DependSuffix): src/PCAM.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_PCAM.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_PCAM.cpp$(DependSuffix) -MM "src/PCAM.cpp"

$(IntermediateDirectory)/src_PCAM.cpp$(PreprocessSuffix): src/PCAM.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_PCAM.cpp$(PreprocessSuffix) "src/PCAM.cpp"

$(IntermediateDirectory)/src_SerialDevice.cpp$(ObjectSuffix): src/SerialDevice.cpp $(IntermediateDirectory)/src_SerialDevice.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nvidia/Software/BUMP_divergui/PCAM/src/SerialDevice.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_SerialDevice.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_SerialDevice.cpp$(DependSuffix): src/SerialDevice.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_SerialDevice.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_SerialDevice.cpp$(DependSuffix) -MM "src/SerialDevice.cpp"

$(IntermediateDirectory)/src_SerialDevice.cpp$(PreprocessSuffix): src/SerialDevice.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_SerialDevice.cpp$(PreprocessSuffix) "src/SerialDevice.cpp"

$(IntermediateDirectory)/src_RawCameraFileWriter.cpp$(ObjectSuffix): src/RawCameraFileWriter.cpp $(IntermediateDirectory)/src_RawCameraFileWriter.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nvidia/Software/BUMP_divergui/PCAM/src/RawCameraFileWriter.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_RawCameraFileWriter.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_RawCameraFileWriter.cpp$(DependSuffix): src/RawCameraFileWriter.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_RawCameraFileWriter.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_RawCameraFileWriter.cpp$(DependSuffix) -MM "src/RawCameraFileWriter.cpp"

$(IntermediateDirectory)/src_RawCameraFileWriter.cpp$(PreprocessSuffix): src/RawCameraFileWriter.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_RawCameraFileWriter.cpp$(PreprocessSuffix) "src/RawCameraFileWriter.cpp"

$(IntermediateDirectory)/src_CameraGUI.cpp$(ObjectSuffix): src/CameraGUI.cpp $(IntermediateDirectory)/src_CameraGUI.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nvidia/Software/BUMP_divergui/PCAM/src/CameraGUI.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_CameraGUI.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_CameraGUI.cpp$(DependSuffix): src/CameraGUI.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_CameraGUI.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_CameraGUI.cpp$(DependSuffix) -MM "src/CameraGUI.cpp"

$(IntermediateDirectory)/src_CameraGUI.cpp$(PreprocessSuffix): src/CameraGUI.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_CameraGUI.cpp$(PreprocessSuffix) "src/CameraGUI.cpp"

$(IntermediateDirectory)/src_BUMPControl.cpp$(ObjectSuffix): src/BUMPControl.cpp $(IntermediateDirectory)/src_BUMPControl.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nvidia/Software/BUMP_divergui/PCAM/src/BUMPControl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_BUMPControl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_BUMPControl.cpp$(DependSuffix): src/BUMPControl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_BUMPControl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_BUMPControl.cpp$(DependSuffix) -MM "src/BUMPControl.cpp"

$(IntermediateDirectory)/src_BUMPControl.cpp$(PreprocessSuffix): src/BUMPControl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_BUMPControl.cpp$(PreprocessSuffix) "src/BUMPControl.cpp"

$(IntermediateDirectory)/common_PGMFileFunctions.cpp$(ObjectSuffix): include/mvIMPACT_Aquire/common/PGMFileFunctions.cpp $(IntermediateDirectory)/common_PGMFileFunctions.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nvidia/Software/BUMP_divergui/PCAM/include/mvIMPACT_Aquire/common/PGMFileFunctions.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/common_PGMFileFunctions.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/common_PGMFileFunctions.cpp$(DependSuffix): include/mvIMPACT_Aquire/common/PGMFileFunctions.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/common_PGMFileFunctions.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/common_PGMFileFunctions.cpp$(DependSuffix) -MM "include/mvIMPACT_Aquire/common/PGMFileFunctions.cpp"

$(IntermediateDirectory)/common_PGMFileFunctions.cpp$(PreprocessSuffix): include/mvIMPACT_Aquire/common/PGMFileFunctions.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/common_PGMFileFunctions.cpp$(PreprocessSuffix) "include/mvIMPACT_Aquire/common/PGMFileFunctions.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./obj/


