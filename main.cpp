/***
  This file is part of PulseAudio.
  PulseAudio is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published
  by the Free Software Foundation; either version 2.1 of the License,
  or (at your option) any later version.
  PulseAudio is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.
  You should have received a copy of the GNU Lesser General Public License
  along with PulseAudio; if not, see <http://www.gnu.org/licenses/>.
 ***/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <BTrack.h>
#include "matrix.h"
#define BUFSIZE 1024
#define AMPLITUDE_WINDOW 2

int main(int argc, char*argv[]) {
  /* The sample type to use */
  static const pa_sample_spec ss = {
    .format = PA_SAMPLE_FLOAT32LE,
    .rate = 44100,
    .channels = 1
  };
  if(argc < 2)
    return 1;
  BTrack b;
  Matrix mat;
  pa_simple *s = NULL;
  int ret = 1;
  int error;
  double slow_acc=0;
  float amplitude;
  int led_count=0;
  int slow_acc_ctr=0;;
  unsigned int ctr=0;
  /* Create the recording stream */
  if (!(s = pa_simple_new(NULL, argv[0], PA_STREAM_RECORD, argv[1], "record", &ss, NULL, NULL, &error))) {
    fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
    goto finish;
  }
  for (;;) {
    float buf[BUFSIZE];
    double bufdbl[BUFSIZE];
    float acc=0;
    /* Record some data ... */
    if (pa_simple_read(s, buf, sizeof(buf), &error) < 0) {
      fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
      goto finish;
    }
    for(unsigned int i=0;i<BUFSIZE;i++){
      bufdbl[i]=buf[i];
      acc+=fabs(buf[i]);
      slow_acc += fabs(buf[i]);
    }

    acc/=BUFSIZE;

    slow_acc_ctr++;

    if(slow_acc_ctr == AMPLITUDE_WINDOW){
      slow_acc = 10*log10(slow_acc / (AMPLITUDE_WINDOW * BUFSIZE));
      amplitude=slow_acc + 35;
      if(amplitude > 35)
        led_count = 35;
      else if(amplitude < 0)
        led_count = 0;
      else
        led_count = amplitude;


      slow_acc_ctr=0;
      slow_acc=0;
    }

    b.processAudioFrame(bufdbl);
    if (b.beatDueInCurrentFrame())
    {
      ctr++;
      if(ctr==3)
        ctr=0;
    }
    switch(ctr){
      case 0:
        mat.setEverloopCircular(led_count,255,0,0,0);                   
        break;
      case 1:
        mat.setEverloopCircular(led_count,0,255,0,0);                   
        break;
      case 2:
        mat.setEverloopCircular(led_count,0,0,255,0);                   
    } 
  }
  ret = 0;
finish:
  if (s)
    pa_simple_free(s);
  return ret;
}
