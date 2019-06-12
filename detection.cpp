#include "detection.h"

using namespace cv;

Detection::Detection(QMap<qint64, road> rds, QMap<qint64, mut_road*> *mRds, QMap<int64, float*> *speedRds, QMap<qint64, bool*> *congRds, QObject *parent) :
    QObject (parent),
    rds(rds),
    mRds(mRds),
    speedRds(speedRds),
    congRds(congRds)
{
    //
    if (!file_exists(TRAINED_SVM)) {

        vector< Mat > pos_lst;
        vector< Mat > full_neg_lst;
        vector< Mat > neg_lst;
        vector< Mat > gradient_lst;
        vector< int > labels;

        load_images(POSITIVE_TRAINING_SET_PATH, pos_lst);
        labels.assign(pos_lst.size(), +1);

        load_images(NEGATIVE_TRAINING_SET_PATH, full_neg_lst);
        labels.insert(labels.end(), full_neg_lst.size(), -1);

        compute_hog(pos_lst, gradient_lst, IMAGE_SIZE);
        compute_hog(full_neg_lst, gradient_lst, IMAGE_SIZE);

        train_svm(gradient_lst, labels);
    }
    // test_it(IMAGE_SIZE);
    //
    dlg = new DetectionDialog(rds.keys(), QString("Vehicle Count: "));
    speedDlg = new DetectionDialog(rds.keys(), QString("Vehicle Speed"));
}

Detection::~Detection()
{
//    if(mRds != nullptr)
//        delete mRds;
//    qDeleteAll(rDetects);
}

bool Detection::file_exists(const string &file)
{
    return access(file.c_str(), 0) == 0;
}

void Detection::load_images(string directory, vector<Mat> &image_list)
{
    Mat img;
    vector<string> files;
    files = files_in_directory(directory);

    for (int i = 0; i < files.size(); ++i) {

        img = imread(files.at(i));
        if (img.empty())
            continue;
#ifdef _DEBUG
        imshow("image", img);
        waitKey(10);
#endif
        resize(img, img, IMAGE_SIZE);
        image_list.push_back(img.clone());
    }
}

vector<string> Detection::files_in_directory(string directory)
{
    vector<string> files;
    char buf[256];
    string command;

#ifdef __linux__
    command = "ls " + directory;
    shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);

    char cwd[256];
    getcwd(cwd, sizeof(cwd));

    while (!feof(pipe.get()))
        if (fgets(buf, 256, pipe.get()) != NULL) {
            string file(cwd);
            file.append("/");
            file.append(buf);
            file.pop_back();
            files.push_back(file);
        }
#else
    command = "dir /b /s " + directory;
    FILE* pipe = NULL;

    if (pipe = _popen(command.c_str(), "rt"))
        while (!feof(pipe))
            if (fgets(buf, 256, pipe) != NULL) {
                string file(buf);
                file.pop_back();
                files.push_back(file);
            }
    _pclose(pipe);
#endif

    return files;
}

void Detection::get_svm_detector(const Ptr<SVM> &svm, vector<float> &hog_detector)
{
    // get the support vectors
    Mat sv = svm->getSupportVectors();
    const int sv_total = sv.rows;
    // get the decision function
    Mat alpha, svidx;
    double rho = svm->getDecisionFunction(0, alpha, svidx);

    CV_Assert(alpha.total() == 1 && svidx.total() == 1 && sv_total == 1);
    CV_Assert((alpha.type() == CV_64F && alpha.at<double>(0) == 1.) ||
        (alpha.type() == CV_32F && alpha.at<float>(0) == 1.f));
    CV_Assert(sv.type() == CV_32F);
    hog_detector.clear();

    hog_detector.resize(sv.cols + 1);
    memcpy(&hog_detector[0], sv.ptr(), sv.cols*sizeof(hog_detector[0]));
    hog_detector[sv.cols] = (float)-rho;
}

