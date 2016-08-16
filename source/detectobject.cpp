#include "headers/detectobject.h"

using namespace cv;
using namespace std;

CascadeClassifier faceCascade, eyeCascade, eyeGlassesCascade;

detectobject::detectobject()
{
    //init cascades
    const char* faceCascadeFilename = "./cascades/lbpcascade_frontalface.xml";
    const char* eyeCascadeFilename = "./cascades/haarcascade_eye_tree_eyeglasses.xml";
    const char* eyeGlassesCascadeFilename = "./cascades/haarcascade_eye.xml";

    faceCascade.load(faceCascadeFilename);
    eyeCascade.load(eyeCascadeFilename);
    eyeGlassesCascade.load(eyeGlassesCascadeFilename);

    if(faceCascade.empty() || eyeCascade.empty() || eyeGlassesCascade.empty()){
        qDebug() << "error loading cascade files";
        qDebug() << "check filenames and paths";
        return;
    }else{
        qDebug() << "cascades loaded";
    }
}

Mat detectobject::findFace(Mat &image)
{
    //convert image to greyscale
    Mat grey;
    //detect channels and apply appropriate conversion
    if(image.channels() == 3){
        cvtColor(image,grey,CV_BGR2GRAY);
    }else if(image.channels() == 4){
        cvtColor(image,grey, CV_BGRA2GRAY);
    }else{
        grey = image;
    }

    //equalise hist - smooths differences in lighting
    equalizeHist(grey,grey);
    //find larget object in image : face
    vector<Rect> objects;
    Rect faceRect;
    detectlargestobject(grey, faceCascade, objects);

    if(objects.size() > 0){
        //set largest object
        faceRect = objects.at(0);
    }else{
        //set false rect
        faceRect = Rect(-1,-1,-1,-1);
    }

    if (faceRect.width > 0){
        //return face
        Mat face = image(faceRect);
        vector<Point> eyes = findEyes(face);
        Point left = eyes.at(0);
        if(left.x > 0){
            Mat warped = warpImage(face,eyes);
            return warped;
        }
        else
            return Mat();
    }else{
        //return empty Mat
        qDebug() << " no face found";
        return Mat();
    }
}

vector<Point> detectobject::findEyes(Mat &face)
{
    vector<Rect> leftEyeRect, rightEyeRect;
    Rect leftEye,rightEye;
    Point leftEyePos, rightEyePos;
    vector<Point> eyePos;
    //set default position values for cascades
    const float EYE_SX = 0.16f;
    const float EYE_SY = 0.26f;
    const float EYE_SW = 0.30f;
    const float EYE_SH = 0.28f;

    //set the rough bounds of where the eye should be
    int leftX = cvRound(face.cols * EYE_SX);
    int topY = cvRound(face.rows * EYE_SY);
    int widthX = cvRound(face.cols * EYE_SW);
    int heightY = cvRound(face.rows * EYE_SH);
    int rightX = cvRound(face.cols *(1.0 - EYE_SX - EYE_SW));

    Mat topLeftFace = face(Rect(leftX,topY,widthX,heightY));
    Mat topRightFace = face(Rect(rightX, topY,widthX,heightY));

    //search each Mat individually to accurately determine eye pos.
    detectlargestobject(topLeftFace,eyeCascade,leftEyeRect);
    detectlargestobject(topRightFace,eyeCascade,rightEyeRect);

    //check success, if not try again with eye_glasses
    if(leftEyeRect.size() > 0){
        leftEye = leftEyeRect.at(0);
    }else{
        detectlargestobject(topLeftFace,eyeGlassesCascade, leftEyeRect);
        if(leftEyeRect.size() > 0){
            leftEye = leftEyeRect.at(0);
        }else{
            qDebug() << "Could not find left Eye";
            leftEyePos = Point(-1,-1);
            //return;
        }
    }

    if(rightEyeRect.size() > 0){
        rightEye = rightEyeRect.at(0);
    }else{
        detectlargestobject(topRightFace,eyeGlassesCascade, rightEyeRect);
        if(rightEyeRect.size() > 0){
            rightEye = rightEyeRect.at(0);
        }else{
            qDebug() << "Could not find right Eye";
            rightEyePos = Point(-1,-1);
            //return;
        }
    }

    if(leftEye.x > 0 && rightEye.x > 0){
        qDebug() << "got both eyes";

        leftEye.x += leftX;//adjust image
        leftEye.y += topY;
        leftEyePos = Point(leftEye.x + leftEye.width/2, leftEye.y + leftEye.height/2);

        rightEye.x += rightX;
        rightEye.y += topY;
        rightEyePos = Point(rightEye.x + leftEye.width/2, rightEye.y + leftEye.height/2);

    }
    eyePos.push_back(leftEyePos);
    eyePos.push_back(rightEyePos);

    return eyePos;
}

