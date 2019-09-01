#include<opencv2/opencv.hpp>
#include "ImageHandelingComponent.h"

using namespace std;
using namespace cv;

int main(int argv, char** argc)
{

	ImageHandelingComponent imageHandelingComponent = ImageHandelingComponent();

	int key = 0;
	while (key != 27)
	{
		imageHandelingComponent.camptureScreen();

		if (imageHandelingComponent.cropToGameWindow())
			imageHandelingComponent.drawGridBins();
		else
			cout << "Could not Crop\n";


		//vector<Point2f> List_Matches;
		//Point matchP;
		//singleTemplateMatchingGrey(imgScreenGray, imgScreenGray, 0.9, 0.9, matchP);
		//rectangle(mResult_Bgr, matchP, Point(matchP.x + mTemplate_Bgr.cols, matchP.y + mTemplate_Bgr.rows), Scalar(0, 255, 0), 2);

		/*for (int i = 0; i < List_Matches.size(); i++)
		{
			rectangle(mResult_Bgr, List_Matches[i], Point(List_Matches[i].x + mTemplate_Bgr.cols, List_Matches[i].y + mTemplate_Bgr.rows), Scalar(0, 255, 0), 2);
		}*/

		key = waitKey(60);
		//imshow("Color Img", imageHandelingComponent.imgScreen.imgColor);
		//imshow("Gray Img", imgScreenGray);
	}
	waitKey(0);
	return 0;
}