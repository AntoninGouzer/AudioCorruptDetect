/*------------------------------------------------------------------------------

 AudioCorruptDetect

  V2.0:  add analyse only support
  V2.1: add Sample Hold detect support
  V2.2: Large files

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU Affero General Public License for more details.

  You should have received a copy of the GNU Affero General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.

------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "sound.h"
#include "AudioCorruptDetect.h"

/*----------------------------------------------------------------------------*/
static void print_usage (char *progname,double K,int b,int p,int Nw,int T,int Aud,int SampleHold,int sampleHoldTresh)
{
    fprintf(stderr,"usage: AudioCorruptDetect in.wav out.wav [K b p Nw]\n"
                     "optional parameters:\n"
                     "       K: threshold parameter    (default value %0.2f)\n"
                     "       b: burst fusion parameter (default value %d)\n"
                     "       p: order of AR model      (default value %d)\n"
                     "       Nw: window size           (default value %d)\n"
                     "       T: Test Mode , no output file (default value %d)\n"
                     "       Aud: Audacity Mode ,  output Audacity Marker Std Out (default value %d)\n"
		     "       SampleHold: Detect consecutive same sample Values , (default value %d)\n"
		     "       sampleHoldTresh:Number of  consecutive same sample Values , (default value %d)\n" ,
                     K,b,p,Nw,T,Aud,SampleHold,sampleHoldTresh);
      exit(EXIT_FAILURE);

} /* print_usage */


int main(int argc, char ** argv)
{
    char  *progname, *infilename, *outfilename ;
    SNDFILE		*infile = NULL ;
    FILE		*outfile = NULL ;
    SF_INFO		sfinfo ;

   progname = strrchr (argv [0], '/') ;
   progname = progname ? progname + 1 : argv [0] ;

  double K = 2.0;    /* First threshold */
  int b = 20;        /* Second threshold */
  int p = 3*100+2;   /* Order of the model */
  int Nw = 8*p;      /* Window length */
  int T = 0; /*Test Mode, Wav output generated*/
  int Aud= 0; /*Audacity Mode,  output Audacity marker Std Out*/
  int SampleHold= 0; /*Detect consecutive same sample Values*/
  int sampleHoldTresh= 3; /* Number of  consecutive same sample Values*/
  int mergeTimeHoldsFeMultiplier=1;
  int mergeTimeHolds=0;
  long nLongBuffer=100000;
  long readCount=0;
  Sound * signal = NULL;
  Sound * soundInfo = NULL;
  int * burst1 = NULL;
  int * burst2 = NULL;
  int nSamples = 0;
  long numAudioSamples = 0;
  int numChannels =0;
  int res;
  int i,c;

  /* read input input */
  if( argc < 3 )
    {
     print_usage (progname,K,b,p,Nw,T,Aud,SampleHold,sampleHoldTresh) ;
     return 1 ;
    }

    infilename = argv [1] ;
     outfilename = argv [2] ;

if (strcmp (infilename, outfilename) == 0)
	{	printf ("Error : Input and output filenames are the same.\n\n") ;
		 print_usage (progname,K,b,p,Nw,T,Aud,SampleHold,sampleHoldTresh) ;
		return 1 ;
		} ;

	if (infilename [0] == '-')
	{	printf ("Error : Input filename (%s) looks like an option.\n\n", infilename) ;
		 print_usage (progname,K,b,p,Nw,T,Aud,SampleHold,sampleHoldTresh) ;
		return 1 ;
		} ;

	if (outfilename [0] == '-')
	{	printf ("Error : Output filename (%s) looks like an option.\n\n", outfilename) ;
		 print_usage (progname,K,b,p,Nw,T,Aud,SampleHold,sampleHoldTresh) ;
		return 1 ;
		} ;

    memset (&sfinfo, 0, sizeof (sfinfo)) ;
    if ((infile = sf_open (infilename, SFM_READ, &sfinfo)) == NULL)
	{	printf ("Not able to open input file %s.\n", infilename) ;
		puts (sf_strerror (NULL)) ;
		return 1 ;
		} ;

    numAudioSamples = sfinfo.frames;
    numChannels = sfinfo.channels;


  if( argc >= 4 ) K  = atof(argv[3]);
  if( argc >= 5 ) b  = atoi(argv[4]);
  if( argc >= 6 ) p  = atoi(argv[5]);
  if( argc >= 7 ) Nw = atoi(argv[6]);
  if( argc >= 8 ) T = atoi(argv[7]);
  if( argc >= 9 ) Aud = atoi(argv[8]);
  if( argc >= 10) SampleHold = atoi(argv[9]);
  if( argc >= 11) sampleHoldTresh = atoi(argv[10]);

 if(Aud==0)
{
  printf("input signal: %d channels, %ld samples\n",
         numChannels,numAudioSamples);
  printf("parameters: K=%.2f b=%d p=%d Nw=%d T=%d Aud=%d\n\n",K,b,p,Nw,T,Aud);
}

/*Sample Hold Detect  Mode*/
if (SampleHold==1)
{
//integration time of events
mergeTimeHolds=mergeTimeHoldsFeMultiplier * sfinfo.samplerate;

    res=sample_hold_detect (infile, sfinfo.channels,sfinfo.samplerate, sampleHoldTresh,mergeTimeHolds) ;
    sf_close (infile) ;

}
  /* free memory */

  return 0;
}
/*----------------------------------------------------------------------------*/
