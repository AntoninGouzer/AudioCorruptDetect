/*------------------------------------------------------------------------------

 AudioCorruptDetect
 Some checks on audio files: Sample Hold, digital clicks, start and end Cuts, digital mutes, clipping.

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
#include <getopt.h>
#include <string.h>
#include <ctype.h>
#include "sound.h"
#include "AudioCorruptDetect.h"

/*----------------------------------------------------------------------------*/
static void print_usage (char *progname, int clicksDetect,int audacityMode,int SampleHoldDetect,int sampleHoldTresh,int mergeTimeHoldSec,int CutStartDetect,int CutStartTreshdB,int CutEndDetect,int CutEndTreshdB )
{
    fprintf(stderr,"usage: AudioCorruptDetect -i in.wav [OPTIONS]\n"
                     "optional parameters:\n"
                     "       -audacityMode: Audacity Mode ,  output Audacity Marker Std Out (default %d)\n"
                     "       -clicksDetect: Detect presence of digital clicks, (default %d)\n"
		     "       -SampleHoldDetect: Detect consecutive same sample Values , (default %d)\n"
		     "       -sampleHoldTresh x:Number of  consecutive same sample Values , (default %d)\n"
		     "       -mergeTimeHoldSec x:Number seconds for merging Sample Holds alerts, (default %d)\n"
		     "       -CutStartDetect: Detect an abnormal audio level at the start sample, (default %d)\n"
		     "       -CutStartTreshdB: Audio level alert treshhold at the start sample, (default %d)\n"
		     "       -CutEndDetect: Detect an abnormal audio level at the End sample, (default %d)\n"
		     "       -CutEndTreshdB: Audio level alert treshhold at the End sample, (default %d)\n"  ,
                     clicksDetect,audacityMode,SampleHoldDetect,sampleHoldTresh,mergeTimeHoldSec,CutStartDetect,CutStartTreshdB,CutEndDetect,CutEndTreshdB);
      exit(EXIT_FAILURE);

} /* print_usage */


