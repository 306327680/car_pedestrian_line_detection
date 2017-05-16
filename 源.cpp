#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include<opencv2/objdetect/objdetect.hpp>  
#include<opencv2/highgui/highgui.hpp>  
#include<opencv2/imgproc/imgproc.hpp>  

using namespace std;
using namespace cv;

CascadeClassifier peopele;
CascadeClassifier cars;
CascadeClassifier half_peopele;
using namespace cv;
/////////////////////
Point getCenterPoint(Rect rect)
{
	Point cpt;
	cpt.x = rect.x + cvRound(rect.width / 2.0);
	cpt.y = rect.y + cvRound(rect.height / 2.0);
	return cpt;
}
///////////////////////

int main()
{
	////////////////////////////////////////////
	peopele.load("C://Users//echo//Desktop//上汽opencv比赛//carpeopelsignal//haarcascade_fullbody.xml");   //加载分类器，注意文件路径  
	cars.load("C://Users//echo//Desktop//上汽opencv比赛//carpeopelsignal//haarcascade_cars2.xml");
	half_peopele.load("C://Users//echo//Desktop//上汽opencv比赛//carpeopelsignal//haarcascade_upperbody.xml");
	//【1】读入视频
	VideoCapture cap;
	//cap.open(0);   //打开摄像头  
	VideoCapture capture("C://Users//echo//Desktop//road6.avi");//road3.avi road4.mp4
	
	
	Point root_points[1][6];                                  //多边形数组根据不同的分辨率选择大小
	root_points[0][0] = Point(0, 0);
	root_points[0][1] = Point(0, 280);
	root_points[0][2] = Point(213, 220);
	root_points[0][3] = Point(426, 220);
	root_points[0][4] = Point(640, 280);
	root_points[0][5] = Point(640, 0);

	Mat dstImage, edge, grayImage;                           //创建与src同类型和大小的矩阵(dst)

	////////////////////////////////////////////
	std::vector<Rect> peopele1;                              //设置用来圈车人的方框
	std::vector<Rect> cars1;
	std::vector<Rect> half_peopele1;
	/////////////////////////////////
	const Point* ppt[1] = { root_points[0] };                //指向多边形的数组指针
	int npt[] = { 6 };                                       //多边形顶点个数的数组

	vector<Vec4i> lines;                                     //定义一个矢量结构lines用于存放得到的线段矢量集合
	Vec4f l = NULL;                                          //计算出的直线信息存放在 line 中，为 cv::Vec4f 类型。
	Vec4f line1 = NULL;
	Vec4f line2 = NULL;

	while (1)                                                //循环显示每一帧
	{
		Mat frame,edge1;                                     //定义一个Mat变量，用于存储每一帧的图像;edge1用来检测Haar
	    capture >> frame;                                    //读取当前帧
	//	cap >> frame;	  
		dstImage.create(frame.size(), frame.type());         //创建与src同类型和大小的矩阵(dst)
		
		cvtColor(frame, grayImage, CV_BGR2GRAY);             //将原图像转换为灰度图像
	
		GaussianBlur(grayImage, edge, Size(5, 5), 0, 0);     //先用使用 5x5内核来降噪
	  //blur(grayImage, edge, Size(5, 5));				     //blur的作用是对输入的图像src进行均值滤波后用dst输出。

//		Canny(grayImage, edge1, 50, 150, 3);                 //Haar检测用的
	 	Canny(edge, edge, 30, 180, 3);                       //运行Canny算子;可以改改阈值现在效果一般
	//////////////////////////////////////
	    cars.detectMultiScale(grayImage, cars1, 1.2, 6, 0, Size(0, 0));                //检测车
		peopele.detectMultiScale(grayImage, peopele1, 1.2, 6, 0, Size(0, 0));           //检测人
	//	eyerCascade.detectMultiScale(grayImage, eyer, 1.2, 6, 0, Size(0, 0));           //检测人上半身
    //////////////////////////////////////////

		//【5】填充多边形
		polylines(edge, ppt, npt, 1, 1, Scalar(255, 0, 0), 1, 8, 0);                    //绘制多边形polylines
		fillPoly(edge, ppt, npt, 1, Scalar(0));                                         //多边形填充fillPoly
		
    	//Rect ROI(0, 100, width / 2, height / 2);                                      //获取ROI

		HoughLinesP(edge, lines, 1, CV_PI / 180, 5, 40, 20);                           //概率霍夫变换来检测直线——HoughLinesP；第五个是阈值默认15

		vector<Point2f> points;                                                         //???????????
		vector<Point2f> points2;

		float avr1 = 0.0;                                                               //????????????
		float avr2 = 0.0;
		
		for (size_t i = 0; i < lines.size(); i++)                                       //依次在图中绘制出每条线段,lines为输出线条
		{
			l = lines[i];                                                               // Vec4f l = NULL;     
			printf("斜率： %f \r\n", (l[3] - l[1]) / (l[2] - l[0]));
			
			if ((l[3] - l[1]) / (l[2] - l[0])>0.5 && (l[3] - l[1]) / (l[2] - l[0])<1)   //求直线斜率；l[3] - l[1]) / (l[2] - l[0])>0.5或者(l[3] - l[1]) / (l[2] - l[0])<1
			{
				avr1 += (l[3] - l[1]) / (l[2] - l[0]);                                  //avrl=avrl+斜率
				printf("--(%f,%f),(%f,%f)\r\n", l[0], l[1], l[2], l[3]);                //
				points.push_back(Point2f(l[0], l[1]));                                  //push_back函数的意思是在vector的末尾插入一个元素。
				points.push_back(Point2f(l[2], l[3]));
			}
			else if ((l[3] - l[1]) / (l[2] - l[0])<-0.5 && (l[3] - l[1]) / (l[2] - l[0])>-1)
			{                                                                           //求右边斜率
				avr2 += (l[3] - l[1]) / (l[2] - l[0]);
				printf("**(%f,%f),(%f,%f)\r\n", l[0], l[1], l[2], l[3]);
				points2.push_back(Point2f(l[0], l[1]));
				points2.push_back(Point2f(l[2], l[3]));
			}
			for (auto i = 0; i < points.size(); i++)                                     // auto自动类型推断
				circle(frame, cvPoint(points.at(i).x, points.at(i).y), 3, Scalar(0, 255, 0));//cvPoint(points.at(i).x, points.at(i).y)取得圆心坐标
			for (auto i = 0; i < points2.size(); i++)
				circle(frame, cvPoint(points2.at(i).x, points2.at(i).y), 3, Scalar(255, 255, 0));//半径为3
			//cv::line(frame, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 1, CV_AA);
		}
		printf("%f,%f \r\n", avr1, avr2);                                                //输出斜率
//////////////////////////////////////////拟合第一个直线和参数设置
		fitLine(Mat(points), line1, CV_DIST_L1, 0, 0.01, 0.01);                          //直线拟合
		double k = line1[1] / line1[0];                                                  //得到斜率？
		double step = 350;                                                               //截距？
		cvtColor(edge, dstImage, CV_GRAY2BGR);                                           //CV_RGB2GRAY：<彩色图像---灰度图像>
		cv::line(dstImage, cvPoint(line1[2] - step, line1[3] - k*step), cvPoint(line1[2] + step, k*step + line1[3]), Scalar(0, 0, 255), 5);//
		//cvPoint(line1[2] - step, line1[3] - k*step)直线起点；cvPoint(line1[2] + step, k*step + line1[3])   直线终点；
		//cv::line(frame, Point(line1[0], line1[1]), Point(line1[2], line1[3]), Scalar(0, 0, 255), 1, CV_AA);
//////////////////////////////////////////拟合第二个
		fitLine(Mat(points2), line2, CV_DIST_L1, 0, 0.01, 0.01);
		k = line2[1] / line2[0];
		cv::line(dstImage, cvPoint(line2[2] - step, line2[3] - k*step), cvPoint(line2[2] + step, k*step + line2[3]), Scalar(0, 0, 255), 5);
		//cv::line(frame, Point(line2[0], line2[1]), Point(line2[2], line2[3]), Scalar(0, 0, 255), 1, CV_AA);
//////////////////////////////////////////
    	fillPoly(dstImage, ppt, npt, 1, Scalar(0));//绘制多边形polylines(ppt);  Scalar(0)  灰度值0为黑1为白；
		
		//cvtColor(edge, dstImage, CV_GRAY2BGR);
		addWeighted(frame, 0.8, dstImage, 1, 0.0, dstImage);//图像叠加，把多边形叠加在原图像上
////////////////////////////////////////用来框出目标
	//	char *strID; //显示名称
	//	CvFont font;
	//	strID = "Car";
		if (cars1.size()>0)
		{
			for (int i = 0; i<cars1.size(); i++)
			{
				rectangle(dstImage, Point(cars1[i].x, cars1[i].y), Point(cars1[i].x + cars1[i].width, cars1[i].y + cars1[i].height),//dstImage or frame?
					Scalar(0, 255, 0), 1, 8);    //框出车辆位置  
				//cvPutText(dstImage, strID, cvPoint(cars1[i].x, cars1[i].y - 10), &font, CV_RGB(255, 0, 0));//红色字体注释 不能用在Mat
			//printf("车辆位置：%Point,%Point \r\n", cars1[i].x+0.5*cars1[i].width, cars1[i].y+0.5*cars1[i].height);
				cout << "车辆位置：" << cars1[i].x + 0.5*cars1[i].width << "  "<< cars1[i].y + 0.5*cars1[i].height<< endl;
			}
		}
		if (peopele1.size()>0)
		{
			for (int i = 0; i<peopele1.size(); i++)
			{
				rectangle(dstImage, Point(peopele1[i].x, peopele1[i].y), Point(peopele1[i].x + peopele1[i].width, peopele1[i].y + peopele1[i].height),//dstImage or frame?
					Scalar(0, 0, 255), 1, 8);    //框出人位置  
				cout << "行人位置：" << peopele1[i].x + 0.5*peopele1[i].width << "  " << peopele1[i].y + 0.5*peopele1[i].height << endl;
			}
		}
////////////////////////////////////////
		imshow("读取视频", dstImage);  //显示视频
    //  imshow("灰度", grayImage);
	//	imshow("边缘", edge);
	//	imshow("边缘1", edge1);
		printf("********%d,%d*******\r\n", points.size(), points2.size());
		waitKey(1);  //延时1ms
	}
	return 0;
}