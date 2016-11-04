#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QPainter>
#include <QWidget>
#include <QTimer>
#include <QTime>
#include <opencv2/opencv.hpp>

QT_BEGIN_NAMESPACE
class QComboBox;
class QLabel;
class QToolButton;
class QPushButton;
class QTextEdit;
QT_END_NAMESPACE
class MainWindow : public QWidget
{
    Q_OBJECT

private slots:
    void chooseSource();
    void recalculateResult();
    void changeDisplay();

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QTimer *timer;
    //void changeDisplay(cv::Mat image_gauss, cv::Mat image_lbp1, cv::Mat image_lbp2);
    //void changeDisplay();

private:
    void sleep(unsigned int msec);
    void chooseImage(const QString &title, QImage *image);
    void loadImage(const QString &fileName, QImage *image, QToolButton *button);
    QPoint imagePos(const QImage &image) const;

    QToolButton *sourceButton;
    QToolButton *gaussButton;
    QToolButton *LBPButton_1;
    QToolButton *LBPButton_2;
    QPushButton *calculateButton;
    QPushButton *chooseButton;

    QLabel *sourceLabel;
    QLabel *gaussLabel;
    QLabel *LBPLabel_1;
    QLabel *LBPLabel_2;

    QLabel *testLabel;

    QImage sourceImage;
    QTextEdit *showResult;
    QTextEdit *gaussResult;

    cv::Mat sourceImg;
};

#endif // MAINWINDOW_H
