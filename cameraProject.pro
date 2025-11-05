QT       += core gui network serialbus serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    LocationArithmetic.cpp \
    QsLog/QsLog.cpp \
    QsLog/QsLogDest.cpp \
    QsLog/QsLogDestConsole.cpp \
    QsLog/QsLogDestFile.cpp \
    QsLog/QsLogDestFunctor.cpp \
    QtHalconWin.cpp \
    UI/CammerWidget.cpp \
    UI/form.cpp \
    UI/mainwindow.cpp \
    UI/settings.cpp \
    cameracontroller.cpp \
    controller.cpp \
    datamanager.cpp \
    main.cpp \
    modbusclient.cpp \
    serialport.cpp \
    tcpclient.cpp \
    tcpserver.cpp \
    threedcamerawidget.cpp \
    visionalgorithm.cpp


HEADERS += \
    LocationArithmetic.h \
    QtHalconArithmetic.hpp \
    QtHalconWin.h \
    UI/settings.h \
    datamanager.h \
    detectionsystem.h \
    include/CloudProcess.h \
    include/ExportDef.h \
    include/PCL/pcl_1.9/3rdParty/Boost/include/boost-1_68/boost/numeric/conversion/detail/preprocessed/numeric_cast_traits_long_long.hpp \
    include/PoseOperateEigen.h \
    QsLog/QsLog.h \
    QsLog/QsLogDest.h \
    QsLog/QsLogDestConsole.h \
    QsLog/QsLogDestFile.h \
    QsLog/QsLogDestFunctor.h \
    QsLog/QsLogDisableForThisFile.h \
    QsLog/QsLogLevel.h \
    UI/CammerWidget.h \
    UI/form.h \
    UI/mainwindow.h \
    cameracontroller.h \
    controller.h \
    modbusclient.h \
    serialport.h \
    tcpclient.h \
    tcpserver.h \
    threedcamerawidget.h \
    visionalgorithm.h

INCLUDEPATH += opencv2/  \
            opencv2/
INCLUDEPATH += $$PWD/Include/IMV/

FORMS += \
    UI/cammerwidget.ui \
    UI/form.ui \
    UI/mainwindow.ui \
    UI/settings.ui \
    threedcamerawidget.ui

INCLUDEPATH += $$PWD/include
INCLUDEPATH += $$PWD/include/PCL/
INCLUDEPATH += $$PWD/include/PCL/dependent
INCLUDEPATH += $$PWD/include/PCL/dependent/glog/debug
INCLUDEPATH += $$PWD/include/PCL/dependent/glog/release
INCLUDEPATH += $$PWD/include/PCL/pcl_1.9/include/pcl-1.9
INCLUDEPATH += $$PWD/include/PCL/pcl_1.9/3rdParty/Boost/include/boost-1_68
INCLUDEPATH += $$PWD/include/PCL/pcl_1.9/3rdParty/Eigen/eigen3
INCLUDEPATH += $$PWD/include/PCL/pcl_1.9/3rdParty/FLANN/include
INCLUDEPATH += $$PWD/include/PCL/pcl_1.9/3rdParty/VTK/include/vtk-8.1
INCLUDEPATH += $$PWD/include/PCL/pcl_1.9/3rdParty/VTK/include/vtk-8.1/vtklz4
INCLUDEPATH += $$PWD/include/PCL/pcl_1.9/3rdParty/VTK/include/vtk-8.2
INCLUDEPATH += $$PWD/include/PCL/pcl_1.9/3rdParty/VTK/include/vtk-8.2/vtklz4

