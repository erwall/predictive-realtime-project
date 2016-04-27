LIBS		= -L/opt/Qt5.6.0/5.6/gcc_64/lib -lQt5Widgets -lQt5Gui \
	-lQt5Core -lGL -lpthread
LFLAGS		= -Wl,-z,origin -Wl,-rpath,\$$ORIGIN \
	-Wl,-rpath,/opt/Qt5.6.0/5.6/gcc_64/lib
DEFINES		= -DQT_QML_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB
CFLAGS		= -g -Wall -DNO_GUI -Iinc
CXXFLAGS	= -pipe -g -std=gnu++0x -Wall -W -D_REENTRANT -fPIC $(DEFINES)
INCPATH		= -Iinc -I/opt/Qt5.6.0/5.6/gcc_64/include \
	-I/opt/Qt5.6.0/5.6/gcc_64/include/QtWidgets \
	-I/opt/Qt5.6.0/5.6/gcc_64/include/QtGui \
	-I/opt/Qt5.6.0/5.6/gcc_64/include/QtCore \

MOC = /opt/Qt5.6.0/5.6/gcc_64/bin/moc

.PHONY : all nogui clean

all : src/main.c regulator.o ui.o moc_ui.o
	g++ $(CXXFLAGS) $(LFLAGS) $(INCPATH) src/main.c regulator.o ui.o moc_ui.o \
	$(LIBS) -o main
	
nogui : src/main.c regulator.o
	gcc $(CFLAGS) src/main.c regulator.o -lpthread -o main

regulator.o : src/regulator.c
	gcc -c $(CFLAGS) src/regulator.c -lpthread -o regulator.o

ui.o : src/ui.cc
	g++ -c $(CXXFLAGS) $(INCPATH) src/ui.cc -o ui.o

moc_ui.o : src/moc_ui.cc
	g++ -c $(CXXFLAGS) $(INCPATH) src/moc_ui.cc -o moc_ui.o

src/moc_ui.cc : inc/ui.h
	$(MOC) $(DEFINES) $(INCPATH) inc/ui.h -o src/moc_ui.cc
	
clean :
	rm *.o
