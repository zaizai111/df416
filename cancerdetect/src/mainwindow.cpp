#include <QtWidgets>
#include "mainwindow.h"
#include "breastcancer_predict.h"
#include "cvmatandqimage.h"
#include <cstdio>


static const QSize resultSize(480, 360);
//static const QSize resultSize(960, 720);
extern double sigmas[7];

MainWindow::MainWindow(QWidget *parent):QWidget(parent)
{
//    setMaximumSize(QSize(QApplication::desktop()->width(),QApplication::desktop()->height()));

    //setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint & ~Qt::WindowMinimizeButtonHint);

    QPixmap pixmap("/home/xuguo/projects/cancerdetect/resource/bg3.png");
    QPalette palette;
    palette.setBrush(backgroundRole(), QBrush(pixmap));
    setPalette(palette);

    sourceButton = new QToolButton;
    sourceButton->setIconSize(resultSize);

    gaussButton = new QToolButton;
    gaussButton->setIconSize(resultSize);

    LBPButton_1 = new QToolButton;
    LBPButton_1->setIconSize(resultSize);

//    LBPButton_2 = new QToolButton;
//    LBPButton_2->setIconSize(resultSize);

    QIcon icon;
    icon.addFile(tr("/home/xuguo/projects/cancerdetect/resource/dabai.png"));
    sourceButton->setIcon(icon);
    gaussButton->setIcon(icon);
    LBPButton_1->setIcon(icon);
//    LBPButton_2->setIcon(icon);

    sourceLabel = new QLabel(tr("原图:"));
    gaussLabel = new QLabel(tr("高斯模糊图:"));
    LBPLabel_1 = new QLabel(tr("LBP特征图:"));
    LBPLabel_2 = new QLabel(tr("高斯模糊尺度:"));

//    testLabel = new QLabel(this);
//    testLabel->resize(resultSize);
//    testLabel->setScaledContents(true);

//    QPixmap pixmap2;
//    pixmap2.load("/home/xuguo/projects/cancerdetect/resource/dabai.png");
//    testLabel->setPixmap(pixmap2);

    palette.setColor(QPalette::WindowText,Qt::white);
    sourceLabel->setPalette(palette);
    gaussLabel->setPalette(palette);
    LBPLabel_1->setPalette(palette);
    LBPLabel_2->setPalette(palette);

    calculateButton = new QPushButton(tr("识别"));
    calculateButton->setEnabled(false);

    chooseButton = new QPushButton(tr("选择图片"));
    chooseButton->setEnabled(true);

    showResult = new QTextEdit;
    showResult->setReadOnly(true);

    gaussResult = new QTextEdit;
    gaussResult->setReadOnly(true);

//   timer = new QTimer(this);
//    timer->setInterval(10);
//    timer->start();
//   connect(timer, SIGNAL(timeout()), this, SLOT(changeDisplay()));


//    connect(sourceButton, SIGNAL(clicked()), this, SLOT(chooseSource()));
    connect(chooseButton, SIGNAL(clicked()), this, SLOT(chooseSource()));
    connect(calculateButton, SIGNAL(clicked()), this, SLOT(recalculateResult()));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(sourceLabel, 0, 0, 1, 1);
    mainLayout->addWidget(gaussLabel, 0, 2, 1, 1);
    mainLayout->addWidget(LBPLabel_1, 4, 0, 1, 1);
    mainLayout->addWidget(LBPLabel_2, 4, 2, 1, 1);

    mainLayout->addWidget(sourceButton, 1, 0, 3, 1);
    mainLayout->addWidget(gaussButton, 1, 2, 3, 1);
    mainLayout->addWidget(LBPButton_1, 5, 0, 3, 1);
    mainLayout->addWidget(gaussResult, 5, 2, 3, 1);
    
    mainLayout->addWidget(calculateButton, 1, 4, 1, 1);
    mainLayout->addWidget(chooseButton, 9, 0, 1, 3);
    mainLayout->addWidget(showResult, 2, 4, 8, 1);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout);
//    setWindowTitle(tr("breastcancer predict"));
    setWindowTitle(tr("乳腺癌的预诊断"));