DEPENDPATH += $$PWD/include
DEPENDPATH += $$PWD/include/PCL/
DEPENDPATH += $$PWD/include/PCL/dependent
DEPENDPATH += $$PWD/include/PCL/dependent/glog/debug
DEPENDPATH += $$PWD/include/PCL/dependent/glog/release
DEPENDPATH += $$PWD/include/PCL/pcl_1.9/include/pcl-1.9
DEPENDPATH += $$PWD/include/PCL/pcl_1.9/3rdParty/Boost/include/boost-1_68
DEPENDPATH += $$PWD/include/PCL/pcl_1.9/3rdParty/Eigen/eigen3
DEPENDPATH += $$PWD/include/PCL/pcl_1.9/3rdParty/FLANN/include
DEPENDPATH += $$PWD/include/PCL/pcl_1.9/3rdParty/VTK/include/vtk-8.1
DEPENDPATH += $$PWD/include/PCL/pcl_1.9/3rdParty/VTK/include/vtk-8.1/vtklz4
DEPENDPATH += $$PWD/include/PCL/pcl_1.9/3rdParty/VTK/include/vtk-8.2
DEPENDPATH += $$PWD/include/PCL/pcl_1.9/3rdParty/VTK/include/vtk-8.2/vtklz4
#Camera
INCLUDEPATH += $$PWD/include/Camera
INCLUDEPATH += $$PWD/include/Camera/3DCamera
INCLUDEPATH += $$PWD/include/Camera/balser
INCLUDEPATH += $$PWD/include/Camera/halcon
INCLUDEPATH += $$PWD/include/Camera/robot_simulation
INCLUDEPATH += $$PWD/include/Camera/huaray
INCLUDEPATH += $$PWD/include/Camera/opencv2
DEPENDPATH += $$PWD/include/Camera
DEPENDPATH += $$PWD/include/Camera/3DCamera
DEPENDPATH += $$PWD/include/Camera/balser
DEPENDPATH += $$PWD/include/Camera/halcon
DEPENDPATH += $$PWD/include/Camera/robot_simulation
DEPENDPATH += $$PWD/include/Camera/huaray
DEPENDPATH += $$PWD/include/Camera/opencv2

LIBS += -L$$PWD/lib
LIBS += -L$$PWD/lib/ -lPoseOperate
LIBS += -L$$PWD/lib/Camera
LIBS += -L$$PWD/lib/PCL
LIBS += -L$$PWD/lib/robot_simulation
LIBS += -L$$PWD/lib/PCL/VLD
LIBS += -L$$PWD/lib/PCL/pcl1.9
LIBS += -L$$PWD/lib/PCL/fast_gicp/release
LIBS += -L$$PWD/lib/PCL/3rdParty/Boost/lib_140
LIBS += -L$$PWD/lib/PCL/3rdParty/Boost/lib
LIBS += -L$$PWD/lib/PCL/3rdParty/FLANN/lib
LIBS += -L$$PWD/lib/PCL/3rdParty/OpenNI2/lib
LIBS += -L$$PWD/lib/PCL/3rdParty/Qhull/lib
LIBS += -L$$PWD/lib/PCL/3rdParty/VTK/lib

