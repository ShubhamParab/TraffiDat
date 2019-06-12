#ifndef DETECTION_H
#define DETECTION_H

#include <QObject>
#include <QMap>
#include <QThread>
#include <QThreadPool>
#include <QMutex>
#include <QTimer>
#include <QDebug>
#include <QPointer>
#include <QLabel>
#include <QDialog>
#include <QVBoxLayout>
#include <QListWidget>
#include <QApplication>
#include <QDesktopWidget>

#include "model.h"
#include "constants.h"

//
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <ctime>

// includes for file_exists and files_in_directory functions
#ifndef __linux
#include <io.h>
#define access _access_s
#else
#include <unistd.h>
#include <memory>
#endif

#define POSITIVE_TRAINING_SET_PATH "C:\\Users\\Shubham\\Documents\\final-proj\\model\\DATASET\\POSITIVE"
#define NEGATIVE_TRAINING_SET_PATH "C:\\Users\\Shubham\\Documents\\final-proj\\model\\DATASET\\NEGATIVE"
#define WINDOW_NAME "WINDOW"
#define TRAFFIC_VIDEO_FILE "C:\\Users\\Shubham\\Documents\\final-proj\\model\\video.mp4"
#define TRAINED_SVM "C:\\Users\\Shubham\\Documents\\final-proj\\model\\vehicle_detector.yml"
#define	IMAGE_SIZE Size(40, 40)

// tracking
// OpenCV includes:
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio/videoio.hpp>

#include "tracker/Transformation.hpp"
#include "tracker/Detector.hpp"
#include "tracker/VehicleTracker.hpp"

#define ANALYSIS_WIDTH 320  // target width for frame resize
#define CAPTURE_WIDTH 320   // camera capture width
#define CAPTURE_HEIGHT 240
#define FRAME_BORDER 20
//
#define CONGESTION_COUNT_THRESHOLD 5
#define CONGESTION_SPEED_THRESHOLD 7

//using namespace cv;
using namespace cv::ml;
using namespace std;
//
using namespace TraffiDat;

class RunDetection;
class DetectionDialog;

class Detection : public QObject
{
    Q_OBJECT
public:
    explicit Detection(QMap<qint64, road> rds, QMap<qint64, mut_road*> *mRds, QMap<qint64, float*> *speedRds, QMap<qint64, bool*> *congRds, QObject *parent = nullptr);
    ~Detection();

    //
    bool file_exists(const string &file);
    void load_images(string directory, vector<cv::Mat>& image_list);
    vector<string> files_in_directory(string directory);

    void get_svm_detector(const cv::Ptr<SVM>& svm, vector< float > & hog_detector);
    void convert_to_ml(const std::vector< cv::Mat > & train_samples, cv::Mat& trainData);
    void sample_neg(const vector< cv::Mat > & full_neg_lst, vector< cv::Mat > & neg_lst, const cv::Size & size);
    cv::Mat get_hogdescriptor_visu(const cv::Mat& color_origImg, vector<float>& descriptorValues, const cv::Size & size);
    void compute_hog(const vector<cv:: Mat > & img_lst, vector< cv::Mat > & gradient_lst, const cv::Size & size);
    void train_svm(const vector< cv::Mat > & gradient_lst, const vector< int > & labels);
    static void draw_locations(cv::Mat & img, const vector< cv::Rect > & locations, const cv::Scalar & color);
    void test_it(const cv::Size & size);
    //
    cv::HOGDescriptor getHogDetector();


signals:
    void finish(qint64 road_id);
    void detectInfo(QString msg, qint64 msgType = INFO);

public slots:
    void start();
    void pause();
    void stop();
    void finished(qint64 road_id);
    void onCongestion(qint64 road_id, bool isCongested);
    void scrollTo(qint64 id =0);


private:
    QMap<qint64, road> rds;
    QMap<qint64, mut_road*> *mRds;
    QMap<qint64, float*> *speedRds;
    QMap<qint64, bool*> *congRds;
    QList<RunDetection*> rDetects;
    QList<QLabel*> rImage;
    QPointer<DetectionDialog> dlg;
    QList<QLabel*> speedImage;
    QPointer<DetectionDialog> speedDlg;

    bool fin = false;

};

class RunDetection : public QObject, public QRunnable{
    Q_OBJECT
public:
    explicit RunDetection(road r, cv::HOGDescriptor hog, mut_road *mRd, float* speed, bool* congRd, QLabel* imageLabel = new QLabel(), QLabel* speedLabel = new QLabel(), qint64 width = 320, qint64 height = 240);
    ~RunDetection();
    void run() override;
    void setImageLabel(QLabel* value){imageLabel = value;}
    void setAverageSpeed(vector<float> avgs);
    void checkCongestion(qint64 onScreenCount, float vehicleSpeed);
    void setPlay(qint64 type);
    road getRoad(){return road;}

    int playType = STOP_PLAY;

signals:
    void finish(qint64 road_id);
    void congestion(qint64 road_id, bool isCongested);

private:
    cv::HOGDescriptor hog;
    road road;
    QMutex mutex;
    mut_road *mRd;
    float* speed;
    bool* congRd;
    QPointer<QLabel> imageLabel;
    QPointer<QLabel> speedLabel;
    qint64 mFrameW, mFrameH;

    // tracking
    float mFR = 23;
    bool firstRun = true;
    bool runMovie = true;
    bool doCalibration = false;
    bool firstCalibRun = true;
    float mResizeRatio;
//    std::vector<ci::vec2> mCorners;
    std::vector<cv::Point2f>  mCornersTransCV; // array transformed corner points
    Transformation *transformation;
    Detector *detector;

    VehicleTracker *mVehicleTracker; // process tracks made by detected vehicles
    float mPx2meter; // conversion factor pixels -> meters
    int mNumCars=0; // number of cars detected (total)
    int mNumCurrentCars = 0; // number of currently detected cars
    std::vector<float> mCurrentCarSpeeds; // inst. speeds of current cars
    std::vector<float> mCurrentCarSpeedAverages; // aveaged speeds of current cars
    int nNumCarsRight=0;

    float avgSpeed;
    //

};

class DetectionDialog : public QDialog{
    Q_OBJECT
public:
    explicit DetectionDialog(QList<qint64> ids, QString title = QString("DETECTION"), QWidget* parent = nullptr, qint64 width = 320, qint64 height = 240);
    void buildImageLabels();
    void buildImageList();
    QLabel* getLabelById(int id){return imageLabels.value(id);}
    ~DetectionDialog();

public slots:
    void scrollTo(qint64 id =0);

private:
    QMap<qint64, QLabel*> imageLabels;
    QMap<qint64, QListWidgetItem*> imageWdgItm;
    QList<qint64> ids;
    qint64 width, height;
    QPointer<QListWidget> imageList;

};

#endif // DETECTION_H
