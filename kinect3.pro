TEMPLATE	= 	app
TARGET		= 	MagicForest
LANGUAGE	= 	C++
CONFIG		+= 	qt thread warn_on console debug_and_release
QT		+= 	opengl

CONFIG(release, debug|release) {
    CONFIG -= debug release
    CONFIG += release
}

HEADERS		+=	headers/ui/MainWindow.h \
                        headers/ui/MainMenu.h \
                        headers/ui/Game.h \
                        headers/ui/Scores.h \
                        headers/ui/Options.h \
                        headers/ui/Info.h \
                        headers/ui/PopUpMenu.h \
                        headers/ui/PopUpEndGame.h \
                        headers/ui/PopUpInit.h \
                        headers/kinect/Kinect.h \
                        headers/kinect/Cylinder.h \
                        headers/render/Scene.h \
                        headers/render/SceneGUI.h \
                        headers/render/ShaderManager.h \
                        headers/render/Geometry.h \
                        headers/render/obj/Object.h \
                        headers/render/obj/Box.h \
                        headers/render/obj/Material_lib.h \
                        headers/logic/GameInfo.h \
                        headers/logic/Attack.h \
                        headers/logic/Combo.h \
                        headers/logic/Element.h \
                        headers/logic/Character.h \
                        headers/logic/Player.h \
                        headers/logic/NPC.h \
                        headers/logic/Obstacle.h \
                        headers/logic/Reward.h \
                        headers/logic/Physics.h \
                        headers/data/ScoreManager.h \


SOURCES		+=	src/main.cpp \
                        src/ui/MainWindow.cpp \
                        src/ui/MainMenu.cpp \
                        src/ui/Game.cpp \
                        src/ui/Scores.cpp \
                        src/ui/Options.cpp \
                        src/ui/Info.cpp \
                        src/ui/PopUpMenu.cpp \
                        src/ui/PopUpEndGame.cpp \
                        src/ui/PopUpInit.cpp \
                        src/kinect/Kinect.cpp \
                        src/kinect/Cylinder.cpp \
                        src/render/Scene.cpp \
                        src/render/SceneGUI.cpp \
                        src/render/Geometry.cpp \
                        src/render/obj/Object.cpp \
                        src/render/obj/Box.cpp \
                        src/render/obj/Material_lib.cpp \
                        src/logic/GameInfo.cpp \
                        src/logic/Attack.cpp \
                        src/logic/Combo.cpp \
                        src/logic/Player.cpp \
                        src/logic/NPC.cpp \
                        src/logic/Obstacle.cpp \
                        src/logic/Reward.cpp \
                        src/logic/Physics.cpp \
			
FORMS		+=	forms/MainWindow.ui \
                        forms/MainMenu.ui \
                        forms/Game.ui \
                        forms/Options.ui \
                        forms/Scores.ui \
                        forms/Info.ui \
                        forms/PopUpMenu.ui \
                        forms/PopUpEndGame.ui \
                        forms/PopUpInit.ui \

RESOURCES 	+=	resources/resources.qrc \

INCLUDEPATH	+=      libs/bullet/src \
                        libs/openni/include \


				
LIBS		+= 	-lGLEW -lglut -lGLU -lGL -lSDL -lSDLmain -lSDL_mixer -lOpenNI \
 #                       -Llibs/openni/lib -lOpenNI \
                        -Llibs/bullet/lib -lBulletCollision \
                        -Llibs/bullet/lib -lBulletDynamics \
                        -Llibs/bullet/lib -lLinearMath \