void Detection::convert_to_ml(const std::vector<Mat> &train_samples, Mat &trainData)
{
    //--Convert data
    const int rows = (int)train_samples.size();
    const int cols = (int)std::max(train_samples[0].cols, train_samples[0].rows);
    cv::Mat tmp(1, cols, CV_32FC1); //< used for transposition if needed
    trainData = cv::Mat(rows, cols, CV_32FC1);
    vector< Mat >::const_iterator itr = train_samples.begin();
    vector< Mat >::const_iterator end = train_samples.end();
    for (int i = 0; itr != end; ++itr, ++i)
    {
        CV_Assert(itr->cols == 1 ||
            itr->rows == 1);
        if (itr->cols == 1)
        {
            transpose(*(itr), tmp);
            tmp.copyTo(trainData.row(i));
        }
        else if (itr->rows == 1)
        {
            itr->copyTo(trainData.row(i));
        }
    }
}

void Detection::sample_neg(const vector<Mat> &full_neg_lst, vector<Mat> &neg_lst, const Size &size)
{
    Rect box;
    box.width = size.width;
    box.height = size.height;

    const int size_x = box.width;
    const int size_y = box.height;

    srand((unsigned int)time(NULL));

    vector< Mat >::const_iterator img = full_neg_lst.begin();
    vector< Mat >::const_iterator end = full_neg_lst.end();
    for (; img != end; ++img)
    {
        box.x = rand() % (img->cols - size_x);
        box.y = rand() % (img->rows - size_y);
        Mat roi = (*img)(box);
        neg_lst.push_back(roi.clone());
#ifdef _DEBUG
        imshow("img", roi.clone());
        waitKey(10);
#endif
    }
}

