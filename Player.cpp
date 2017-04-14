#include "Player.h"

Player::Player(QObject *parent)
	: QThread(parent)
{
	stop = true;
	isConnected = false;

	//Socket
	//------------------------------------------


	udpSocket = new QUdpSocket(this);
	udpSocket->bind(QHostAddress::LocalHost, 7755);

	connect(udpSocket, SIGNAL(readyRead()),
		this, SLOT(read()));
	//------------------------------------------
}

bool Player::loadVideo() 
{
	//open template
	templ = imread("template4.jpg");
	if (!templ.empty())
	{
		isConnected = true;
		//cvtColor(templ,templ,	CV_RGB2GRAY);

	}
	//open camera
	capture.open(0);
	if (capture.isOpened())
	{
		frameRate = 30;//(int) capture.get(CV_CAP_PROP_FPS);
		return true;
	}
	else
		return false;
}

void Player::Play()
{
	if (!isRunning()) {
		if (isStopped()){
			stop = false;
		}
		start(LowPriority);
	}	
}

void Player::run()
{
	int delay = (1000/frameRate);
	while(!stop){
		if (!capture.read(frame))
		{
			stop = true;
		}
		if (frame.channels()== 3){

			if (!isConnected)
			{
				putText(frame, "Template file is not matched", cvPoint(30,300), 
					FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(250,250,250), 1, CV_AA);
			}
			else
			{
				//Поиск машин
				//_______________________________________
				matchTemplate(frame,templ,korrF,3);
				carCoord.clear();
				getCarsLoc(&korrF,carCoord,templ.cols,templ.rows);
				//cvtColor(frame,tmp_fr,		CV_RGB2GRAY);
				//bashFindTemplate(&tmp_fr,&templ,carCoord);


				chekPoints(carCoord, pointCoord, templ.cols, templ.rows);

				for(int i = 0;i<carCoord.size();i++)
					rectangle(frame,carCoord[i],cvPoint(carCoord[i].x + templ.cols,carCoord[i].y + templ.rows),Scalar(200,0,0));
				
				imshow("1",korrF);
				waitKey(1);
				cvtColor(korrF, korrF, CV_GRAY2RGB);

				img_korr = QImage((const unsigned char*)(korrF.data),
					korrF.cols,korrF.rows,QImage::Format_RGB888);
			}
			for(int i = 0;i<pointCoord.size();i++)
				drawMarker(frame, pointCoord[i],cv::Scalar(250, 255, 255));
			cv::cvtColor(frame, RGBframe, CV_BGR2RGB);
			img = QImage((const unsigned char*)(RGBframe.data),
				RGBframe.cols,RGBframe.rows,QImage::Format_RGB888);
		}
		else
		{
			img = QImage((const unsigned char*)(frame.data),
				frame.cols,frame.rows,QImage::Format_Indexed8);
		}
		emit processedImage(img, img_korr);
		msleep(delay);
	}
}

Player::~Player()
{
	mutex.lock();
	stop = true;
	capture.release();
	condition.wakeOne();
	mutex.unlock();
	wait();
}
void Player::Stop()
{
	stop = true;
}

bool Player::isStopped() const{
	return this->stop;
}


Mat Player::getCurFr()
{
	return frame;
}

void getCarsLoc(Mat *korr,QVector<CvPoint> &cord, int xS, int yS)
{
	QMap<double,CvPoint>	trgets;
	double t;
	for (int y=0;y<korr->rows;y++)
	{
		for (int x=0;x<korr->cols;x++)
		{
			t = korr->at<float>(Point(x,y));
			if (t>0.880)//0.96)
			{
				trgets[t] = cvPoint(x,y);
			}
		}
	}
	CvPoint p;
	bool cond;
	if (trgets.size())
	{
		QMap<double,CvPoint>::ConstIterator it = trgets.constEnd();
		while(it != trgets.constBegin())
		{
			if (!cord.size())
			{
				cord.push_back(it.value());
			}
			else
			{
				cond = true;
				for (int i =0;i<cord.size();i++)
				{
					if (abs(it->x - cord[i].x)< xS && abs(it->y - cord[i].y) <yS)
					{
						cond = false;
						break;
					}
				}
				if (cond)
				{
					cord.push_back(it.value());
				}
				it--;
			}
		}
			
	}
	
	//minMaxLoc(*korr,&mi,&ma,&m,&p);
	
}


void chekPoints(QVector<CvPoint> &cars, QVector<CvPoint> &points, int xS, int yS)
{
	//double r;
	for (int i = 0;i<points.size();i++)
	{
		for (int k = 0;k<cars.size();k++)
		{
			//r = sqrt(double((points[i].x - cars[k].x)*(points[i].x - cars[k].x)+(points[i].x - cars[k].x)*(points[i].y - cars[k].y)));
			if (abs(points[i].x - cars[k].x) < xS+R_DOP && abs(points[i].y - cars[k].y) < yS+R_DOP)
			{
				points.remove(i);
			}
		}
	}
}


int bashFindTemplate(Mat* _in, Mat* _tmpl, QVector<CvPoint>& _p)
{
	
	int xk,yk;
	double	k, maxK;
	bool	condit;
	for (int y = 0;y<_in->cols;y++)
	{
		for (int x = 0;x<_in->rows;x++)
		{
			k=0;maxK =0;
			condit = false;
			for (int y_ = 0;y_ < _tmpl->cols;y_++)
			{
				for (int x_ = 0;x_<_tmpl->rows;x_++)
				{
					k += abs(_tmpl->at<uchar>(x_,y_) - _in->at<uchar>(x,y));
					if (k>K_DOP)
					{
						if (k>maxK)
						{
							maxK = k;

						}
						condit = true;
						break;
					}
				}
				if (condit)
				{
					break;
				}
			}	
			if(condit == false)
				_p<<cvPoint(x,y);
			//else
			//	_p<<cvPoint(x,y);
		}
	}

	return 0;
}