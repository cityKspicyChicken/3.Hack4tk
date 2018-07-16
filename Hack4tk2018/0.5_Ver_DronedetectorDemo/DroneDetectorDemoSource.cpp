#include<iostream>
#include"opencv2/imgcodecs.hpp"
#include"opencv2/imgproc/imgproc.hpp"
#include"opencv2/highgui/highgui.hpp"
#include<stdio.h>
using namespace std;
using namespace cv;

Mat input_image,gray_image,hsv_image,hsv_equalize_image,hsv_filted_image,blured_image,/*canny_image,*/element_1,element_2;
int area_treshold_min_div=10000.0;
int area_treshold_max_div=100000.0;
int adjust_value1 = 55;
int adjust_value2 = 3;
const char* source_color_window = "source color image";
const char* hsv_filter_window = "hsv filted image";
//const char* canny_window = "canny image";
const char* contours_window = "contours image";
double area_contour;
Mat equalizeChannelHist(const Mat & inputImage)
{
	if (inputImage.channels() >= 3)
	{
		vector<Mat> channels;
		split(inputImage, channels);

		Mat H, S, V;

		equalizeHist(channels[0], H);
		equalizeHist(channels[1], S);
		equalizeHist(channels[2], V);

		vector<Mat> combined;
		combined.push_back(H);
		combined.push_back(S);
		combined.push_back(V);

		Mat result;
		merge(combined, result);

		return result;
	}

	return Mat();
}

Mat HSV_Filter(const Mat & inputImage)
{
	if (inputImage.channels() >= 3)
	{
		vector<Mat> channels;
		Mat ImageH;
		Mat ImageS;
		Mat ImageV;
		split(inputImage, channels);
		ImageH = channels.at(0);
		ImageS = channels.at(1);
		ImageV = channels.at(2);
		uchar *h = ImageH.data;
		uchar *s = ImageS.data;
		uchar *v = ImageV.data;
		for (int i = 0; i < ImageH.rows * ImageH.cols; i++)
		{
			if ((*h >= 0 && *h <= 30 ) || (*h >= 200 && *h <= 255))
			{
				if (*s >= 0 && *s <= 150)
				{
					if ((*v >= 0 && *v <= 30) || (*v >= 250 && *v <= 255))
					{
						*h = 0;
						*s = 0;
						*v = 255;
					}
					else
					{
						*h = 0;
						*s = 0;
						*v = 0;
					}
				}
				else
				{
					*h = 0;
					*s = 0;
					*v = 0;
				}
			}
			else
			{
				*h = 0;
				*s = 0;
				*v = 0;
			}
			/*cout << *h << "," << *s << "," << *v << endl;*/
			h++;
			s++;
			v++;
		}
		Mat result;
		merge(channels, result);
		return result;
	}
	return Mat();
}

void on_treshold_min(int, void *);
void on_treshold_max(int, void *);
void on_adjust(int, void *);
void process1();
//void process2();

int main( int argc, char** argv )
{

	if( argc != 2)
	    {
	     cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
	     return -1;
	    }

	input_image = imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file

	if(!input_image.data )                              // Check for invalid input
	    {
             cout <<  "Could not open or find the image" << std::endl ;
             return -1;
	    }

	//input_image = imread("1.jpg");
	/*gray_image;
	hsv_image;
	hsv_equalize_image;
	hsv_filted_image;
	blured_image;*/
	//vector<Vec2f> hough_lines;

	
	//const char* hough_window = "hough image";

	namedWindow(source_color_window, WINDOW_NORMAL);
	namedWindow(hsv_filter_window, WINDOW_NORMAL);
	namedWindow(contours_window, WINDOW_NORMAL);
	//namedWindow(canny_window, WINDOW_NORMAL);
	//namedWindow(hough_window, WINDOW_AUTOSIZE);

	createTrackbar("threshold_min: ", hsv_filter_window, &area_treshold_min_div,20000,on_treshold_min);
	createTrackbar("threshold_max: ", hsv_filter_window, &area_treshold_max_div,200000,on_treshold_max);
        createTrackbar("adjust_parameter1: ", hsv_filter_window, &adjust_value1,70,on_adjust);
        createTrackbar("adjust_parameter2: ", hsv_filter_window, &adjust_value2,30,on_adjust);

	waitKey(0);
	return 0;
}
void on_treshold_min(int ,void *)
{
  process1();
  
}