Mat Detection::get_hogdescriptor_visu(const Mat &color_origImg, vector<float> &descriptorValues, const Size &size)
{
    const int DIMX = size.width;
    const int DIMY = size.height;
    float zoomFac = 3;
    Mat visu;
    resize(color_origImg, visu, Size((int)(color_origImg.cols*zoomFac), (int)(color_origImg.rows*zoomFac)));

    int cellSize = 8;
    int gradientBinSize = 9;
    float radRangeForOneBin = (float)(CV_PI / (float)gradientBinSize); // dividing 180° into 9 bins, how large (in rad) is one bin?

                                                                       // prepare data structure: 9 orientation / gradient strenghts for each cell
    int cells_in_x_dir = DIMX / cellSize;
    int cells_in_y_dir = DIMY / cellSize;
    float*** gradientStrengths = new float**[cells_in_y_dir];
    int** cellUpdateCounter = new int*[cells_in_y_dir];
    for (int y = 0; y<cells_in_y_dir; y++)
    {
        gradientStrengths[y] = new float*[cells_in_x_dir];
        cellUpdateCounter[y] = new int[cells_in_x_dir];
        for (int x = 0; x<cells_in_x_dir; x++)
        {
            gradientStrengths[y][x] = new float[gradientBinSize];
            cellUpdateCounter[y][x] = 0;

            for (int bin = 0; bin<gradientBinSize; bin++)
                gradientStrengths[y][x][bin] = 0.0;
        }
    }

    // nr of blocks = nr of cells - 1
    // since there is a new block on each cell (overlapping blocks!) but the last one
    int blocks_in_x_dir = cells_in_x_dir - 1;
    int blocks_in_y_dir = cells_in_y_dir - 1;

    // compute gradient strengths per cell
    int descriptorDataIdx = 0;
    int cellx = 0;
    int celly = 0;

    for (int blockx = 0; blockx<blocks_in_x_dir; blockx++)
    {
        for (int blocky = 0; blocky<blocks_in_y_dir; blocky++)
        {
            // 4 cells per block ...
            for (int cellNr = 0; cellNr<4; cellNr++)
            {
                // compute corresponding cell nr
                cellx = blockx;
                celly = blocky;
                if (cellNr == 1) celly++;
                if (cellNr == 2) cellx++;
                if (cellNr == 3)
                {
                    cellx++;
                    celly++;
                }

                for (int bin = 0; bin<gradientBinSize; bin++)
                {
                    float gradientStrength = descriptorValues[descriptorDataIdx];
                    descriptorDataIdx++;

                    gradientStrengths[celly][cellx][bin] += gradientStrength;

                } // for (all bins)


                  // note: overlapping blocks lead to multiple updates of this sum!
                  // we therefore keep track how often a cell was updated,
                  // to compute average gradient strengths
                cellUpdateCounter[celly][cellx]++;

            } // for (all cells)


        } // for (all block x pos)
    } // for (all block y pos)


      // compute average gradient strengths
    for (celly = 0; celly<cells_in_y_dir; celly++)
    {
        for (cellx = 0; cellx<cells_in_x_dir; cellx++)
        {

            float NrUpdatesForThisCell = (float)cellUpdateCounter[celly][cellx];

            // compute average gradient strenghts for each gradient bin direction
            for (int bin = 0; bin<gradientBinSize; bin++)
            {
                gradientStrengths[celly][cellx][bin] /= NrUpdatesForThisCell;
            }
        }
    }

    // draw cells
    for (celly = 0; celly<cells_in_y_dir; celly++)
    {
        for (cellx = 0; cellx<cells_in_x_dir; cellx++)
        {
            int drawX = cellx * cellSize;
            int drawY = celly * cellSize;

            int mx = drawX + cellSize / 2;
            int my = drawY + cellSize / 2;

            rectangle(visu, Point((int)(drawX*zoomFac), (int)(drawY*zoomFac)), Point((int)((drawX + cellSize)*zoomFac), (int)((drawY + cellSize)*zoomFac)), Scalar(100, 100, 100), 1);

            // draw in each cell all 9 gradient strengths
            for (int bin = 0; bin<gradientBinSize; bin++)
            {
                float currentGradStrength = gradientStrengths[celly][cellx][bin];

                // no line to draw?
                if (currentGradStrength == 0)
                    continue;

                float currRad = bin * radRangeForOneBin + radRangeForOneBin / 2;

                float dirVecX = cos(currRad);
                float dirVecY = sin(currRad);
                float maxVecLen = (float)(cellSize / 2.f);
                float scale = 2.5; // just a visualization scale, to see the lines better

                                   // compute line coordinates
                float x1 = mx - dirVecX * currentGradStrength * maxVecLen * scale;
                float y1 = my - dirVecY * currentGradStrength * maxVecLen * scale;
                float x2 = mx + dirVecX * currentGradStrength * maxVecLen * scale;
                float y2 = my + dirVecY * currentGradStrength * maxVecLen * scale;

                // draw gradient visualization
                line(visu, Point((int)(x1*zoomFac), (int)(y1*zoomFac)), Point((int)(x2*zoomFac), (int)(y2*zoomFac)), Scalar(0, 255, 0), 1);

            } // for (all bins)

        } // for (cellx)
    } // for (celly)


      // don't forget to free memory allocated by helper data structures!
    for (int y = 0; y<cells_in_y_dir; y++)
    {
        for (int x = 0; x<cells_in_x_dir; x++)
        {
            delete[] gradientStrengths[y][x];
        }
        delete[] gradientStrengths[y];
        delete[] cellUpdateCounter[y];
    }
    delete[] gradientStrengths;
    delete[] cellUpdateCounter;

    return visu;
}

void Detection::compute_hog(const vector<Mat> &img_lst, vector<Mat> &gradient_lst, const Size &size)
{
    HOGDescriptor hog;
    hog.winSize = size;
    Mat gray;
    vector< Point > location;
    vector< float > descriptors;

    vector< Mat >::const_iterator img = img_lst.begin();
    vector< Mat >::const_iterator end = img_lst.end();
    for (; img != end; ++img)
    {
        cvtColor(*img, gray, COLOR_BGR2GRAY);
        hog.compute(gray, descriptors, Size(8, 8), Size(0, 0), location);
        gradient_lst.push_back(Mat(descriptors).clone());
#ifdef _DEBUG
        imshow("gradient", get_hogdescriptor_visu(img->clone(), descriptors, size));
        waitKey(10);
#endif
    }
}

void Detection::train_svm(const vector<Mat> &gradient_lst, const vector<int> &labels)
{
    /* Default values to train SVM */
    Ptr<SVM> svm = SVM::create();
    svm->setCoef0(0.0);
    svm->setDegree(3);
    svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-3));
    svm->setGamma(0);
    svm->setKernel(SVM::LINEAR);
    svm->setNu(0.5);
    svm->setP(0.1); // for EPSILON_SVR, epsilon in loss function?
    svm->setC(0.01); // From paper, soft classifier
    svm->setType(SVM::EPS_SVR); // C_SVC; // EPSILON_SVR; // may be also NU_SVR; // do regression task

    Mat train_data;
    convert_to_ml(gradient_lst, train_data);

    clog << "Start training...";
    svm->train(train_data, ROW_SAMPLE, Mat(labels));
    clog << "...[done]" << endl;

    svm->save(TRAINED_SVM);
}

