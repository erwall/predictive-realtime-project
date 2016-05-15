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

#include "io.h"


static ComediFile *gFile = 0;


/*Help method for opening file*/
static int OpenFile(ComediFile *f, int chan) 
{
  int result = 0;

  if (!f || !f->name) {
    printf("Comedi device doesn't exist.\n");
    exit(1);
  } else {
    if (!f->useCount) {
      f->cf = comedi_open(f->name); 
    }
    if (f->cf) {
      f->useCount++;
      result = f->useCount;
    } else {
      printf("Comedi device doesn't exist.\n");
      exit(1);
    }
  }
  return result;
}
/*Help method for closing file*/
static void CloseFile(ComediFile *f, int chan) 
{
  if (!f || !f->name) {
    printf("Comedi device doesn't exist.\n");
    exit(1);
  } else {
    if (f->useCount <= 0 || !f->cf) {
      printf("Comedi device doesn't exist.\n");
      exit(1);
    } else {
      f->useCount--;
      if (f->useCount == 0) {
	comedi_close(f->cf);
	f->cf = 0;
      }
    }
  }
}
/*Help method for opening port*/
static int Open(ComediPort *p, int chan) 
{
  int result = 0;

  if (!p) {
    printf("Comedi device doesn't exist.\n");
    exit(1);
  } else if (OpenFile(p->file, chan)) {
    p->useCount++;
    result = p->useCount;
  }
  return result;
}
/*Help method for closing port*/
static void Close(ComediPort *p, int chan) 
{
  if (!p) {
    printf("Comedi device doesn't exist.\n");
    exit(1);
  } else if (p->useCount <= 0) {
    printf("Comedi device doesn't exist.\n");
    exit(1);
  } else {
    p->useCount--;
    CloseFile(p->file, chan);
  }
}
/*Help method*/
static lsampl_t Maxdata(ComediPort *p) {
  return comedi_get_maxdata(p->file->cf, p->subdev, p->index);
}
/*Help method*/
static comedi_range *GetRange(ComediPort *p, unsigned int range) {
  return comedi_get_range(p->file->cf, p->subdev, p->index, range);
}
/*Help method for AnalogIn*/
static lsampl_t DataRead(ComediPort *p, unsigned int range, unsigned int ref) 
{
  lsampl_t result = 0;

  if (!p->useCount) {
    printf("Something went wrong when reading data.\n");
    exit(1);
  } else if (comedi_data_read(p->file->cf, p->subdev, p->index, range, ref, &result) < 0) {
    printf("Something went wrong when reading data.\n");
    exit(1);
  }
  return result;
}
/*Help method for AnalogOut*/
static void DataWrite(ComediPort *p, unsigned int range, unsigned int ref, lsampl_t data) 
{
  if (!p->useCount) {
    printf("Something went wrong when reading data.\n");
    exit(1);
  } else if (comedi_data_write(p->file->cf, p->subdev, p->index, 
			range, ref, data) < 0) {
    printf("Something went wrong when reading data.\n");
    exit(1);
  }
}
/*Help method for Init*/
static ComediFile *AddFile(char *name) 
{
  ComediFile *result = gFile;
  while (result != 0) {
    if (strcmp(result->name, name) == 0) { break; }
    result = result->next;
  }
  if (result == 0) {
    result = (ComediFile*) malloc(sizeof(ComediFile));
    result->next = gFile;
    result->useCount = 0;
    result->name = strdup(name);
    result->cf = 0;
    gFile = result;
  }
  return result;
}
/*Attempts to read data from an analogIn port. Call this method when reading data*/
double analogIn(int channel) 
{
  double result = 0.0;

  if (channel < 0 || channel > gAin.count ||
    gAin.port[channel].port.file == 0) {
    printf("Something went wrong when reading from analogin port.\n");
    exit(1);
  } else {
    AnalogPort *p = &gAin.port[channel];
    result = p->min + p->delta * DataRead(&p->port, 0, 0);
  }
  return result;
}
/*Attempts to write value to an analogOut port. Call this method when writing data*/
void analogOut(int channel, double value) 
{
  if (channel < 0 || channel > gAout.count ||
    gAout.port[channel].port.file == 0) {
    printf("Someing went wrong when reading from analogOut port.\n");
    exit(1);
  } else {
    AnalogPort *p = &gAout.port[channel];
    int intvalue;
    if (value < p->min) {
      intvalue = 0;
    } else if (value > p->max) {
      intvalue = p->maxdata;
    } else {
      intvalue = (value - p->min) / p->delta;
    }
    if (intvalue < 0) { 
      intvalue = 0; 
    } else if (intvalue > p->maxdata) {
      intvalue = p->maxdata;
    }
    DataWrite(&p->port, 0, 0, intvalue);
  }
}
/*Attempts to open an analogIn port.*/
void analogInOpen(int channel)
{
  if (channel >= 0 && channel <= gAin.count) {
    AnalogPort *p;

    p = &gAin.port[channel];
    if (Open(&p->port, channel) == 1) {
      // Get range on first open of channel
      comedi_range *range;

      range = GetRange(&p->port, 0);
      p->maxdata = Maxdata(&p->port);
      if (range) {
	p->min = range->min;
	p->max = range->max;
	p->delta = (p->max - p->min) / p->maxdata;
      } else {
	Close(&p->port, channel);
	printf("Something went wrong when opening analogIn port, closing.\n");
        exit(1); 
      }
    }
  } else {
    printf("Something went wrong when opening analogIn port, closing.\n");
    exit(1);   
  }
}
/*Attempts to open an analogOut port*/
void analogOutOpen(int channel)
{
  if (channel >= 0 && channel <= gAout.count) {
    AnalogPort *p;

    p = &gAout.port[channel];
    if (Open(&p->port, channel)  == 1) {
      // Get range on first open of channel
      comedi_range *range;

      range = GetRange(&p->port, 0);
      p->maxdata = Maxdata(&p->port);
      if (range) {
	p->min = range->min;
	p->max = range->max;
	p->delta = (p->max - p->min) / p->maxdata;
      } else {
	Close(&p->port, channel);
	printf("Something went wrong when opening analogOut port, closing.\n");
        exit(1);
      }
    }
  } else {
    printf("Comedi device doesn't exist.\n");
    exit(1);   
  }
}

/*Attempts to close an analogIn port*/
void analogInClose(int channel)
{
  if (channel >= 0 && channel <= gAin.count) {
    Close(&gAin.port[channel].port, channel);
  } else {
    printf("Comedi device doesn't exist.\n");
    exit(1);    
  }
}
/*Attempts to close an analogOut port*/
void analogOutClose(int channel)
{
  if (channel >= 0 && channel <= gAout.count) {
    Close(&gAout.port[channel].port, channel);
  } else {
    printf("Comedi device doesn't exist.\n");
    exit(1);
  }
}

/*Read from config file and initialize things*/
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


