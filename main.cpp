#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <BTrack.h>
#include <assert.h>
#include <lo/lo.h>
#include <lo/lo_cpp.h>

#define BUFSIZE 1024

typedef struct {
    char const *osc_addr;
    int osc_port;
    char const *osc_beatpath;
    char const *osc_amplpath;
    unsigned int minampl;
    unsigned int maxampl;
    double gain;
    unsigned int amplwind;
} options_t;

template <class T>
T lin_map (T x, T in_min, T in_max, T out_min, T out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void print_startup(options_t *opt){
    printf("\n---osc-audio-visualizer--\n\n \
    Running with input parameters: \n \
    OSC address = %s\n \
    OSC port = %d\n \
    OSC beat info output path = %s\n \
    OSC amplitude output path = %s\n \
    Min amplitude = %d\n \
    Max amplitude = %d\n \
    Input gain = %f\n \
    Amplitude window = %d (%d samples)\n\n \
\n \
    This program is free software: you can redistribute it and/or modify\n \
    it under the terms of the GNU General Public License as published by\n \
    the Free Software Foundation, either version 3 of the License, or\n \
    (at your option) any later version.\n \
\n \
    This program is distributed in the hope that it will be useful,\n \
    but WITHOUT ANY WARRANTY; without even the implied warranty of\n \
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n \
    GNU General Public License for more details.",opt->osc_addr,opt->osc_port,opt->osc_beatpath,opt->osc_amplpath,opt->minampl,opt->maxampl,opt->gain,opt->amplwind,opt->amplwind*BUFSIZE);
}

void print_usage(char* exename){
    printf("Usage: %s \n \
            -a OSC address, default='localhost'\n \
            -p OSC port, default='9000'\n \
            -B OSC path to send beat info, default='/beat'\n \
            -A OSC path to send average amplitude, default='/amplitude'\n \
            -m Amplitude value corresponding to -infinity dB, default='0'\n \
            -M Amplitude value corresponding to +10 dB (hard capped), default='255'\n \
            -G Gain applied to incoming audio, default=1.0\n \
            -W Amplitude averaging window size, default=2\n \
            -h Prints this message\n\n",exename);
}

/* sets the default values of the configuration and parses the arguments */
int parse_arguments(options_t *opt, int argc, char **argv){
    /* Set defaults */
    opt->osc_addr="localhost";
    opt->osc_port=9000;
    opt->osc_beatpath="/beat";
    opt->osc_amplpath="/amplitude";
    opt->minampl=0;
    opt->maxampl=255;
    opt->amplwind=2;
    opt->gain=1;
    
    /* Parse arguments */
    int c;
    int cnt=0;
    opterr = 0;
    while ((c = getopt (argc, argv, "ha:p:B:A:m:M:G:W:")) != -1){
        cnt++;
        switch (c)
        {
            case 'a':
                opt->osc_addr = optarg;
                break;
            case 'p':
                opt->osc_port = atoi(optarg);
                break;
            case 'B':
                opt->osc_beatpath = optarg;
                break;
            case 'A':
                opt->osc_amplpath = optarg;
                break;
            case 'm':
                opt->minampl = atoi(optarg);
                break;
            case 'M':
                opt->maxampl = atoi(optarg);
                break;
            case 'G':
                opt->gain = atof(optarg);
                break;
            case 'W':
                opt->amplwind = atoi(optarg);
                break;
            case 'h':
                print_usage(argv[0]);
                exit(EXIT_SUCCESS);
            case '?':
                if (strchr("apBAmMGW",optopt) != NULL)
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr,
                            "Unknown option character `\\x%x'.\n",
                            optopt);
                return -1;
            default:
                abort();
        }
    }
    return cnt;
}

/* Reads exactly n s16le samples from the provided fd */ 
ssize_t read_samples_fd(int fd, int16_t* buf, size_t n){
    ssize_t cur_read;
    size_t nread=0;
    while(nread != n){
        cur_read = read(fd,buf,n-nread);
        if (cur_read < 0) {
            perror("Reading samples failed");
            return -1;
        } else if( cur_read == 0 ) {
            return 0;
        }                   
        nread += cur_read;
    }
    /* Test for a big-endian machine and swap all bytes if needed*/
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    for(int i=0; i<n; i++)
        buf[i] = (buf[i] << 8) | (((uint16_t) buf[i]) >> 8);
#endif
    return nread;
}

int main(int argc, char*argv[]) {
    BTrack b;
    options_t options;
    int i,j,rv;
    ssize_t nread;
    double slow_acc=0;
    float amplitude;
    unsigned int slow_acc_ctr=0;
    /* Initialize and parse args */
    if((rv=parse_arguments(&options, argc, argv)) < 0)
        return -rv;
    /* Print the startup message */
    print_startup(&options);

    /* Initialize liblo */
    lo::Address lo_addr(options.osc_addr, options.osc_port);

    /* Main processing loop */
    for (;;) {
        int16_t buf[BUFSIZE*2];
        double bufdbl[BUFSIZE];
        float acc=0;
        /* Record some data ... */
        nread=read_samples_fd(STDIN_FILENO,buf,BUFSIZE*2);
            if(nread <= 0)
                return -nread;
        /* convert left (1st) channel to double*/
        j=0;
        assert(nread==(2*BUFSIZE));
        for(i=0; i<nread; i++){
            if(i%2 == 0){
                bufdbl[j] = (((double)buf[i])/32768.0)*options.gain;
                j++;
            }
        }
        /* Iterate accumulators and apply gain*/
        for(unsigned int i=0;i<BUFSIZE;i++){
            acc+=fabs(bufdbl[i]);
            slow_acc += fabs(bufdbl[i] * options.gain);
        }
        acc/=BUFSIZE;
        slow_acc_ctr++;

        /* Compute avg. amplitude in dB if we have seen AMPLITUDE_WINDOW FFT windows */
        if(slow_acc_ctr == options.amplwind){
            slow_acc = 20*log10(slow_acc / (options.amplwind * (BUFSIZE)));
            if (slow_acc < -60)
                slow_acc = -60;
            else if(slow_acc > 0)
                slow_acc = 0;
            amplitude=lin_map<double>(slow_acc,-60,0,options.minampl,options.maxampl);

            lo_addr.send(options.osc_amplpath,"i",(int)amplitude);
            slow_acc_ctr=0;
            slow_acc=0;
        }
        /* Compute FFT and search for beats */
        b.processAudioFrame(bufdbl);
        if (b.beatDueInCurrentFrame()) {
            /* Handle a beat here */
                lo_addr.send(options.osc_beatpath,"i",255);
                lo_addr.send(options.osc_beatpath,"i",0);
        }
    }
    return 0;
}
