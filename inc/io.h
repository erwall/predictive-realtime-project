#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <comedi.h>
#include <comedilib.h>
#include <math.h>
#include <unistd.h>

/* Defining structs needed for working with the ports*/
typedef struct ComediFile {
  struct ComediFile *next;
  int useCount;
  char *name;
  comedi_t *cf;
} ComediFile;

typedef struct {
  int chan;
  int useCount;
  ComediFile *file;
  int subdev;
  int index;
} ComediPort;

typedef struct {
  ComediPort port;
  int maxdata;
  double min;
  double max;
  double delta;
} AnalogPort;

typedef struct {
  ComediPort port;
} DigitalPort;

typedef struct {
  ComediPort port;
  int min;
} EncoderPort;


static struct {
  int count;
  AnalogPort *port;
} gAin = { -1, 0 };

static struct {
  int count;
  AnalogPort *port;
} gAout = { -1, 0 };

static struct {
  int count;
  DigitalPort *port;
} gDin = { -1, 0 };

static struct {
  int count;
  DigitalPort *port;
} gDout = { -1, 0 };

static struct {
  int count;
  EncoderPort *port;
} gEin = { -1, 0 };

static int OpenFile(ComediFile *f, int chan);

static void CloseFile(ComediFile *f, int chan);

static int Open(ComediPort *p, int chan);

static void Close(ComediPort *p, int chan);

static lsampl_t Maxdata(ComediPort *p);

static comedi_range *GetRange(ComediPort *p, unsigned int range);

static lsampl_t DataRead(ComediPort *p, unsigned int range, unsigned int ref);

static void DataWrite(ComediPort *p, unsigned int range, unsigned int ref, lsampl_t data);

static ComediFile *AddFile(char *name);

double analogIn(int channel);

void analogOut(int channel, double value);

void analogInOpen(int channel);

void analogOutOpen(int channel);

void analogInClose(int channel);

void analogOutClose(int channel);

void init();