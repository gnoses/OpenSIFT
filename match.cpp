/*
Detects SIFT features in two images and finds matches between them.

Copyright (C) 2006-2012  Rob Hess <rob@iqengines.com>

@version 1.1.2-20100521
*/
#include "stdafx.h"
#include "sift.h"
#include "imgfeatures.h"
#include "kdtree.h"
#include "utils.h"
#include "xform.h"



#include <stdio.h>


/* the maximum number of keypoint NN candidates to check during BBF search */
#define KDTREE_BBF_MAX_NN_CHKS 200

/* threshold on squared ratio of distances between NN and 2nd NN */
#define NN_SQ_DIST_RATIO_THR 0.49
extern int DisplayFeature(char *filename, char*);


int SIFTMatch(struct SIFTFeature* srcFeature, int n1, IplImage* image)
{
	struct SIFTFeature* targetFeature;

	struct SIFTFeature** nbrs;
	struct kd_node* kd_root;
	double d0, d1;
	int n2, k, i, m = 0;
	CvPoint pt2;
	n2 = sift_features( image, &targetFeature );
	fprintf( stderr, "Building kd tree...\n" );
	kd_root = kdtree_build( targetFeature, n2 );
	for( i = 0; i < n1; i++ )
	{
		struct SIFTFeature *tempFeature;
		tempFeature = srcFeature + i;
		k = kdtree_bbf_knn( kd_root, tempFeature, 2, &nbrs, KDTREE_BBF_MAX_NN_CHKS );
		if( k == 2 )
		{
			d0 = descr_dist_sq( tempFeature, nbrs[0] );
			d1 = descr_dist_sq( tempFeature, nbrs[1] );
			if( d0 < d1 * NN_SQ_DIST_RATIO_THR )
			{
				//pt1 = cvPoint( cvRound( srcFeature->x ), cvRound( feat->y ) );
				pt2 = cvPoint( cvRound( nbrs[0]->x ), cvRound( nbrs[0]->y ) );
				cvCircle(image, pt2, 5,  CV_RGB(255,0,255));
				//pt2.y += img1->height;
				//cvLine( stacked, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );

				m++;
				targetFeature[i].fwd_match = nbrs[0];
			}
		}
		free( nbrs );
	}

	
	fprintf( stderr, "Found %d total matches\n", m );	

	kdtree_release( kd_root );
	free( targetFeature );

	// fprintf( stderr, "Release All\n", m );	
	return 0;
}



int SIFTGenerateFeature(char *filename, struct SIFTFeature** feature, int *count)
{
	IplImage* img1 = cvLoadImage( filename, 1 );
	if( ! img1 )
	{
		fatal_error( "unable to load image from %s", filename );
		return 0;
	}
	*count = sift_features( img1, feature );
	cvReleaseImage( &img1 );
	return 1;
}


int SIFTMatch(char *filename1, char *filename2)
{
	IplImage* img1, * img2, * stacked;
	struct SIFTFeature* feat1, * feat2, * feat;
	struct SIFTFeature** nbrs;
	struct kd_node* kd_root;
	CvPoint pt1, pt2;
	double d0, d1;
	int n1, n2, k, i, m = 0;

	img1 = cvLoadImage( filename1, 1 );
	if( ! img1 )
		fatal_error( "unable to load image from %s", filename1 );




	img2 = cvLoadImage( filename2, 1 );
	if( ! img2 )
		fatal_error( "unable to load image from %s", filename2 );
	stacked = stack_imgs( img1, img2 );

	fprintf( stderr, "Finding features in %s...\n", filename1 );
	n1 = sift_features( img1, &feat1 );

	draw_features( img1, feat1, n1 );
	cvNamedWindow( "Model", 1 );
	cvShowImage( "Model", img1);

	fprintf( stderr, "Finding features in %s...\n", filename2 );
	n2 = sift_features( img2, &feat2 );
	fprintf( stderr, "Building kd tree...\n" );
	kd_root = kdtree_build( feat2, n2 );
	for( i = 0; i < n1; i++ )
	{
		feat = feat1 + i;
		k = kdtree_bbf_knn( kd_root, feat, 2, &nbrs, KDTREE_BBF_MAX_NN_CHKS );
		if( k == 2 )
		{
			d0 = descr_dist_sq( feat, nbrs[0] );
			d1 = descr_dist_sq( feat, nbrs[1] );
			if( d0 < d1 * NN_SQ_DIST_RATIO_THR )
			{
				pt1 = cvPoint( cvRound( feat->x ), cvRound( feat->y ) );
				pt2 = cvPoint( cvRound( nbrs[0]->x ), cvRound( nbrs[0]->y ) );
				pt2.y += img1->height;
				cvLine( stacked, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );
				m++;
				feat1[i].fwd_match = nbrs[0];
			}
		}
		free( nbrs );
	}

	fprintf( stderr, "Found %d total matches\n", m );
	display_big_img( stacked, "Matches" );
	cvWaitKey( 0 );

	/* 
	UNCOMMENT BELOW TO SEE HOW RANSAC FUNCTION WORKS

	Note that this line above:

	feat1[i].fwd_match = nbrs[0];

	is important for the RANSAC function to work.
	*/
	/*
	{
	CvMat* H;
	IplImage* xformed;
	H = ransac_xform( feat1, n1, FEATURE_FWD_MATCH, lsq_homog, 4, 0.01,
	homog_xfer_err, 3.0, NULL, NULL );
	if( H )
	{
	xformed = cvCreateImage( cvGetSize( img2 ), IPL_DEPTH_8U, 3 );
	cvWarpPerspective( img1, xformed, H, 
	CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS,
	cvScalarAll( 0 ) );
	cvNamedWindow( "Xformed", 1 );
	cvShowImage( "Xformed", xformed );
	cvWaitKey( 0 );
	cvReleaseImage( &xformed );
	cvReleaseMat( &H );
	}
	}
	*/

	cvReleaseImage( &stacked );
	cvReleaseImage( &img1 );
	cvReleaseImage( &img2 );
	kdtree_release( kd_root );
	free( feat1 );
	free( feat2 );
	return 0;
}
