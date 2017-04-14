#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <QImage>
#include <QThread>
#include <string>
#include <QMutex>
#include <QWaitCondition>
#include <QFile>
#include <QMap>
#include <QFileDialog>
#include <QMessageBox>
#include <QWaitCondition>
#include <opencv2/core/operations.hpp>
#include <QtNetwork/QUdpSocket.h>
using namespace cv;
#define R_DOP 0
#define K_DOP 4000

class Player : public QThread
{    Q_OBJECT
private:
	bool stop;
	QMutex mutex;
	QWaitCondition condition;
	Mat frame,korrF;
	int frameRate;
	VideoCapture capture;
	Mat RGBframe,tmp_fr;
	QImage img,img_korr;
	

signals:
	//Signal to output frame to be displayed
	void processedImage(const QImage &image,const QImage &image_kor);
protected:
	void run();
public:
	bool isConnected;							//убедимся что прочли темплейт
	QVector<CvPoint>	carCoord, pointCoord;	//координаты тачек и точек
	Mat					templ;					//образец для поиска
	std::vector<Vec3f>	circles;
	
	//Constructor
	Player(QObject *parent = 0);
	//Destructor
	~Player();
	//Socket
	QUdpSocket *udpSocket;
	bool		isHough;
	//get current frame
	Mat  getCurFr();
	//Load a video from memory
	bool loadVideo();
	//Play the video
	void Play();
	//Stop the video
	void Stop();
	//check if the player has been stopped
	bool isStopped() const;

	void HoughFind	();
};

void getCarsLoc(Mat *korr,QVector<CvPoint> &cord, int xS, int yS);
void chekPoints(QVector<CvPoint> &cars, QVector<CvPoint> &points, int xS, int yS);
int  bashFindTemplate(Mat* _in, Mat* _tmpl, QVector<CvPoint>& _p);

