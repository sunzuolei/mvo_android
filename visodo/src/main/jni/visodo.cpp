/*

 The MIT License

 Copyright (c) 2015 Avi Singh

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

 */

#include "vo_features.h"
#include <visodo.h>
#include <stdio.h>
#include <android/log.h>

using namespace cv;
using namespace std;

#define MAX_FRAME 20000
#define MIN_NUM_FEAT 2000
#define LOG_TAG "VisodoCPP"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))
Mat R_f, t_f;
double focal = 718.8560;
cv::Point2d pp(607.1928, 185.2157);
Mat E, R, t, mask;
Mat prevImage;
Mat currImage;
vector<Point2f> prevFeatures;

extern "C" JNIEXPORT jstring JNICALL Java_com_example_visodo_LibVisodo_init(JNIEnv *jenv,
                                                                            jclass, jlong firstPic,
                                                                            jlong secondPic,
                                                                            jboolean isFromCamera) {
    Mat img_1, img_2, img_1_c, img_2_c;
    ofstream myfile;
    myfile.open("results1_1.txt");
    char filename1[200];
    char filename2[200];
    sprintf(filename1, "/storage/emulated/0/image_2/%06d.png", 0);
    sprintf(filename2, "/storage/emulated/0/image_2/%06d.png", 1);

    if (isFromCamera) {
        img_1_c = *(Mat *) firstPic;
        img_2_c = *(Mat *) secondPic;
    } else {
        //read the first two frames from the dataset
        img_1_c = imread(filename1);
        img_2_c = imread(filename2);
    }

    if (!img_1_c.data || !img_2_c.data) {
        //	    std::cout<< " --(!) Error reading images " << std::endl;
        LOGE("Error reading images");
        return jenv->NewStringUTF("Error reading images");
    }
    // we work with grayscale images
    cvtColor(img_1_c, img_1, COLOR_BGR2GRAY);
    cvtColor(img_2_c, img_2, COLOR_BGR2GRAY);

    // feature detection, tracking
    vector<Point2f> points1, points2; //vectors to store the coordinates of the feature points
    featureDetection(img_1, points1); //detect features in img_1
    vector<uchar> status;
    featureTracking(img_1, img_2, points1, points2, status); //track those features to img_2

    E = findEssentialMat(points2, points1, focal, pp, RANSAC, 0.999, 1.0, mask);
    recoverPose(E, points2, points1, R, t, focal, pp, mask);

    prevImage = img_2;
    prevFeatures = points2;

    R_f = R.clone();
    t_f = t.clone();

    clock_t begin = clock();

    LOGI("Init success");
    return jenv->NewStringUTF("Init success");
}

