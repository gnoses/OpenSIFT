/*
  Displays image features from a file on an image

  Copyright (C) 2006-2012  Rob Hess <rob@iqengines.com>

  @version 1.1.2-20100521
*/
#include "stdafx.h"
#include "imgfeatures.h"
#include "utils.h"

#define OPTIONS ":oh"

/*************************** Function Prototypes *****************************/

static void usage( char* );
//static void arg_parse( int, char** );

/******************************** Globals ************************************/

char* img_file;
int feat_type = FEATURE_OXFD;// FEATURE_LOWE;


/********************************** Main *************************************/


int DisplayFeature(char *filename, char *featureFile)
{
  IplImage* img;
  struct SIFTFeature* feat;
  char* name;
  int n;

  //arg_parse( argc, argv );

  img = cvLoadImage( filename, 1 );
  if( ! img )
    fatal_error( "unable to load image from %s", filename );
  n = import_features( featureFile, feat_type, &feat );
  if( n == -1 )
    fatal_error( "unable to import features from %s", featureFile );
  name = featureFile;

  draw_features( img, feat, n );
  cvNamedWindow( name, 1 );
  cvShowImage( name, img );
  cvWaitKey( 0 );
  return 0;
}


/************************** Function Definitions *****************************/

/*
  Print usage info for this program.

  @param name program name
*/
static void usage( char* name )
{
  fprintf( stderr, "%s: display image features on an image\n\n", name );
  fprintf( stderr, "Usage: %s [options] <feat_file> <img_file>\n", name );
  fprintf( stderr, "Options:\n" );
  fprintf( stderr, "  -h      Display this message and exit\n" );
  fprintf( stderr, "  -o      Specifies that <feat_file> is an"	\
	   " Oxford-type feature file.  By\n");
  fprintf( stderr, "             default, it is treated as a"	\
	   " Lowe-type file.\n" );
}


