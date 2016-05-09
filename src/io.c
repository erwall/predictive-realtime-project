#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <comedi.h>
#include <comedilib.h>

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
static ComediFile *gFile = 0;
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



void init()
{
  FILE *conf;
  char buffer[80];
  char *data;

  conf = fopen("/etc/comedi.conf", "r");
  if (conf == 0) {
    printf("Couldn't find /etc/comedi.conf.\n");
    exit(1);
  } else {
    while ((data = fgets(buffer, 80, conf))) {
      char file[80];
      int chan, subdev, index;
      if (sscanf(data, " ain %d %s %d:%d",
		 &chan, file, &subdev, &index) == 4) {
	if (chan > gAin.count) { gAin.count = chan; }
      } else if (sscanf(data, " aout %d %s %d:%d", 
			&chan, file, &subdev, &index) == 4) {
	if (chan > gAout.count) { gAout.count = chan; }
      } else if (sscanf(data, " din %d %s %d:%d", 
			&chan, file, &subdev, &index) == 4) {
	if (chan > gDin.count) { gDin.count = chan; }
      } else if (sscanf(data, " dout %d %s %d:%d", 
			&chan, file, &subdev, &index) == 4) {
	if (chan > gDout.count) { gDout.count = chan; }
      } else if (sscanf(data, " ein %d %s %d:%d", 
			&chan, file, &subdev, &index) == 4) {
	if (chan > gEin.count) { gEin.count = chan; }
      }
    }
    {
      int i;
      gAin.port = (AnalogPort*) calloc(gAin.count + 1, sizeof(AnalogPort));
      for (i = 0 ; i < gAin.count ; i++) { gAin.port[i].port.chan = i; }
      gAout.port = (AnalogPort*) calloc(gAout.count + 1, sizeof(AnalogPort));
      for (i = 0 ; i < gAout.count ; i++) { gAout.port[i].port.chan = i; }
      gDin.port = (DigitalPort*) calloc(gDin.count + 1, sizeof(DigitalPort));
      for (i = 0 ; i < gDin.count ; i++) { gDin.port[i].port.chan = i; }
      gDout.port = (DigitalPort*) calloc(gDout.count + 1, sizeof(DigitalPort));
      for (i = 0 ; i < gDout.count ; i++) { gDout.port[i].port.chan = i; }
      gEin.port = (EncoderPort*) calloc(gEin.count + 1, sizeof(EncoderPort));
      for (i = 0 ; i < gEin.count ; i++) { gEin.port[i].port.chan = i; }
    }
    fseek(conf, 0, SEEK_SET);
    while ((data = fgets(buffer, 80, conf))) {
      char file[80];
      int chan, subdev, index;
      ComediPort *p = 0;
      
      if (sscanf(data, " ain %d %s %d:%d", 
		 &chan, file, &subdev, &index) == 4) {
	p = &gAin.port[chan].port;
      } else if (sscanf(data, " aout %d %s %d:%d", 
			&chan, file, &subdev, &index) == 4) {
	p = &gAout.port[chan].port;
      } else if (sscanf(data, " din %d %s %d:%d", 
			&chan, file, &subdev, &index) == 4) {
	p = &gDin.port[chan].port;
      } else if (sscanf(data, " dout %d %s %d:%d", 
			&chan, file, &subdev, &index) == 4) {
	p = &gDout.port[chan].port;
      } else if (sscanf(data, " ein %d %s %d:%d", 
			&chan, file, &subdev, &index) == 4) {
	p = &gEin.port[chan].port;
      }
      if (p) {
	p->chan = chan;
	p->file = AddFile(file);
	p->subdev = subdev;
	p->index = index;
      }
    }
    fclose(conf);
  }
}