void on_treshold_max(int ,void *)
{
 process1();
  
}
void on_adjust(int ,void *)
{
process1();
}
void process1()
{

cvtColor(input_image, gray_image, CV_BGR2GRAY);
	cvtColor(input_image, hsv_image, CV_BGR2HSV);
	hsv_equalize_image = equalizeChannelHist(hsv_image);
	hsv_filted_image = HSV_Filter(hsv_equalize_image);
	element_1 = getStructuringElement(MORPH_RECT, Size(adjust_value2, adjust_value2));
	element_2 = getStructuringElement(MORPH_RECT, Size(adjust_value1, adjust_value1));
	morphologyEx(hsv_filted_image, hsv_filted_image, MORPH_OPEN, element_1);
	morphologyEx(hsv_filted_image, hsv_filted_image, MORPH_CLOSE, element_2);
	cvtColor(hsv_filted_image, hsv_filted_image, CV_HSV2BGR);
	cvtColor(hsv_filted_image, hsv_filted_image, CV_BGR2GRAY);
	medianBlur(hsv_filted_image, hsv_filted_image, 11);
	GaussianBlur(hsv_filted_image, blured_image, Size(7, 7), 0, 0);
	//Mat tempImg = hsv_filted_image.clone();
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(hsv_filted_image, contours, hierarchy, CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
	cout << "contoursSize=" << contours.size() << endl;
imshow(source_color_window, input_image);
	//for (int idx= 0; idx >= 0; idx = hierarchy[idx][0])
	//{
	//	Scalar color(rand() & 255, rand() & 255, rand() & 255);
	//	drawContours(tempImg, contours, idx, color, FILLED, 8, hierarchy);
	//}t
	int sum(0);

	for (int i = 0; i < contours.size(); ++i)
	{
		double area_contour = contourArea(contours[i]);
		cout << "Area_Contour: " << area_contour << endl;
		if (area_contour >= area_treshold_min_div && area_contour <= area_treshold_max_div)
		{
			sum += 1;
		}
	}
	cout << "The total number of coils is: " << sum << endl;
	//double g_dsrcArea = contourArea(contours, true);
	//Canny(hsv_filted_image, canny_image, 60, 120, 3);
	//HoughLines(canny_image, hough_lines, );
	//findContours(hsv_filted_image, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);//CV_RETR_EXTERNAL只检测外部轮廓，可根据自身需求进行调整

	Mat contoursImage(hsv_filted_image.rows, hsv_filted_image.cols, CV_8U, Scalar(255));
	int index = 0;
	for (; index >= 0; index = hierarchy[index][0]) {
		cv::Scalar color(rand() & 255, rand() & 255, rand() & 255);
		// for opencv 2
		// cv::drawContours(dstImage, contours, index, color,  CV_FILLED, 8, hierarchy);//CV_FILLED所在位置表示轮廓线条粗细度，如果为负值（如thickness==cv_filled），绘制在轮廓内部
		// for opencv 3
		//cv::drawContours(contoursImage, contours, index, color, cv::FILLED, 8, hierarchy);

		cv::drawContours(contoursImage, contours, index, Scalar(0), 1, 8, hierarchy);//描绘外轮廓

		Rect rect = boundingRect(contours[index]);//检测外轮廓
		rectangle(contoursImage, rect,(0,255,0),5);//对外轮廓加矩形框
	}
	imshow(hsv_filter_window, hsv_filted_image);
	imshow(contours_window, contoursImage);
	//imshow(canny_window, canny_image);
        
	waitKey(0);
	
	
	

}