// IMP: Change the file directories (4 places) according to where your dataset is saved before running!
extern "C" JNIEXPORT jdoubleArray JNICALL Java_com_example_visodo_LibVisodo_start(
        JNIEnv *jenv, jclass, jlong addrRgba, jlong afterPic, jint i,
        jdouble xx, jdouble yy, jdouble zz, jboolean isFromCamera) {
    Mat &traj = *(Mat *) addrRgba;

    double scale = 1.00;

    char text[100];
    char second[100];
    int fontFace = FONT_HERSHEY_PLAIN;
    double fontScale = 2;
    int thickness = 2;
    cv::Point textOrg(10, 50);
    jdoubleArray result;
    result = jenv->NewDoubleArray(3);

    vector<Point2f> currFeatures;

    char filename[100];
    sprintf(filename, "/storage/emulated/0/image_2/%06d.png", i);
    Mat currImage_c;
    if (isFromCamera) {
        currImage_c = *(Mat *) afterPic;
    } else {
        currImage_c = imread(filename);
    }
    cvtColor(currImage_c, currImage, COLOR_BGR2GRAY);
    vector<uchar> status;
    featureTracking(prevImage, currImage, prevFeatures, currFeatures, status);

    E = findEssentialMat(currFeatures, prevFeatures, focal, pp, RANSAC, 0.999, 1.0, mask);
    recoverPose(E, currFeatures, prevFeatures, R, t, focal, pp, mask);

    Mat prevPts(2, prevFeatures.size(), CV_64F), currPts(2, currFeatures.size(), CV_64F);

    //this (x,y) combination makes sense as observed from the source code of triangulatePoints on GitHub
    for (unsigned int i = 0; i < prevFeatures.size(); i++) {
        prevPts.at<double>(0, i) = prevFeatures.at(i).x;
        prevPts.at<double>(1, i) = prevFeatures.at(i).y;

        currPts.at<double>(0, i) = currFeatures.at(i).x;
        currPts.at<double>(1, i) = currFeatures.at(i).y;
    }

    if (isFromCamera) {
        scale = 12;
    } else {
        scale = 6; //getAbsoluteScale(numFrame, 0, t.at<double>(2));
    }

    if ((scale > 0.1) && (t.at<double>(2) > t.at<double>(0)) &&
        (t.at<double>(2) > t.at<double>(1))) {

        t_f = t_f + scale * (R_f * t);
        R_f = R * R_f;
    }

    if (prevFeatures.size() < MIN_NUM_FEAT) {
        featureDetection(prevImage, prevFeatures);
        featureTracking(prevImage, currImage, prevFeatures, currFeatures, status);
    }
    prevImage = currImage.clone();
    prevFeatures = currFeatures;

    int x = int(t_f.at<double>(0)) + 300;
    int y = int(t_f.at<double>(2)) + 100;
    if (isFromCamera) {
        circle(traj, Point(x, y), 1, CV_RGB(0, 0, 255), 10);
    } else {
        circle(traj, Point(x, y), 1, CV_RGB(0, 0, 255), 5);
    }
    rectangle(traj, Point(10, 30), Point(550, 50), CV_RGB(255, 255, 255), cv::FILLED);
    xx = t_f.at<double>(0);
    yy = t_f.at<double>(1);
    zz = t_f.at<double>(2);
    jdouble *buf;
    buf = new double[3];
    buf[0] = xx;
    buf[1] = yy;
    buf[2] = zz;
    jenv->SetDoubleArrayRegion(result, 0, 3, buf);

    clock_t end = clock();

    return result;
}

extern "C" JNIEXPORT void JNICALL JNICALL Java_com_example_visodo_LibVisodo_FindFeatures(
        JNIEnv *, jclass clazz, jlong addrRgba) {
    Mat mGr;
    Mat &mRgb = *(Mat *) addrRgba;
    cvtColor(mRgb, mGr, COLOR_BGR2GRAY);
    vector<KeyPoint> v;

    Ptr<FeatureDetector> detector = FastFeatureDetector::create(50);
    detector->detect(mGr, v);
    for (unsigned int i = 0; i < v.size(); i++) {
        const KeyPoint &kp = v[i];
        circle(mRgb, Point(kp.pt.x, kp.pt.y), 6, Scalar(0, 255, 0, 255));
    }
}

//double getAbsoluteScale(int frame_id, int sequence_id, double z_cal)	{
//
//  string line;
//  int i = 0;
//  ifstream myfile ("/home/avisingh/Datasets/KITTI_VO/00.txt");
//  double x =0, y=0, z = 0;
//  double x_prev, y_prev, z_prev;
//  if (myfile.is_open())
//  {
//    while (( getline (myfile,line) ) && (i<=frame_id))
//    {
//      z_prev = z;
//      x_prev = x;
//      y_prev = y;
//      std::istringstream in(line);
//      //cout << line << '\n';
//      for (int j=0; j<12; j++)  {
//        in >> z ;
//        if (j==7) y=z;
//        if (j==3)  x=z;
//      }
//
//      i++;
//    }
//    myfile.close();
//  }
//
//  else {
//    cout << "Unable to open file";
//    return 0;
//  }
//
//  return sqrt((x-x_prev)*(x-x_prev) + (y-y_prev)*(y-y_prev) + (z-z_prev)*(z-z_prev)) ;
//
//}