LIBS += -L$$PWD/lib/Camera/halcon/ -lhalconcpp
LIBS += -L$$PWD/lib/Camera/opencv/ -lopencv_world451
LIBS += -L$$PWD/lib/Camera/3DCamera/ -lThreeDCamerad
LIBS += -L$$PWD/lib/Camera/HK/ -lMvCameraControl
LIBS += -L$$PWD/lib/Camera/balser/ -lGCBase_MD_VC141_v3_1_Basler_pylon
LIBS += -L$$PWD/lib/Camera/balser/ -lGenApi_MD_VC141_v3_1_Basler_pylon
LIBS += -L$$PWD/lib/Camera/balser/ -lPylonUtility_v6_3
LIBS += -L$$PWD/lib/Camera/balser/ -lPylonGUI_v6_3
LIBS += -L$$PWD/lib/Camera/balser/ -lPylonBase_v6_3
LIBS += -L$$PWD/lib/Camera/huaray\\MVSDKmd
LIBS += -L$$PWD/lib/ -lCloudProcess
LIBS += -L$$PWD/lib/ -lQVTKWidgetPlugin
LIBS += -L$$PWD/lib/ -lvtkalglib-8.1
LIBS += -L$$PWD/lib/ -lvtkChartsCore-8.1
LIBS += -L$$PWD/lib/ -lvtkCommonColor-8.1
LIBS += -L$$PWD/lib/ -lvtkCommonComputationalGeometry-8.1
LIBS += -L$$PWD/lib/ -lvtkCommonCore-8.1
LIBS += -L$$PWD/lib/ -lvtkCommonDataModel-8.1
LIBS += -L$$PWD/lib/ -lvtkCommonExecutionModel-8.1
LIBS += -L$$PWD/lib/ -lvtkCommonMath-8.1
LIBS += -L$$PWD/lib/ -lvtkCommonMisc-8.1
LIBS += -L$$PWD/lib/ -lvtkCommonSystem-8.1
LIBS += -L$$PWD/lib/ -lvtkCommonTransforms-8.1
LIBS += -L$$PWD/lib/ -lvtkDICOMParser-8.1
LIBS += -L$$PWD/lib/ -lvtkDomainsChemistry-8.1
LIBS += -L$$PWD/lib/ -lvtkDomainsChemistryOpenGL2-8.1
LIBS += -L$$PWD/lib/ -lvtkexoIIc-8.1
LIBS += -L$$PWD/lib/ -lvtkexpat-8.1
LIBS += -L$$PWD/lib/ -lvtkFiltersAMR-8.1
LIBS += -L$$PWD/lib/ -lvtkFiltersCore-8.1
LIBS += -L$$PWD/lib/ -lvtkFiltersExtraction-8.1
LIBS += -L$$PWD/lib/ -lvtkFiltersFlowPaths-8.1
LIBS += -L$$PWD/lib/ -lvtkFiltersGeneral-8.1
LIBS += -L$$PWD/lib/ -lvtkFiltersGeneric-8.1
LIBS += -L$$PWD/lib/ -lvtkFiltersGeometry-8.1
LIBS += -L$$PWD/lib/ -lvtkFiltersHybrid-8.1
LIBS += -L$$PWD/lib/ -lvtkFiltersHyperTree-8.1
LIBS += -L$$PWD/lib/ -lvtkFiltersImaging-8.1
LIBS += -L$$PWD/lib/ -lvtkFiltersModeling-8.1
LIBS += -L$$PWD/lib/ -lvtkFiltersParallel-8.1
LIBS += -L$$PWD/lib/ -lvtkFiltersParallelImaging-8.1
LIBS += -L$$PWD/lib/ -lvtkFiltersPoints-8.1
LIBS += -L$$PWD/lib/ -lvtkFiltersProgrammable-8.1
LIBS += -L$$PWD/lib/ -lvtkFiltersSelection-8.1
LIBS += -L$$PWD/lib/ -lvtkFiltersSMP-8.1
LIBS += -L$$PWD/lib/ -lvtkFiltersSources-8.1
LIBS += -L$$PWD/lib/ -lvtkFiltersStatistics-8.1
LIBS += -L$$PWD/lib/ -lvtkFiltersTexture-8.1
LIBS += -L$$PWD/lib/ -lvtkFiltersTopology-8.1
LIBS += -L$$PWD/lib/ -lvtkFiltersVerdict-8.1
LIBS += -L$$PWD/lib/ -lvtkfreetype-8.1
LIBS += -L$$PWD/lib/ -lvtkGeovisCore-8.1
LIBS += -L$$PWD/lib/ -lvtkgl2ps-8.1
LIBS += -L$$PWD/lib/ -lvtkglew-8.1
LIBS += -L$$PWD/lib/ -lvtkGUISupportQt-8.1
LIBS += -L$$PWD/lib/ -lvtkGUISupportQtSQL-8.1
LIBS += -L$$PWD/lib/ -lvtkhdf5-8.1
LIBS += -L$$PWD/lib/ -lvtkhdf5_hl-8.1
LIBS += -L$$PWD/lib/ -lvtkImagingColor-8.1
LIBS += -L$$PWD/lib/ -lvtkImagingCore-8.1
LIBS += -L$$PWD/lib/ -lvtkImagingFourier-8.1
LIBS += -L$$PWD/lib/ -lvtkImagingGeneral-8.1
LIBS += -L$$PWD/lib/ -lvtkImagingHybrid-8.1
LIBS += -L$$PWD/lib/ -lvtkImagingMath-8.1
LIBS += -L$$PWD/lib/ -lvtkImagingMorphological-8.1
LIBS += -L$$PWD/lib/ -lvtkImagingSources-8.1
LIBS += -L$$PWD/lib/ -lvtkImagingStatistics-8.1
LIBS += -L$$PWD/lib/ -lvtkImagingStencil-8.1
LIBS += -L$$PWD/lib/ -lvtkInfovisCore-8.1
LIBS += -L$$PWD/lib/ -lvtkInfovisLayout-8.1
LIBS += -L$$PWD/lib/ -lvtkInteractionImage-8.1
LIBS += -L$$PWD/lib/ -lvtkInteractionStyle-8.1
LIBS += -L$$PWD/lib/ -lvtkInteractionWidgets-8.1
LIBS += -L$$PWD/lib/ -lvtkIOAMR-8.1
LIBS += -L$$PWD/lib/ -lvtkIOCore-8.1
LIBS += -L$$PWD/lib/ -lvtkIOEnSight-8.1
LIBS += -L$$PWD/lib/ -lvtkIOExodus-8.1
LIBS += -L$$PWD/lib/ -lvtkIOExport-8.1
LIBS += -L$$PWD/lib/ -lvtkIOExportOpenGL2-8.1
LIBS += -L$$PWD/lib/ -lvtkIOGeometry-8.1
LIBS += -L$$PWD/lib/ -lvtkIOImage-8.1
LIBS += -L$$PWD/lib/ -lvtkIOImport-8.1
LIBS += -L$$PWD/lib/ -lvtkIOInfovis-8.1
LIBS += -L$$PWD/lib/ -lvtkIOLegacy-8.1
LIBS += -L$$PWD/lib/ -lvtkIOLSDyna-8.1
LIBS += -L$$PWD/lib/ -lvtkIOMINC-8.1
LIBS += -L$$PWD/lib/ -lvtkIOMovie-8.1
LIBS += -L$$PWD/lib/ -lvtkIONetCDF-8.1
LIBS += -L$$PWD/lib/ -lvtkIOParallel-8.1
LIBS += -L$$PWD/lib/ -lvtkIOParallelXML-8.1
LIBS += -L$$PWD/lib/ -lvtkIOPLY-8.1
LIBS += -L$$PWD/lib/ -lvtkIOSQL-8.1
LIBS += -L$$PWD/lib/ -lvtkIOTecplotTable-8.1
LIBS += -L$$PWD/lib/ -lvtkIOVideo-8.1
LIBS += -L$$PWD/lib/ -lvtkIOXML-8.1
LIBS += -L$$PWD/lib/ -lvtkIOXMLParser-8.1
LIBS += -L$$PWD/lib/ -lvtkjpeg-8.1
LIBS += -L$$PWD/lib/ -lvtkjsoncpp-8.1
LIBS += -L$$PWD/lib/ -lvtklibharu-8.1
LIBS += -L$$PWD/lib/ -lvtklibxml2-8.1
LIBS += -L$$PWD/lib/ -lvtklz4-8.1
LIBS += -L$$PWD/lib/ -lvtkmetaio-8.1
LIBS += -L$$PWD/lib/ -lvtkNetCDF-8.1
LIBS += -L$$PWD/lib/ -lvtknetcdfcpp-8.1
LIBS += -L$$PWD/lib/ -lvtkoggtheora-8.1
LIBS += -L$$PWD/lib/ -lvtkParallelCore-8.1
LIBS += -L$$PWD/lib/ -lvtkpng-8.1
LIBS += -L$$PWD/lib/ -lvtkproj4-8.1
LIBS += -L$$PWD/lib/ -lvtkRenderingAnnotation-8.1
LIBS += -L$$PWD/lib/ -lvtkRenderingContext2D-8.1
LIBS += -L$$PWD/lib/ -lvtkRenderingContextOpenGL2-8.1
LIBS += -L$$PWD/lib/ -lvtkRenderingCore-8.1
LIBS += -L$$PWD/lib/ -lvtkRenderingFreeType-8.1
LIBS += -L$$PWD/lib/ -lvtkRenderingGL2PSOpenGL2-8.1
LIBS += -L$$PWD/lib/ -lvtkRenderingImage-8.1
LIBS += -L$$PWD/lib/ -lvtkRenderingLabel-8.1
LIBS += -L$$PWD/lib/ -lvtkRenderingLOD-8.1
LIBS += -L$$PWD/lib/ -lvtkRenderingOpenGL2-8.1
LIBS += -L$$PWD/lib/ -lvtkRenderingQt-8.1
LIBS += -L$$PWD/lib/ -lvtkRenderingVolume-8.1
LIBS += -L$$PWD/lib/ -lvtkRenderingVolumeOpenGL2-8.1
LIBS += -L$$PWD/lib/ -lvtksqlite-8.1
LIBS += -L$$PWD/lib/ -lvtksys-8.1
LIBS += -L$$PWD/lib/ -lvtkTestingGenericBridge-8.1
LIBS += -L$$PWD/lib/ -lvtkTestingIOSQL-8.1
LIBS += -L$$PWD/lib/ -lvtkTestingRendering-8.1
LIBS += -L$$PWD/lib/ -lvtktiff-8.1
LIBS += -L$$PWD/lib/ -lvtkverdict-8.1
LIBS += -L$$PWD/lib/ -lvtkViewsContext2D-8.1
LIBS += -L$$PWD/lib/ -lvtkViewsCore-8.1
LIBS += -L$$PWD/lib/ -lvtkViewsInfovis-8.1
LIBS += -L$$PWD/lib/ -lvtkViewsQt-8.1
LIBS += -L$$PWD/lib/ -lvtkzlib-8.1

