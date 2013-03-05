#include "stdafx.h"

extern void GenerateFeature(char *filename, char *featureFile);
extern int DisplayFeature(char *filename, char*);
extern int SIFTMatch(char *filename1, char *filename2);
extern int SIFTMatch(struct SIFTFeature* srcFeature, int , IplImage* image);
extern int SIFTGenerateFeature(char *filename, struct SIFTFeature** feature, int *);


char filename[1024] = "face.png";
char filename2[1024] = "face2.png";
char featureFile[1024] = "feature.dat";
int onDrag = 0;
CvRect box;
int onTraining = 1;
struct SIFTFeature* srcFeature = 0;
int featureCount = 0;


void DrawBox(IplImage *image, CvRect rect)
{
	cvRectangle(image, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),
		cvScalar(0xff, 0x00, 0x00));

	//printf("draw %d %d %d %d\n", rect.x, rect.y, rect.width, rect.height);
}

void SaveModelImage(IplImage *image)
{
	cvSetImageROI(image, cvRect(box.x, box.y, box.width, box.height));			
	IplImage *cropImage = cvCreateImage(cvGetSize(image), image->depth, image->nChannels);
	cvCopy(image, cropImage);
	cvSaveImage("face.png", cropImage);
	cvReleaseImage( &cropImage );
	cvSetImageROI(image, cvRect(0,0,image->width, image->height));
	printf("Model saved\n");
}

void MouseProc(int event, int x, int y, int flags, void *param)
{
	IplImage *image = (IplImage*)param;
	
	switch (event)
	{
	case CV_EVENT_MOUSEMOVE:
		if (onDrag)
		{
			box.width = x-box.x;
			box.height = y-box.y;
			//DrawBox(image, box);
		}
		break;
	case CV_EVENT_LBUTTONDOWN:
		onDrag = 1;
		box = cvRect(x,y,0,0);
		break;
	case CV_EVENT_LBUTTONUP:
		onDrag = 0;
		if (box.width < 0)
		{
			box.x += box.width;
			box.width *= - 1;
		}
		if (box.height < 0)
		{
			box.y += box.height;
			box.height *= -1;
			
		}
		SaveModelImage(image);
		SIFTGenerateFeature(filename, &srcFeature, &featureCount);
		onTraining = 0;
		//DrawBox(image, box);
		break;
	}
}

void SIFTOnline()
{
	CvCapture *capture = cvCaptureFromCAM(0);
	cvNamedWindow("SIFT", 1);
	CvVideoWriter *videoOut = NULL;
	IplImage *image = 0;
	
	cvGrabFrame(capture);
	image = cvRetrieveFrame(capture);
	cvSetMouseCallback("SIFT", MouseProc, (void*) image);
	int c;
	
	CStopWatch w;

	while(1)
	{
		cvGrabFrame(capture);
		image = cvRetrieveFrame(capture);
		cvFlip(image, image, 1);
		if (onTraining == 0)
		{
			SIFTMatch(srcFeature, featureCount, image);
			printf("%f fps\n", w.CheckTime()/1000);
			w.StartTime();
		}
		else
		{
			DrawBox(image, box);
		}
		cvShowImage("SIFT", image);
		
		//cvSaveImage("face2.png", image);

		//break;
		
		c = cvWaitKey(10);
		
		if (c == 't')
		{
			onTraining = 1;
		}
		else if (c > 0)
		{
			break;
		}			
	}
	cvReleaseImage( &image );
	free( srcFeature );
}

void main()
{
	
	SIFTOnline();
	//GenerateFeature(filename,featureFile);
	// DisplayFeature(filename, featureFile);
	//SIFTMatch(filename, filename2);
}