//    setMaximumSize(QSize(QApplication::desktop()->width(),QApplication::desktop()->height()));

    std::cout << "The mainwindow" << std::endl;

}

MainWindow::~MainWindow()
{

}

void MainWindow::chooseSource()
{
    chooseImage(tr("Choose Source Image"), &sourceImage);
}

void MainWindow::chooseImage(const QString &title, QImage *image)
{
    QString fileName = QFileDialog::getOpenFileName(this, title);
    if (!fileName.isEmpty())
        loadImage(fileName, image, sourceButton);
}

void MainWindow::loadImage(const QString &fileName, QImage *image,
                              QToolButton *button)
{
    sourceImg=cv::imread(fileName.toStdString());
    //image->load(fileName);
    QImage temp=QtOcv::mat2Image(sourceImg);
    //cv::imshow("source img",sourceImg);
    //cv::waitKey(0);
    image=&temp;

    // Scale the image to given size
    *image = image->scaled(resultSize, Qt::KeepAspectRatio);

    QImage fixedImage(resultSize, QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&fixedImage);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(fixedImage.rect(), Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(imagePos(*image), *image);
    painter.end();
    button->setIcon(QPixmap::fromImage(fixedImage));

    *image = fixedImage;
    calculateButton->setEnabled(true);
    //recalculateResult();
}

QPoint MainWindow::imagePos(const QImage &image) const
{
    return QPoint((resultSize.width() - image.width()) / 2,
                  (resultSize.height() - image.height()) / 2);
}

cv::Mat QImage2cvMat(QImage image)
{
    cv::Mat mat;
    qDebug() << image.format();
    switch(image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    }
    return mat;
}


#define mp "/home/xuguo/projects/cancerdetect/resource/svm_for_all_LBP_Canny.model" //svm_for_glcm_1.model"
//#define mp "../resource/svm_for_glcm_all_2_linear.model"
//#define store_param "../resource/samples_all_pos_neg.param"
#define store_param "/home/xuguo/projects/cancerdetect/resource/samples_all_pos_neg.param"

void MainWindow::recalculateResult()
{
    showResult->clear();
    gaussResult->clear();

    showResult->setText(tr("doing...\n"));
    //cv::waitKey(100);
    showResult->setFont(QFont("Timers", 18, QFont::Bold));
    gaussResult->setFont(QFont("Timers", 18, QFont::Bold));
    showResult->setTextColor(Qt::red);
    gaussResult->setTextColor(Qt::red);
    //CancerPredictGlcm* mcp;
    //mcp = new CancerPredictGlcm(store_param);
    CancerPredict mcp;
    //cv::Mat img = QImage2cvMat(sourceImage);
    cv::Mat img = sourceImg;
    //cv::imshow("fdfdfd",img);
    //cv::waitKey(10);
    svm_model* model = svm_load_model(mp);
    showResult->setText(tr("加载模型成功!\n"));
    double* result = new double[3];
    result = mcp.predictSample(img,model);

    QIcon gauss_icon, lbp_icon, lbpmap_icon;
    QString res, ref;
    res = QString("采用的高斯模糊尺度如下:\n");

    for(int i = 0; i < 7; i++)
    {
        QString str1 = "/home/xuguo/projects_test/cancerdetect/process/gauss_";
        str1 = str1 + QString::number(i, 7) + QString(".jpg");

        QString str2 = "/home/xuguo/projects_test/cancerdetect/process/lbp_";
        str2 = str2 + QString::number(i, 7) + QString(".jpg");

//        QString str3 = "/home/xuguo/projects_test/cancerdetect/process/lbpmap_";
//        str3 = str3 + QString::number(i, 7) + QString(".jpg");

        res += "尺度" + QString("%1").arg(i+1, 0, 10) + ": " + QString("%1").arg(sigmas[i], 0, 'g', 8) + "\n";
        gaussResult->setText(res);
        //std::cout << "The sigmas is:" << sigmas[i] << std::endl;

        gauss_icon.addFile(str1);
        gaussButton->setIcon(gauss_icon);

        lbp_icon.addFile(str2);
        LBPButton_1->setIcon(lbp_icon);

//        lbpmap_icon.addFile(str3);
//        LBPButton_2->setIcon(lbpmap_icon);

        sleep(1500);
    }

    std::cout << "The result is:" << result[2] << std::endl;

    switch(int(result[2])){
    case -1:
        res = QString("识别结果:") + "恶性" + "\n";
        ref = "无";
        break;
    case 0:
        res = QString("识别结果:") + "拒识" + "\n";
        ref = "无";
        break;
    case 1:
        res = QString("识别结果:") + "良性" + "\n";
        ref = "无";
        break;
    case 2:
        res = QString("识别结果:") + "拒识" + "\n";
        ref = "A级(低度可疑恶性)";
        break;
    case 3:
        res = QString("识别结果:") + "拒识" + "\n";
        ref = "B级(中度可疑恶性)";
        break;
    case 4:
        res = QString("识别结果:") + "拒识" + "\n";
        ref = "C级(高度可疑恶性)";
        break;
    default: std::cout << "Something was wrong!" << std::endl;
        break;
    }

    res += "良性概率:" + QString("%1").arg(result[0], 0, 'g', 5) + "\n";
    res += "恶性概率:" + QString("%1").arg(result[1], 0, 'g', 5) + "\n";
    res += "拒识风险等级:\n" + ref + "\n";

    svm_parameter param = model->param;

//    std::cout << "param.svm_type:" << param.svm_type << std::endl;
//    std::cout << "param.kernel_type:" << param.kernel_type << std::endl;
//    std::cout << "param.degree:" << param.degree << std::endl;
//    std::cout << "param.gamma:" << param.gamma << std::endl;
//    std::cout << "param.coef0:" << param.coef0 << std::endl;
//    std::cout << "param.nu:" << param.nu << std::endl;
//    std::cout << "param.cache_size:" << param.cache_size << std::endl;
//    std::cout << "param.C:" << param.C << std::endl;
//    std::cout << "param.eps:" << param.eps << std::endl;
//    std::cout << "param.p:" << param.p << std::endl;


    switch(param.svm_type){
    case 0:
        res += QString("SVM问题类型:多类别识别问题(C_SVC)") + "\n";
        break;
    case 1:
        res += QString("SVM问题类型:多类别识别问题(NU_SVC)") + "\n";
        break;
    case 2:
        res += QString("SVM问题类型:两类别识别问题(ONE_CLASS)") + "\n";
        break;
    case 3:
        res += QString("SVM问题类型:回归分析(EPSILON_SVR)") + "\n";
        break;
    case 4:
        res += QString("SVM问题类型:回归分析(NU_SVR)") + "\n";
        break;
    default:
        std::cout << "Something was wrong!" << std::endl;
        break;
    }

    switch(param.kernel_type){
    case 0:
        res += QString("核函数类型:线性核函数") + "\n";
        break;
    case 1:
        res += QString("核函数类型:多项式核函数") + "\n";
        break;
    case 2:
        res += QString("核函数类型:高斯核函数") + "\n";
        break;
    case 3:
        res += QString("核函数类型:Sigmoid函数") + "\n";
        break;
    case 4:
        res += QString("核函数类型:自定义核函数") + "\n";
        break;
    default:
        std::cout << "Something was wrong!" << std::endl;
        break;
    }

    // default values
    //res += "svm_type:" + QString("%1").arg(param.svm_type, 0, 10) + "\n";
    //res += "kernel_type:" + QString("%1").arg(param.kernel_type, 0, 10) + "\n";
    //res += "degree:" + QString("%1").arg(param.degree, 0, 10) + "\n";
    //res += "gamma:" + QString("%1").arg(param.gamma, 0, 'g', 3) + "\n";
    //res += "coef0:" + QString("%1").arg(param.coef0, 0, 'g', 3) + "\n";
    //res += "nu:" + QString("%1").arg(param.nu, 0, 'g', 3) + "\n";
    //res += "cache_size:" + QString("%1").arg(param.cache_size, 0, 'g', 3) + "\n";
    res += "惩罚系数: 5\n";
    //res += "eps:" + QString("%1").arg(param.eps, 0, 'g', 3) + "\n";
    //res += "p:" + QString("%1").arg(param.p, 0, 'g', 3) + "\n";

    showResult->setText(res);
}

void MainWindow::sleep(unsigned int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}


void MainWindow::changeDisplay()
{
//    QImage *im_gauss, *im_lbp1, *im_lbp2;
//    QImage im_gauss;

    // Scale the image to given size


//        QImage temp_gauss=QtOcv::mat2Image(image_gauss[i]);

//        im_gauss=&temp_gauss;

//        if(im_gauss == NULL)
//        {
//            std::cout << "gauss is null" << std::endl;
//        }else{
//            std::cout << "gauss not null" << std::endl;
//            std::cout << image_gauss[i] << std::endl;
//            std::cout << "temp_gauss" << std::endl;
//            //std::cout << temp_gauss << std::endl;
//            std::cout << "im_gauss" << std::endl;
//            std::cout << im_gauss << std::endl;
//        }

//        *im_gauss = im_gauss->scaled(resultSize, Qt::KeepAspectRatio);

//        QImage gauss_fixedImage(resultSize, QImage::Format_ARGB32_Premultiplied);

//        QPainter gauss_painter(&gauss_fixedImage);
//        gauss_painter.setCompositionMode(QPainter::CompositionMode_Source);
//        gauss_painter.fillRect(gauss_fixedImage.rect(), Qt::transparent);
//        gauss_painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
//        gauss_painter.drawImage(imagePos(*im_gauss), *im_gauss);
//        gauss_painter.end();
//        gaussButton->setIcon(QPixmap::fromImage(gauss_fixedImage));

//        testLabel->setPixmap(QPixmap::fromImage(gauss_fixedImage));
//        testLabel->show();



//        if(!image_gauss[i].data)
//        {
//            QMessageBox msgBox;
//            msgBox.setText(tr("image data is null"));
//            msgBox.exec();
//        }else{
//            cv::cvtColor(image_gauss[i],image_gauss[i],CV_BGR2RGB);
//            im_gauss = QImage((const unsigned char*)(image_gauss[i].data),image_gauss[i].cols,image_gauss[i].rows,QImage::Format_RGB888);
//            testLabel->clear();
//            testLabel->setPixmap(QPixmap::fromImage(im_gauss));
//            //testLabel->resize(ui->label->pixmap()->size());
//        }


//          cv::Mat rgb;
//          QImage scaledImg;

//          if(image_gauss.channels() == 3)    // RGB image
//          {
//              cvtColor(image_gauss,rgb,CV_BGR2RGB);
//              im_gauss = QImage((const uchar*)(rgb.data),  //(const unsigned char*)
//                           rgb.cols,rgb.rows,
//                           rgb.cols*rgb.channels(),   //解决Mat图像与QImage图像不对齐问题
//                           QImage::Format_RGB888);
//             scaledImg=im_gauss.scaled(testLabel->size(),Qt::IgnoreAspectRatio);
//             std::cout << "1" << std::endl;

//          }else {                     // gray imag
//              im_gauss = QImage((const uchar*)(image_gauss.data),
//                           image_gauss.cols,image_gauss.rows,
//                           image_gauss.cols*image_gauss.channels(),    //解决Mat图像与QImage图像不对齐问题
//                           QImage::Format_Indexed8);
//              scaledImg=im_gauss.scaled(testLabel->size(),Qt::IgnoreAspectRatio);
//              std::cout << "2" << std::endl;
//          }
//          testLabel->setPixmap(QPixmap::fromImage(im_gauss));
//          testLabel->show();

//        QPixmap pixmap3;
//        std::cout << "1" << std::endl;
//        pixmap3.load("/home/xuguo/projects_test/cancerdetect/process/gauss.jpg");
//        std::cout << "2" << std::endl;
//        testLabel->setPixmap(pixmap3);

//        timer->stop();
//        QIcon icon1;
//        icon1.addFile(tr("/home/xuguo/projects_test/cancerdetect/process/gauss.jpg"));
//        gaussButton->setIcon(icon1);

        std::cout << "display" << std::endl;
}