#PCL环境配置-lib
LIBS+=-L$$PWD/lib/PCL\
        -lbz2\
        -lceres\
        -ldouble-conversion\
        -lexpat\
        -lflann\
        -lflann_cpp\
        -lfreeglut\
        -lfast_gicp\
        -lgflags\
        -lglew32\
        -lglog\
        -lGlU32\
        -lharfbuzz\
        -lhdf5\
        -lhdf5_hl\
        -licudt\
        -licuin\
        -licuio\
        -licutu\
        -licuuc\
        -ljasper\
        -ljpeg\
        -ljsoncpp\
        -llibcharset\
        -llibcrypto\
        -llibcurl\
        -llibecpg\
        -llibecpg_compat\
        -llibhpdf\
        -llibiconv\
        -llibpgcommon\
        -llibpgport\
        -llibpgtypes\
        -llibpng16\
        -llibpq\
        -llibssl\
        -llibszip\
        -llibwebpmux\
        -llibxml2\
        -llz4\
        -llzma\
        -lnetcdf\
        -logg\
        -lopenblas\
        -lOpenGL32\
        -lpcl_common_release\
        -lpcl_features_release\
        -lpcl_filters_release\
        -lpcl_io_ply_release\
        -lpcl_io_release\
        -lpcl_kdtree_release\
        -lpcl_keypoints_release\
        -lpcl_ml_release\
        -lpcl_octree_release\
        -lpcl_outofcore_release\
        -lpcl_people_release\
        -lpcl_recognition_release\
        -lpcl_registration_release\
        -lpcl_sample_consensus_release\
        -lpcl_search_release\
        -lpcl_segmentation_release\
        -lpcl_stereo_release\
        -lpcl_surface_release\
        -lpcl_tracking_release\
        -lpcl_visualization_release\
        -lpcre2-16\
        -lpcre2-32\
        -lpcre2-8\
        -lproj\
        -lpugixml\
        -lqhull\
        -lqhull_p\
        -lqhull_r\
        -lsqlite3\
        -lszip\
        -ltheora\
        -ltheoradec\
        -ltheoraenc\
        -ltiff\
        -ltiffxx\
        -lturbojpeg\
        -lwebp\
        -lwebpdecoder\
        -lwebpdemux\
        -lxxhash\
        -lzlib\
        -lzstd\
#INCLUDEPATH += QsLog/QsLog.pri

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc


INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

LIBS += -L$$PWD/lib/ -lMVSDKmd
INCLUDEPATH += $$PWD/Include
DEPENDPATH += $$PWD/Include

LIBS += -L$$PWD/lib/ -lThreeDCameraApi
LIBS += -L$$PWD/lib/ -lopencv_world451


#RC_ICONS = jqr.ico


