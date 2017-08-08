#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <BTrack.h>
#include "matrix.h"
#define BUFSIZE 2048
#define AMPLITUDE_WINDOW 2
#define ROTATE_SPEED 1
int main(int argc, char*argv[]) {
   BTrack b;
   Matrix mat;
   int i,j;
   double slow_acc=0;
   float amplitude;
   int led_count=0;
   unsigned int rotation=0;
   unsigned int slow_acc_ctr=0;
   unsigned int ctr=0;
   ssize_t nread, cur_read;
   for (;;) {
      int16_t buf[BUFSIZE];
      double bufdbl[BUFSIZE/2];
      float acc=0;
      /* Record some data ... */
      nread=0;
      while(nread != BUFSIZE){
         cur_read = read(STDIN_FILENO,buf,BUFSIZE-nread);
         if (cur_read < 0) {
            perror("Reading samples failed");
            return 1;
         } else if( cur_read == 0 ) {
            mat.setEverloopAll(0,0,0,0);
            return 0;
         }                   
         nread += cur_read;
      }
      /* convert left (1st) channel to double */
      j=0;
      for(i=0; i<nread; i++){
         if(i%2 == 0){
            bufdbl[j] = ((double)buf[i])/32768.0;
            j++;
         }
      }
      /* ---Process a window--- */   

      /* Iterate accumulators*/
      for(unsigned int i=0;i<BUFSIZE/2;i++){
         acc+=fabs(bufdbl[i]);
         slow_acc += fabs(bufdbl[i]);
      }
      acc/=BUFSIZE;
      slow_acc_ctr++;

      /* Compute avg. amplitude in dB if we have seen AMPLITUDE_WINDOW FFT windows */
      if(slow_acc_ctr == AMPLITUDE_WINDOW){
         slow_acc = 20*log10(slow_acc / (AMPLITUDE_WINDOW * (BUFSIZE/2)));
         amplitude=slow_acc + 45;
         if(amplitude > 35)
            led_count = 35;
         else if(amplitude < 0)
            led_count = 0;
         else
            led_count = amplitude;
         slow_acc_ctr=0;
         slow_acc=0;
      }
      /* Compute FFT and search for beats */
      b.processAudioFrame(bufdbl);
      if (b.beatDueInCurrentFrame()) {
         rotation = (rotation+ROTATE_SPEED)%35;
         ctr++;
      }

      /* Set the LED color based on beat and amplitude */
      switch(ctr){
         case 0:
            mat.setEverloopCircular(led_count,rotation,255,0,0,0);                   
            break;
         case 1:
            mat.setEverloopCircular(led_count,rotation,255,0,255,0);                   
            break;
         case 2:
            mat.setEverloopCircular(led_count,rotation,0,255,255,0);                   
            break;
         case 3:
            mat.setEverloopCircular(led_count,rotation,0,255,0,0);                   
            break;
         case 4:
            mat.setEverloopCircular(led_count,rotation,0,0,255,0);                   
            break;
         case 5:
            mat.setEverloopCircular(led_count,rotation,255,255,0,0);                   
            break;
         case 6:
            mat.setEverloopCircular(led_count,rotation,0,0,0,255);
            ctr=0;
      } 
   }
   return 0;
}
