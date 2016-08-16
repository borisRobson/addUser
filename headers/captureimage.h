#ifndef CAPTUREIMAGE_H
#define CAPTUREIMAGE_H

#include "opencv2/opencv.hpp"
#include <vector>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <QtCore>
#include <QTimer>

using namespace cv;
using namespace std;


class captureimage
{
public:
    captureimage();
    ~captureimage();
    void showimage();
    void begincapture(VideoCapture &cap);

};

class imageWriter : public QObject
{
    Q_OBJECT
public:
    imageWriter();
    ~imageWriter();
    void writeImage(Mat &image);
public slots:
    void imageSent(int exitCode, QProcess::ExitStatus exitStatus);

};

#endif // CAPTUREIMAGE_H