Mat detectobject::warpImage(Mat &face, vector<Point> eyes)
{
    Point leftEye,rightEye;
    Point2f eyesCentre;
    const int faceWidth = 100;
    const int faceHeight = faceWidth;

    leftEye = eyes.at(0);
    rightEye = eyes.at(1);

    //use eye postion to rotate and scale image
    //get centre position
    eyesCentre = Point2f((leftEye.x + rightEye.x) * 0.5f, (leftEye.y + rightEye.y) * 0.5f);

    //get angle
    double dy = (rightEye.y - leftEye.y);
    double dx = (rightEye.x - leftEye.x);
    double len = sqrt(dx*dx + dy*dy);
    double angle = atan2(dy, dx) * 180.0/CV_PI;//convert to degrees

    //hand measured values from other experiments
    //const double DESIRED_LEFT_EYE_X = 0.16;
    const double DESIRED_LEFT_EYE_X = 0.30;
    const double DESIRED_RIGHT_EYE_X = (1.0f - DESIRED_LEFT_EYE_X);

    //get the amount of scaling required
    double desiredLen = (DESIRED_RIGHT_EYE_X - DESIRED_LEFT_EYE_X) * faceWidth;
    double scale = desiredLen / len;


    //get rotation matrix
    Mat rot_mat = getRotationMatrix2D(eyesCentre, angle,scale);
    rot_mat.at<double>(0,2) += (faceWidth * 0.5f - eyesCentre.x);
    rot_mat.at<double>(1,2) += (faceHeight *DESIRED_LEFT_EYE_X - eyesCentre.y);

    //perform the transform
    Mat warped = Mat(faceHeight, faceWidth, CV_8U, Scalar(128));
    warpAffine(face,warped,rot_mat, warped.size());

    //apply mask around edges
    Mat mask = Mat(warped.size(), CV_8U, Scalar(0));
    Point faceCentre = Point(faceWidth/2, cvRound(faceHeight*0.40));
    Size size = Size(cvRound(faceWidth * 0.5), cvRound(faceHeight * 0.8));
    ellipse(mask, faceCentre, size, 0, 0, 360, Scalar(255), CV_FILLED);;
    Mat dstImg = Mat(warped.size(), CV_8U, Scalar(128));
    warped.copyTo(dstImg,mask);


    return dstImg;
}

void detectobject::detectlargestobject(Mat &image, CascadeClassifier &cascade, vector<Rect> &objects)
{
    //set flags for single object detection
    int flags = CASCADE_FIND_BIGGEST_OBJECT;
    //vector<Rect> objects;
    /*set detection parameters:
        -min size
        -search detail
        -false detection threshold
    */
    Size minFeatureSize = Size(20,20);
    float searchScaleFactor = 1.1f;
    int minNeighbours = 6;

    //opencv obj detect function
    cascade.detectMultiScale(image,objects,searchScaleFactor,minNeighbours, flags, minFeatureSize);
    return;
}







