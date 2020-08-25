//
// Created by Noweaj.dev on 2020-07-21.
//

#include <jni.h>
#include "com_noweaj_android_pupildetection_core_opencv_OpencvNative.h"

#include <opencv2/opencv.hpp>

#include <android/log.h>

using namespace cv;
using namespace std;

extern "C"{

    JNIEXPORT
    void JNICALL Java_com_noweaj_android_pupildetection_core_opencv_OpencvNative_ConvertRGBtoGray
    (JNIEnv *env, jobject instance, jlong matAddrInput, jlong matAddrResult){
//        Mat &matInput = *(Mat *)matAddrInput;
//        Mat &matResult = *(Mat *)matAddrResult;
        cvtColor(*(Mat *)matAddrInput, *(Mat *)matAddrResult, COLOR_RGBA2GRAY);
    }

    JNIEXPORT
    jlong JNICALL Java_com_noweaj_android_pupildetection_core_opencv_OpencvNative_LoadCascade
    (JNIEnv *env, jobject instance, jstring filePath){
        const char *filePathString = env->GetStringUTFChars(filePath, 0);

        jlong ret = 0;
        ret = (jlong) new CascadeClassifier(filePathString);
        if(((CascadeClassifier *) ret) -> empty()){
            __android_log_print(ANDROID_LOG_DEBUG, "native-lib :: ", "CascasdeClassifier loading failed %s", filePathString);
            ret = 0;
        } else {
            __android_log_print(ANDROID_LOG_DEBUG, "native-lib :: ", "CascasdeClassifier loading successful %s", filePathString);
        }

        env->ReleaseStringUTFChars(filePath, filePathString);
        return ret;
    }

    JNIEXPORT void JNICALL Java_com_noweaj_android_pupildetection_core_opencv_OpencvNative_DetectFrontalFace
    (JNIEnv *env, jobject instance, jlong cascade_face, jlong matAddrInput, jlong matAddrResult){
        Mat &img_result = *(Mat *)matAddrResult;

        std::vector<Rect> faces;

        int minWidth = img_result.cols/3;

        ((CascadeClassifier *) cascade_face)->detectMultiScale(img_result, faces, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(minWidth, 30));
        __android_log_print(ANDROID_LOG_DEBUG, "native-lib :: ", "face %d found", faces.size());

        for(int i=0; i<faces.size(); i++){
            double face_width = faces[i].width;
            double face_height = faces[i].height;

            double face_x = faces[i].x;
            double face_y = faces[i].y;

            Point center(face_x+face_width/2, face_y+face_height/2);
            ellipse(img_result, center, Size(face_width/2, face_height/2), 0, 0, 360, Scalar(255, 0, 255), 20, 8, 0);
        }
    }

    JNIEXPORT void JNICALL Java_com_noweaj_android_pupildetection_core_opencv_OpencvNative_DetectEyes
    (JNIEnv *env, jobject instance, jlong cascade_face, jlong cascade_eye, jlong matAddrInput, jlong matAddrResult){

        // Detect face
        Mat &img_result = *(Mat *)matAddrResult;

        std::vector<Rect> faces;

        int minWidth = img_result.cols/3;

        ((CascadeClassifier *) cascade_face)->detectMultiScale(img_result, faces, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(minWidth, 30));
//        __android_log_print(ANDROID_LOG_DEBUG, "native-lib :: ", "face %d found", faces.size());

        for(int i=0; i<faces.size(); i++){
            double face_width = faces[i].width;
            double face_height = faces[i].height;

            double face_x = faces[i].x;
            double face_y = faces[i].y;

            Point center(face_x+face_width/2, face_y+face_height/2);
            ellipse(img_result, center, Size(face_width/2, face_height/2), 0, 0, 360, Scalar(255, 0, 255), 20, 8, 0);

            // Detect eyes
            Rect face_area(face_x, face_y, face_width, face_height);
            Mat faceROI = img_result(face_area);
            std::vector<Rect> eyes;

            Point face_center(face_x+((face_width - face_x)/2), face_y+((face_height - face_y)/2));

            ((CascadeClassifier *) cascade_eye)->detectMultiScale(faceROI, eyes, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(30, 30));
            __android_log_print(ANDROID_LOG_DEBUG, "native-lib :: ", "============================");
            for(int j=0; j<eyes.size(); j++){
                Point eye_center(face_x+eyes[j].x+eyes[j].width/2, face_y+eyes[j].y+eyes[j].height/2);
                __android_log_print(ANDROID_LOG_DEBUG, "native-lib :: ", "eye point: %lf %lf", face_x+eyes[j].x+eyes[j].width/2, face_y+eyes[j].y+eyes[j].height/2);
                if(eye_center.y < face_center.y) {
                    int radius = cvRound((eyes[j].width + eyes[j].height) * 0.25);
                    circle(img_result, eye_center, radius, Scalar(255, 0, 255), 20, 8, 0);
//                    circle(img_result, Point(0, 0), radius, Scalar(255, 0, 255), 20, 8, 0);
                }
            }
        }
    }

    JNIEXPORT void JNICALL Java_com_noweaj_android_pupildetection_core_opencv_OpencvNative_DetectPupil
    (JNIEnv *env, jobject instance, jlong cascade_face, jlong cascade_eye, jlong matAddrInput, jlong matAddrResult){

    }
}