int main(int argc, char ** argv)
{
    char  *progname, *infilename ;
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
  static int audacityMode= 0; /*Audacity Mode,  output Audacity marker Std Out*/
  static int SampleHoldDetect= 0; /*Detect consecutive same sample Values*/
  static int clicksDetect= 0; /*Detect consecutive same sample Values*/
  static int CutStartDetect= 0; /*Detect consecutive same sample Values*/
  static int CutEndDetect= 0 ; /*Detect consecutive same sample Values*/
  int sampleHoldTresh= 4; /* Number of  consecutive same sample Values*/
  int  mergeTimeHoldSamples=0;
  int mergeTimeHoldSec=0;
  int CutStartTreshdB=-18;
  int CutEndTreshdB=-18;
  int CutStartNumSamples=1;
  int CutEndNumSamples=1;
  long nLongBuffer=100000;
  long readCount=0;
  Sound * signal = NULL;
  Sound * soundInfo = NULL;
  int * burst1 = NULL;
  int * burst2 = NULL;
  int nSamples = 0;
  long numAudioSamples = 0;
  int numChannels =0;
  int resSampleHoldDetect,resStartCut,resEndCut;

  int i,c,n;

  /* read input input */
  if( argc < 2 )
    {
     print_usage (progname,clicksDetect,audacityMode,SampleHoldDetect,sampleHoldTresh,mergeTimeHoldSec,CutStartDetect,CutStartTreshdB,CutEndDetect,CutEndTreshdB) ;
     return 1 ;
    }

  while (1)
    {
      static struct option long_options[] =
        {
          /* These options set a flag. */
        {"infilename",     required_argument,       0, 'i'},
          {"SampleHoldDetect", no_argument,       &SampleHoldDetect, 1},
          {"clicksDetect",   no_argument,       &clicksDetect, 1},
          {"audacityMode",  no_argument, &audacityMode, 1},
          {"CutStartDetect",  no_argument, &CutStartDetect, 1},
          {"CutEndDetect",  no_argument, &CutEndDetect, 1},
          {"sampleHoldTresh",  required_argument,       0, 't'},
          {"mergeTimeHoldSec",  required_argument, 0, 'm'},
          {"CutStartTreshdB",  required_argument, 0, 's'},
          {"CutEndTreshdB",  required_argument, 0, 'e'},
          {0, 0, 0, 0}
        };
      /* getopt_long stores the option index here. */
      int option_index = 0;

      n = getopt_long_only (argc, argv, "i:t:m:s:e:",
                       long_options, &option_index);

      /* Detect the end of the options. */
      if (n == -1)
        break;

      switch (n)
        {
        case 0:
          /* If this option set a flag, do nothing else now. */
          if (long_options[option_index].flag != 0)
            break;
          if (optarg)
          break;

        case 'i':
          infilename= optarg;
          break;

        case 't':
          sampleHoldTresh= atoi(optarg);
          break;

        case 'm':
          mergeTimeHoldSec= atoi(optarg);
          break;

        case 's':
          CutStartTreshdB= atoi(optarg);
          printf("optarg %s \n",optarg);
          break;

        case 'e':
          CutEndTreshdB= atoi(optarg);
          break;

        case '?':
          /* getopt_long already printed an error message. */
          break;

        default:
          abort ();
        }
    }

  /* Print any remaining command line arguments (not options). */
  if (optind < argc)
    {
      printf ("Be carefull, this arguments are incorrects: ");
      while (optind < argc)
        printf ("%s ", argv[optind++]);
      putchar ('\n');
    }


	if (infilename [0] == '-')
	{	printf ("Error : Input filename (%s) looks like an option.\n\n", infilename) ;
		 print_usage (progname,clicksDetect,audacityMode,SampleHoldDetect,sampleHoldTresh,mergeTimeHoldSec,CutStartDetect,CutStartTreshdB,CutEndDetect,CutEndTreshdB) ;
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



 if(audacityMode==0)
{
  printf("input signal: %d channels, %ld samples\n",
         numChannels,numAudioSamples);
  printf("parameters: K=%.2f b=%d p=%d Nw=%d T=%d audacityMode=%d\n\n",K,b,p,Nw,T,audacityMode);
}
/*CutStartDetect Detect  Mode*/
if (CutStartDetect==1)
{
    resStartCut=start_cut_detect (infile, &sfinfo, CutStartTreshdB,CutStartNumSamples);
}
  /* free memory */
    sf_close (infile) ;
    memset (&sfinfo, 0, sizeof (sfinfo)) ;

    if ((infile = sf_open (infilename, SFM_READ, &sfinfo)) == NULL)
	{	printf ("Not able to open input file %s.\n", infilename) ;
		puts (sf_strerror (NULL)) ;
		return 1 ;
		} ;
/*Sample Hold Detect  Mode*/
if (SampleHoldDetect==1)
{
//integration time of events
    mergeTimeHoldSamples=mergeTimeHoldSec * sfinfo.samplerate;
    resSampleHoldDetect=sample_hold_detect (infile, sfinfo.channels,sfinfo.samplerate, sampleHoldTresh,mergeTimeHoldSamples) ;
}
  /* free memory */
    sf_close (infile) ;
    memset (&sfinfo, 0, sizeof (sfinfo)) ;

    if ((infile = sf_open (infilename, SFM_READ, &sfinfo)) == NULL)
	{	printf ("Not able to open input file %s.\n", infilename) ;
		puts (sf_strerror (NULL)) ;
		return 1 ;
		} ;
/*CutEndDetect Detect  Mode*/
if (CutEndDetect==1)
{
    resEndCut=end_cut_detect (infile, &sfinfo, CutEndTreshdB,CutEndNumSamples);
}

  /* free memory */
    sf_close (infile) ;
  return 0;
}
/*----------------------------------------------------------------------------*/
