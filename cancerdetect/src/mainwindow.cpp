#include <QtWidgets>
#include "mainwindow.h"
#include "breastcancer_predict.h"
#include "cvmatandqimage.h"
#include <cstdio>


static const QSize resultSize(480, 360);
//static const QSize resultSize(960, 720);

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

    LBPButton_2 = new QToolButton;
    LBPButton_2->setIconSize(resultSize);

    QIcon icon;
    icon.addFile(tr("/home/xuguo/projects/cancerdetect/resource/dabai.png"));
    sourceButton->setIcon(icon);
    gaussButton->setIcon(icon);
    LBPButton_1->setIcon(icon);
    LBPButton_2->setIcon(icon);

    sourceLabel = new QLabel(tr("原图:"));
    gaussLabel = new QLabel(tr("高斯模糊图:"));
    LBPLabel_1 = new QLabel(tr("LBP特征图:"));
    LBPLabel_2 = new QLabel(tr("LBP映射图:"));

    palette.setColor(QPalette::WindowText,Qt::white);
    sourceLabel->setPalette(palette);
    gaussLabel->setPalette(palette);
    LBPLabel_1->setPalette(palette);
    LBPLabel_2->setPalette(palette);

    calculateButton = new QPushButton(tr("识别"));
    calculateButton->setEnabled(false);

    chooseButton = new QPushButton(tr("选择图片"));
    chooseButton->setEnabled(true);
//    chooseButton->setStyleSheet("QPushButton{color:blue；background:white}");

    showResult = new QTextEdit;
    showResult->setReadOnly(true);

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
    mainLayout->addWidget(LBPButton_2, 5, 2, 3, 1);
    
    mainLayout->addWidget(calculateButton, 1, 4, 1, 1);
    mainLayout->addWidget(chooseButton, 9, 0, 1, 3);
    mainLayout->addWidget(showResult, 2, 4, 8, 1);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout);
//    setWindowTitle(tr("breastcancer predict"));
    setWindowTitle(tr("乳腺癌的预诊断"));
    setMaximumSize(QSize(QApplication::desktop()->width(),QApplication::desktop()->height()));

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

    showResult->setText(tr("doing...\n"));
    //cv::waitKey(100);
    showResult->setFont(QFont( "Timers" , 18 ,  QFont::Bold));
    //CancerPredictGlcm* mcp;
    //mcp=new CancerPredictGlcm(store_param);
    CancerPredict mcp;
    //cv::Mat img=QImage2cvMat(sourceImage);
    cv::Mat img=sourceImg;
    //cv::imshow("fdfdfd",img);
    //cv::waitKey(10);
    svm_model* model=svm_load_model(mp);
    showResult->setText(tr("load model success!\n"));
    double* result = new double[3];
    result = mcp.predictSample(img,model);

    std::cout << "The result is:" << result[2] << std::endl;

//    char str[10]={0};
//   sprintf(str,"%lf",result);
//    printf("\n%s\n",str);
    QString res;
    if(result[2]<0){
        res=QString("识别结果:")+"恶性"+"\n";
    }
    else if(result[2]=0){
        res=QString("识别结果:")+"拒识"+"\n";
    }
    else{
        res=QString("识别结果:")+"良性"+"\n";
    }

    //res+="model info:\n";
    //svm_model* model=svm_load_model(mp);
    //svm_parameter param=model->param;
    // default values
//    res+="svm_type:C_SVC\n";
//    res+="kernel_type = LINEAR\n";
//    res+="degree = 3\n";
//    res+="gamma = 1.0/1188\n";
//    res+="coef0 = 0\n";
//    res+="nu = 0.5\n";
//    res+="cache_size = 100\n";
//    res+="C = 5\n";
//    res+="eps = 1e-3\n";
//    res+="p = 0.1\n";
    res+="良性概率:" + QString("%1").arg( result[0], 0, 'g', 5 ) + "\n";
    res+="恶性概率:" + QString("%1").arg( result[1], 0, 'g', 5 ) + "\n";
    res+="拒识风险等级:0\n";

    showResult->setText(res);
    //showResult->setFont(QFont( "Timers" , 18 ,  QFont::Bold));
}