void Detection::draw_locations(Mat &img, const vector<Rect> &locations, const Scalar &color)
{
    if (!locations.empty())
    {
        vector< Rect >::const_iterator loc = locations.begin();
        vector< Rect >::const_iterator end = locations.end();
        for (; loc != end; ++loc)
        {
            rectangle(img, *loc, color, 2);
        }
    }
}

void Detection::test_it(const Size &size)
{
    char key = 27;
    Mat img, draw;
    Ptr<SVM> svm;
    HOGDescriptor hog;
    hog.winSize = size;
    VideoCapture video;
    vector< Rect > locations;

    // Load the trained SVM.
    svm = StatModel::load<SVM>(TRAINED_SVM);
    // Set the trained svm to my_hog
    vector< float > hog_detector;
    get_svm_detector(svm, hog_detector);
    hog.setSVMDetector(hog_detector);

    // Open the camera.
    video.open(TRAFFIC_VIDEO_FILE);
    if (!video.isOpened())
    {
        cerr << "Unable to open the device" << endl;
        exit(-1);
    }

    int num_of_vehicles = 0;

    bool end_of_process = false;
    while (!end_of_process)
    {
        video >> img;
        if (img.empty())
            break;

        draw = img.clone();

        // Eliminate ingoing traffic
        for (int pi = 0; pi < img.rows; ++pi)
            for (int pj = 0; pj < img.cols; ++pj)
                if (pj > img.cols / 2) {
                    img.at<Vec3b>(pi, pj)[0] = 0;
                    img.at<Vec3b>(pi, pj)[1] = 0;
                    img.at<Vec3b>(pi, pj)[2] = 0;
                }

        locations.clear();
        hog.detectMultiScale(img, locations);
        draw_locations(draw, locations, Scalar(0, 255, 0));

        for(Rect r : locations) {

            // Center point of the vehicle
            Point center(r.x + r.width / 2, r.y + r.height / 2);

            if (abs(center.y - img.rows * 2 / 3) < 0) {
                ++num_of_vehicles;
                line(draw, Point(0, img.rows * 2 / 3), Point(img.cols / 2, img.rows * 2 / 3), Scalar(0, 255, 0), 3);
                imshow(WINDOW_NAME, draw);
                waitKey(23);
            }
            else
                line(draw, Point(0, img.rows * 2 / 3), Point(img.cols / 2, img.rows * 2 / 3), Scalar(0, 0, 255), 3);

        }

        putText(draw, "Detected vehicles: " + to_string(num_of_vehicles), Point(50, 50), 1, 1, Scalar(0, 0, 255), 2);

        imshow(WINDOW_NAME, draw);
        key = (char)waitKey(10);
        if (27 == key)
            end_of_process = true;
    }
}

HOGDescriptor Detection::getHogDetector()
{
    Ptr<SVM> svm;
    HOGDescriptor hog;
    hog.winSize = IMAGE_SIZE;

    // Load the trained SVM.
    svm = StatModel::load<SVM>(TRAINED_SVM);
    // Set the trained svm to my_hog
    vector< float > hog_detector;
    get_svm_detector(svm, hog_detector);
    hog.setSVMDetector(hog_detector);

    return hog;
}

void Detection::start()
{
    QThreadPool::globalInstance()->clear();
    QThreadPool::globalInstance()->setMaxThreadCount(20);
    rDetects.clear();

    for(road i : rds){
        emit detectInfo(QString("Starting surveillance on ROAD: %1").arg(i.road_id));
        RunDetection *each = new RunDetection(i, getHogDetector(), mRds->value(i.road_id), speedRds->value(i.road_id), congRds->value(i.road_id), dlg->getLabelById(i.road_id), speedDlg->getLabelById(i.road_id));
        connect(each, SIGNAL(finish(qint64)), this, SLOT(finished(qint64)));
        connect(each, SIGNAL(congestion(qint64,bool)), this, SLOT(onCongestion(qint64,bool)));
        rDetects.append(each);
        QThreadPool::globalInstance()->start(each);
    }
    dlg->show();
    speedDlg->show();
}

