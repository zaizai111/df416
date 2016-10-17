#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QPainter>
#include <QWidget>
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

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
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

    QImage sourceImage;
    QTextEdit *showResult;

    cv::Mat sourceImg;
};

#endif // MAINWINDOW_H
