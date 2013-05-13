#include <iostream>
#include <stdio.h>
#include "MultiscaleEditing.h"
#include "highgui.h"

using namespace std;

int ITERATION_ENLARGE = 30;

int MAX_LEVEL = 5;


void DoSynthesisGivenInitialGuess()
{
	printf("Synthesis iteration per level: ");
	cin >> ITERATION_ENLARGE;
	printf("Synthesis multiscale Level: ");
	cin >> MAX_LEVEL;

	IplImage *src = cvLoadImage("src.png");
	IplImage *init = cvLoadImage("init.png");
	IplImage *dst = MultiscaleGivenInitialGuess(src, init, 0, MAX_LEVEL);
}

int main()
{
	
	DoSynthesisGivenInitialGuess();

	system("PAUSE");
	return 0;
}