void Detection::pause()
{
    for(RunDetection *i : rDetects){
        if(i->playType == PAUSE_PLAY)
            i->setPlay(START_PLAY);
        else
            i->setPlay(PAUSE_PLAY);
    }
}

void Detection::stop()
{
    for(RunDetection *i : rDetects){
        i->setPlay(STOP_PLAY);
    }
    QThreadPool::globalInstance()->waitForDone();
    dlg->close();
    speedDlg->close();
}

void Detection::finished(qint64 road_id)
{
    if(!fin){
        fin = true;
        dlg->close();
        speedDlg->close();
        emit detectInfo(QString("Surveillance finished @ROAD: %1. Stopping all.").arg(road_id));
        emit finish(road_id);
    }
}

void Detection::onCongestion(qint64 road_id, bool isCongested)
{
    congRds->insert(road_id, new bool(isCongested));
}

void Detection::scrollTo(qint64 id)
{
    dlg->scrollTo(id);
    speedDlg->scrollTo(id);
}

RunDetection::RunDetection(struct road r, HOGDescriptor hog, mut_road *mRd, float* speed, bool* congRd, QLabel* imageLabel, QLabel* speedLabel, qint64 width, qint64 height):
    hog(hog),
    road(r),
    mRd(mRd),
    speed(speed),
    congRd(congRd),
    imageLabel(imageLabel),
    speedLabel(speedLabel),
    mFrameW(width),
    mFrameH(height)
{
    playType = START_PLAY;
    setAutoDelete(true);
}

RunDetection::~RunDetection()
{
//    if(mRd != nullptr)
//        delete mRd;
}

