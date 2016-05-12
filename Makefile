LIBS		= -L/opt/Qt5.6.0/5.6/gcc_64/lib -lQt5Widgets -lQt5Gui \
	-lQt5Core -lQt5PrintSupport -lGL -lpthread
LFLAGS		= -Wl,-z,origin -Wl,-rpath,\$$ORIGIN \
	-Wl,-rpath,/opt/Qt5.6.0/5.6/gcc_64/lib
DEFINES		= -DQT_QML_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB
CFLAGS		= -g -Wall -Iinc
CXXFLAGS	= -pipe -g -std=gnu++0x -Wall -W -D_REENTRANT -fPIC $(DEFINES)
INCPATH		= -Iinc -I/opt/Qt5.6.0/5.6/gcc_64/include \
	-I/opt/Qt5.6.0/5.6/gcc_64/include/QtWidgets \
	-I/opt/Qt5.6.0/5.6/gcc_64/include/QtGui \
	-I/opt/Qt5.6.0/5.6/gcc_64/include/QtCore \

MOC = /opt/Qt5.6.0/5.6/gcc_64/bin/moc

.PHONY : all nogui clean
.PRECIOUS : src/moc_qcustomplot.cc

all : src/main.c regulator.o ui.o moc_ui.o qcustomplot.o moc_qcustomplot.o
	g++ $(CXXFLAGS) $(LFLAGS) $(INCPATH) $^ $(LIBS) -o main	

nohw : CFLAGS += -DNO_HARDWARE
nohw : all

nogui : CFLAGS += -DNO_GUI
nogui : src/main.c regulator.o
	gcc $(CFLAGS) src/main.c regulator.o -lpthread -o main

nohworgui : CFLAGS += -DNO_HARDWARE
nohworgui : nogui

regulator.o : src/regulator.c
	gcc -c $(CFLAGS) src/regulator.c -lpthread -o regulator.o

%.o : src/%.cc
	g++ -c $(CXXFLAGS) $(INCPATH) $< -o $@

src/moc_%.cc : inc/%.h
	$(MOC) $(DEFINES) $(INCPATH) $< -o $@
	
clean :
	rm *.o
