#include "headers/captureimage.h"
#include "headers/detectobject.h"
#include <unistd.h>
#include <time.h>
using namespace cv;
using namespace std;

detectobject* obj;
imageWriter *imgWrite;
string name = "";

captureimage::captureimage()
{
    obj = new detectobject();
}

captureimage::~captureimage()
{
}

void captureimage::showimage()
{
    //open webcam
    VideoCapture cap(0);
    if(!cap.isOpened()) //check opened succesfully
        return;

    cout << "Enter user name: " << endl;
    cin >> name;

    Mat frame;    
    while(true){
        if(name.empty()){
            cout << "Enter user name: " << endl;
            cin >> name;
        }
        //write new image to frame
        cap >> frame;
        //show the image
        imshow("frame", frame);        
        if(waitKey(15) == 27)   //if 'esc' key pressed
            return;
        else if(waitKey(15) == 32){ //if 'space' pressed
            //capture frame and detect face
            begincapture(cap);
        }
    }
}

void captureimage::begincapture(VideoCapture &cap)
{

    struct timespec tim,tim2;
    tim.tv_sec=0;
    tim.tv_nsec= 10000000L;

    imgWrite = new imageWriter();

    while(true){

        Mat frame;
        cap >> frame;
        imshow("frame", frame);

        while(nanosleep(&tim,&tim2) != 0){;}

        Mat face = obj->findFace(frame);
        if(!face.empty()){
            imshow("face", face);
            cout << "Use this image? <y/n>" << endl;
            char c = waitKey(0);
            if(c == 121 || c == 89){
                imgWrite->writeImage(face);
                break;
            }else if(c == 110 || c == 78){
                cout << "discarding image" << endl;
                face = Mat();
                cvDestroyWindow("face");
                break;
            }
        }        
    }
    return;
}

imageWriter::imageWriter()
{
}

imageWriter::~imageWriter()
{
}

void imageWriter::writeImage(Mat &image)
{
    resize(image,image ,Size(200,200));
    //set image write params
    vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);

    //set filename and write image
    string filename = "./faces/" +name + ".png";
    imwrite(filename,image,compression_params);

    //busybox to panel
    QProcess *sendImage = new QProcess(0);
    connect(sendImage, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(imageSent(int,QProcess::ExitStatus)));
    string cmd = "busybox tftp -p -l " + filename + " -r " + name + ".png fe80::20b:d6ff:fe3e:55f9%eth0";
    const char *charCmd = cmd.c_str();
    QString sendImageCmd = QString(charCmd);
    sendImage->start(sendImageCmd);
}

void imageWriter::imageSent(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);
    qDebug() << "image sent";
    name.clear();
    cvDestroyAllWindows();
}