void RunDetection::run()
{
    VideoCapture video;
    Mat img, draw;
    vector< Rect > locations;
    char key;

    if(video.open(road.video_addr.toStdString())){
        if(video.isOpened()){
            // Tracker setup
            transformation = new Transformation();
            std::vector<cv::Point2f> pts;
            pts.push_back(cv::Point2f(CAPTURE_WIDTH,  0));
            pts.push_back(cv::Point2f(CAPTURE_WIDTH,  CAPTURE_HEIGHT));
            pts.push_back(cv::Point2f(CAPTURE_WIDTH/3,  CAPTURE_HEIGHT));
            pts.push_back(cv::Point2f(CAPTURE_WIDTH/3,  0));

            transformation->setCalibRect(pts);
            // frame width,height is same(320, 240) i.e no resizing the video

            // create transformation matrix to map pts to quadpts:
            const float realWidth = CAPTURE_WIDTH; // calibration area length in meters
            const float realHeight = 2*CAPTURE_HEIGHT/3; // calibration area height in meters
            const float aspectRatio = realWidth / realHeight;

            const float pixelCalibHeight =  CAPTURE_WIDTH/(2*aspectRatio);
            mPx2meter = pixelCalibHeight / realHeight;

            std::cout << "Calibration width: " << CAPTURE_WIDTH/2 << " (" << CAPTURE_WIDTH/2 / realWidth << " px/m" << std::endl;
            std::cout << "Calibration height: " <<  pixelCalibHeight << " (" << pixelCalibHeight/realHeight << " px/m" << std::endl << std::endl;

            std::vector< cv::Point2f > quadpts;
            quadpts.push_back( cv::Point2f( CAPTURE_WIDTH/4, CAPTURE_HEIGHT/3 ));
            quadpts.push_back( cv::Point2f( 3*CAPTURE_WIDTH/4, CAPTURE_HEIGHT/3 ));
            quadpts.push_back( cv::Point2f( 3*CAPTURE_WIDTH/4, CAPTURE_HEIGHT/3+pixelCalibHeight  ));
            quadpts.push_back( cv::Point2f( CAPTURE_WIDTH/4, CAPTURE_HEIGHT/3+pixelCalibHeight ) );


            transformation->setQuadPts( quadpts );
            transformation->createMatrix(); // calculate the perpective transform matrix
            transformation->transformCalibRect(); // apply transform matrix to calibration rectangle corners

            // Detector and tracker classes:
            detector = new Detector;
            // VehicleTracker(float _dt, float _accelNoiseMag, double _dist_thresh, int _maxskipped, int _maxTrailLength);
            mVehicleTracker = new VehicleTracker(1./mFR, 1.5, 90., 20, 100);
            // set calibration related parameters:
            mVehicleTracker->setCalibrationRect( transformation->getCalibRectTransform() );
            mVehicleTracker->setPx2meter( mPx2meter );

            //
            qDebug() << "Started";
            while(playType != STOP_PLAY){
                if(playType != PAUSE_PLAY){

                    video >> img;
                    if(img.empty())
                        break;
                    draw = img.clone();

//                     Eliminate ingoing traffic
//                    for (int pi = 0; pi < img.rows; ++pi)
//                        for (int pj = 0; pj < img.cols; ++pj)
//                            if (pj > img.cols / 2) {
//                                img.at<Vec3b>(pi, pj)[0] = 0;
//                                img.at<Vec3b>(pi, pj)[1] = 0;
//                                img.at<Vec3b>(pi, pj)[2] = 0;
//                            }

                    // Tracking update
                    std::vector<cv::Point2d>  centerPts; // to collect blob centers

                    // get current time
                    clock_t time = clock();

                    // current time in ms
                    mVehicleTracker->setCurrentTime( double(time) / CLOCKS_PER_SEC * 1000);

                    cv::Mat framecopy = img.clone();

                    // Perspective transform
                    transformation->drawCalibRectOrig( framecopy );


                    cv::Mat framePerspT = cv::Mat::zeros(CAPTURE_HEIGHT, CAPTURE_WIDTH, img.type() );

                    transformation->apply(img, framePerspT);

                    transformation->drawCalibRectTransform( framePerspT );


                    // Background segmentation &
                    // Detection of contours on forground image mask:
                    cv::Mat img_processed = framePerspT.clone();

                    cv::Mat img_bgmask;
                    detector->process( framePerspT, img_processed, img_bgmask, centerPts );


                    // update VehicleTracker:
                    mVehicleTracker->update(centerPts);

                    // now the results are available:
                    mVehicleTracker->drawPredictions(img_processed);

                    mVehicleTracker->drawTracks(img_processed);

                    mNumCars = mVehicleTracker->getCarCount();

                    nNumCarsRight = mVehicleTracker->getCarCountRight();

                    mNumCurrentCars = mVehicleTracker->getCurrentCarCount();

                    mCurrentCarSpeeds = mVehicleTracker->getCurrentCarSpeeds();

                    mCurrentCarSpeedAverages = mVehicleTracker->getCurrentCarSpeedAverages();

                    setAverageSpeed(mCurrentCarSpeeds);

                    // qDebug() << "mNumCars: " << mNumCars << " nNumCarsRight: " << nNumCarsRight << " mNumCurrentCars: " << mNumCurrentCars ;
//                    cv::imshow("framePerspT", framePerspT);
//                    cv::imshow("img_processed", img_processed);
                    putText(img_processed, "ROAD: "+road.name.toStdString(), Point(50, 30), 1, 1, Scalar(0, 0, 255), 2);
                    putText(img_processed, "AVG SPEED: "+QString::number(avgSpeed).toStdString(), Point(50, 50), 1, 1, Scalar(0, 0, 255), 2);
                    speedLabel->setPixmap(QPixmap::fromImage(QImage((uchar*)img_processed.data, img_processed.cols, img_processed.rows, img_processed.step, QImage::Format_RGB888).scaled(mFrameW, mFrameH, Qt::KeepAspectRatio)));
                    //

                    locations.clear();

                    hog.detectMultiScale(img, locations);
                    Detection::draw_locations(draw, locations, Scalar(0, 255, 0));

                    checkCongestion(locations.size(), avgSpeed);

                    for(Rect r : locations) {

                        // Center point of the vehicle
                        Point center(r.x + r.width / 2, r.y + r.height / 2);

                        if (abs(center.y - img.rows * 2 / 3) < 5) {
//                            mRd->vehicle_count += 1;
                            ++mRd->vehicle_count;
                            line(draw, Point(0, img.rows * 2 / 3), Point(img.cols, img.rows * 2 / 3), Scalar(0, 255, 0), 3);
                            // imshow("ROAD: "+std::to_string(road.road_id), draw);
                            imageLabel->setPixmap(QPixmap::fromImage(QImage((uchar*) draw.data, draw.cols, draw.rows, draw.step, QImage::Format_RGB888)).scaled(mFrameW, mFrameH, Qt::KeepAspectRatio));
                            waitKey(29);
                        }
                        else
                            line(draw, Point(0, img.rows * 2 / 3), Point(img.cols, img.rows * 2 / 3), Scalar(0, 0, 255), 3);

                    }
                    putText(draw, "Road: "+road.name.toStdString(), Point(50, 30), 1, 1, Scalar(0, 0, 255), 2);
                    putText(draw, "Detected vehicles: " + to_string(mRd->vehicle_count), Point(50, 50), 1, 1, Scalar(0, 0, 255), 2);
                    // imshow("ROAD: "+std::to_string(road.road_id), draw);
                    imageLabel->setPixmap(QPixmap::fromImage(QImage((uchar*) draw.data, draw.cols, draw.rows, draw.step, QImage::Format_RGB888)).scaled(mFrameW, mFrameH, Qt::KeepAspectRatio));
                    key = (char)waitKey(10);
                    if (27 == key){
                        setPlay(STOP_PLAY);
                        emit finish(road.road_id);
                    }
                }else{
                    waitKey(500);
                }
            }
        }
    }
    setPlay(STOP_PLAY);
    qDebug() << "Stopped";
    destroyAllWindows();
    emit finish(road.road_id);
}

