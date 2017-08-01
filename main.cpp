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
#define BUFSIZE 1024

int main(int argc, char*argv[]) {
    /* The sample type to use */
    static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 44100,
        .channels = 1
    };
    if(argc < 2)
        return 1;
    BTrack b;
    pa_simple *s = NULL;
    int ret = 1;
    unsigned int x=0;
    double bpw = 0;
    double minor_acc=0;
    int error;
    /* Create the recording stream */
    if (!(s = pa_simple_new(NULL, argv[0], PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        goto finish;
    }
    for (;;) {
        uint8_t buf[BUFSIZE];
        double bufdbl[BUFSIZE];
        long acc=0;
        /* Record some data ... */
        if (pa_simple_read(s, buf, sizeof(buf), &error) < 0) {
            fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
            goto finish;
        }
        for(unsigned int i=0;i<BUFSIZE;i++){
            bufdbl[i]=buf[i];
            acc+=buf[i];
        }
        acc/=BUFSIZE;

        b.processAudioFrame(bufdbl);
        if (b.beatDueInCurrentFrame() && acc > 10)
        {
            bpw = (b.getCurrentTempoEstimate() / 60) * ((double)BUFSIZE/44100) / 2;
            bpw *= 4;
            printf("~~tap %f~~\n",minor_acc);
            minor_acc=0;
            x++;
        }
            minor_acc += bpw;
            if(minor_acc >= 0.95){
                printf("--minor--\n");
                minor_acc=0;
            }
    }
    ret = 0;
finish:
    if (s)
        pa_simple_free(s);
    return ret;
}
