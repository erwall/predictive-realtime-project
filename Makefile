CLIBS		= -lpthread -lrt
LIBS		= -L/opt/Qt5.6.0/5.6/gcc_64/lib -lQt5Widgets -lQt5Gui \
	-lQt5Core -lQt5PrintSupport -lGL $(CLIBS)
LFLAGS		= -Wl,-z,origin -Wl,-rpath,\$$ORIGIN \
	-Wl,-rpath,/opt/Qt5.6.0/5.6/gcc_64/lib
DEFINES		= -DQT_QML_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB
CFLAGS		= -g -Wall -Iinc -fPIC
CXXFLAGS	= -pipe -g -std=gnu++0x -Wall -W -D_REENTRANT -fPIC $(DEFINES)
INCPATH		= -Iinc -I/opt/Qt5.6.0/5.6/gcc_64/include \
	-I/opt/Qt5.6.0/5.6/gcc_64/include/QtWidgets \
	-I/opt/Qt5.6.0/5.6/gcc_64/include/QtGui \
	-I/opt/Qt5.6.0/5.6/gcc_64/include/QtCore \

MOC = /opt/Qt5.6.0/5.6/gcc_64/bin/moc

GUIOBJS = ui.o moc_ui.o qcustomplot.o moc_qcustomplot.o

.PHONY : all nogui clean
.PRECIOUS : src/moc_qcustomplot.cc

all : LIBS += -lcomedi -lm
all : src/main.c regulator.o io.o $(GUIOBJS)
	g++ $(CXXFLAGS) $(LFLAGS) $(INCPATH) $^ $(LIBS) -o main	

nohw : CFLAGS += -DNO_HARDWARE
nohw : src/main.c regulator.o $(GUIOBJS)
	g++ $(CXXFLAGS) $(LFLAGS) $(INCPATH) $^ $(LIBS) -o main

nogui : CFLAGS += -DNO_GUI
nogui : CLIBS += -lcomedi -lm
nogui : src/main.c regulator.o io.o
	gcc $(CFLAGS) $^ $(CLIBS) -o main

nohworgui : CFLAGS += -DNO_HARDWARE
nohworgui : CFLAGS += -DNO_GUI
nohworgui : src/main.c regulator.o
	gcc $(CFLAGS) $^ $(CLIBS) -o main

%.o : src/%.c
	gcc -c $(CFLAGS) $< -o $@

%.o : src/%.cc
	g++ -c $(CXXFLAGS) $(INCPATH) $< -o $@

src/moc_%.cc : inc/%.h
	$(MOC) $(DEFINES) $(INCPATH) $< -o $@
	
clean :
	rm *.o