void RunDetection::setAverageSpeed(vector<float> avgs)
{
    float sum = 0;
    if(avgs.size() > 0){
        for(float i : avgs){
            sum += i;
        }
        avgSpeed = sum/avgs.size();
        *speed = avgSpeed;
    }
}

void RunDetection::checkCongestion(qint64 onScreenCount, float vehicleSpeed)
{
    if(onScreenCount >= CONGESTION_COUNT_THRESHOLD && vehicleSpeed <= CONGESTION_SPEED_THRESHOLD){
        if(!*congRd){
            *congRd = true;
            mRd->congestion_count++;
            emit congestion(road.road_id, true);
        }
    }else{
        *congRd = false;
        emit congestion(road.road_id, false);
    }
}

void RunDetection::setPlay(qint64 type)
{
    playType = type;
}

DetectionDialog::DetectionDialog(QList<qint64> ids, QString title, QWidget *parent, qint64 width, qint64 height):
    QDialog (parent),
    ids(ids),
    width(width),
    height(height)
{
    buildImageLabels();
    buildImageList();
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(imageList);
    setLayout(layout);
    setWindowTitle(title);
    setMinimumSize(width+50, height+50);
    setWindowFlags(Qt::WindowStaysOnTopHint);

    static int wdgt_count;
    ++wdgt_count;
    if(wdgt_count > 2)
        wdgt_count = 1;
    QRect rect = QApplication::desktop()->availableGeometry();
    move(rect.width()-(wdgt_count*width+60), rect.height()-height-40);
}

void DetectionDialog::buildImageLabels()
{
    for(qint64 id : ids){
        QLabel* each = new QLabel(this);
        each->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        each->setMinimumSize(width, height);
        imageLabels.insert(id, each);
    }
}

void DetectionDialog::buildImageList()
{
    imageList = new QListWidget(this);
    imageList->setFlow(QListWidget::LeftToRight);
    imageList->setSpacing(4);
    for(qint64 id : ids){
        QListWidgetItem* eachItem = new QListWidgetItem(imageList);
        imageWdgItm.insert(id, eachItem);
        eachItem->setSizeHint(imageLabels.value(id)->sizeHint());
        imageList->setItemWidget(eachItem, imageLabels.value(id));
    }
}

DetectionDialog::~DetectionDialog()
{

}

void DetectionDialog::scrollTo(qint64 id)
{
    imageList->scrollToItem(imageWdgItm.value(id));
}
