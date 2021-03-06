﻿#define tpl "/home/xuguo/projects/cancerdetect/resource/breast_cancer/lists/train_norm_pos.lst"  //pos_all_train.lst" //
#define tnl "/home/xuguo/projects/cancerdetect/resource/breast_cancer/lists/train_norm_neg.lst"   //neg_all_train.lst" //
//#define mp "/home/xuguo/projects/cancerdetect/resource/svm_for_glcm_all_3_linear.model" //svm_for_glcm_1.model"
//#define vpl "/home/xuguo/ClionProjects/breast_concer_detection/resource/validation_norm_pos.lst" //pos_all_test.lst" //
//#define vnl "/home/xuguo/ClionProjects/breast_concer_detection/resource/validation_norm_neg.lst"  //neg_all_test.lst" //
#define vpl "/home/xuguo/projects/cancerdetect/resource/samples/pos_all.lst"
#define vnl "/home/xuguo/projects/cancerdetect/resource/samples/neg_all.lst"

#define norm_file "/home/xuguo/projects/cancerdetect/resource/samples__all_neg_pos.waitForScale"

#define scale_train_file "/homexuguo/projects/cancerdetect/resource/scale_data_train_all.dat"
#define scale_test_file "/home/xuguo/projects/cancerdetect/resource/scale_data_test_all.dat"

#define store_param "/home/xuguo/projects/cancerdetect/resource/samples_all_pos_neg.param"

//#define myGLCM
//#define myNORM
//#define myLBP
//#define myTEST
//#define QTdialog
#define myGUI

#ifdef myLBP
#include <iostream>
#include "breastcancer_predict.h"
#include "GLCM.h"
using namespace std;
#define test_pos
#define test_neg
#define train
int main() {
    CancerPredict mcp;
    #ifdef train
    mcp.trainModel("../resource/breast_cancer/lists/train_norm_pos.lst",
               "../resource/breast_cancer/lists/train_norm_neg.lst",
               "../resource/svm_for_all_LBP_Canny_2.model");
    #endif
    #ifdef test_pos
    mcp.testModel("../resource/breast_cancer/lists/validation_norm_pos.lst","../resource/svm_for_all_LBP_Canny_2.model",1.0);
    #endif
    #ifdef test_neg
    mcp.testModel("../resource/breast_cancer/lists/validation_norm_neg.lst","../resource/svm_for_all_LBP_Canny_2.model",-1.0);
    #endif

    return 0;
}
#endif

#ifdef myGLCM
#include "predict_glcm.h"
#define test_pos
#define test_neg
#define train
int main(){
    CancerPredictGlcm* mcp;
    mcp=new CancerPredictGlcm(store_param);
#ifdef train
    mcp->trainModel(vpl,
                   vnl,
                   mp);
#endif
#ifdef test_pos
    mcp->testModel(vpl,mp,1.0);
#endif
#ifdef test_neg
    mcp->testModel(vnl,mp,-1.0);
#endif
    return 0;
}
#endif

#ifdef myNORM

#include <fstream>
#include "normData.h"
#include "mysvmtrain.h"
#include <string>
int main(){
    nd::normData* mynorm;
    mynorm=new nd::normData();
    //mynorm->start(vpl,vnl,norm_file);

    std::ifstream fp("/home/xuguo/projects/cancerdetect/resource/scale_samples_all_pos_neg.dat");
    std::ofstream fp_t(scale_train_file);
    std::ofstream fp_v(scale_test_file);
    if (!fp.is_open()|!fp_t.is_open()|!fp_v.is_open()) {
        std::cerr << "can not read the image path text file." << std::endl;
        return 1;
    }
    cv::string temp;
    int i=0;
    for(;i<0;i++){

        getline(fp,temp);
        fp_v<<temp<<std::endl;
    }
    for(;i<2700;i++){ //2450
        getline(fp,temp);
        fp_t<<temp<<std::endl;
    }
    for(;i<0;i++){//2753

        getline(fp,temp);
        fp_v<<temp<<std::endl;
    }
    fp.close();
    fp_t.close();
    fp_v.close();
    mytrain::mytrain(scale_train_file,mp,scale_train_file);//scale_test_file
    return 0;
}
#endif
#ifdef myTEST
#include "predict_glcm.h"
int main(int argc,char **argv){
    if(argc!=4){
        std::cout<<"param1: param file path\nparam2: image file path\nparam3:model file\n";
        return 0;
    }
    CancerPredictGlcm* mcp;
    mcp=new CancerPredictGlcm(argv[1]);
    cv::Mat img=cv::imread(argv[2]);
    std::cout<<"The result is:"<<mcp->predictSample(img,argv[3]);
    return 0;
}
#endif

#ifdef QTdialog

#include <QFileDialog>

#include <QApplication>
#include <iostream>

#include "predict_glcm.h"

int main(int argc,char** argv){

    QApplication a(argc,argv);
    QString file_name = QFileDialog::getOpenFileName(NULL, //parent moudle
                                                     QObject::tr("Open File"), //dialog title
                                                     "/home/xuguo/projects/cancerdetect/resource", //the init directory
                                                     "JPEG Files(*.jpg);;PNG Files(*.png)",
                                                     0);

    if (!file_name.isNull())
    {
        std::cout<<file_name.toStdString()<<std::endl;
        CancerPredictGlcm* mcp;
        mcp=new CancerPredictGlcm(store_param);
        cv::Mat img=cv::imread(file_name.toStdString());
        cv::imshow("fdfdfd",img);
        cv::waitKey(10);
        std::cout<<"The result is:"<<mcp->predictSample(img,mp);

    }
    else{
        std::cout<<"you have choose cancel;"<<std::endl;

    }

    return  0;
}

#endif

#ifdef myGUI
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //    w.setWindowState(Qt::WindowMaximized);

    //    w.showFullScreen();

    w.show();
    //    w.showMaximized();

        return a.exec();
}

#endif















