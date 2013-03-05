// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2//video/video.hpp>
#include <stdarg.h>

using namespace cv;


#define M_PI 3.1415926535


extern void fatal_error(char* format, ...);


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

class CStopWatch
{
private:
	LARGE_INTEGER freq, start, end;
public:
	CStopWatch()
	{
		QueryPerformanceFrequency(&freq);
		start.QuadPart = 0; end.QuadPart = 0;
		StartTime();
	};

	void StartTime()
	{		
		QueryPerformanceCounter(&start);
	};

	double CheckTime()
	{
		QueryPerformanceCounter(&end);

		return (double)(end.QuadPart-start.QuadPart)/freq.QuadPart*1000;
		//TRACE("%.2f msec\n", elapsed);
	};
};