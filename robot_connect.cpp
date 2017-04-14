#include "robot_connect.h"

Robot_Connect::Robot_Connect(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	myPlayer = new Player();
	QObject::connect(myPlayer, SIGNAL(processedImage(QImage,QImage)),
		this, SLOT(updatePlayerUI(QImage,QImage)));

	ui->setupUi(this);

	connect(ui->getPoint_pb, SIGNAL(clicked()),
		this, SLOT(getPoints()));
	connect(ui->checkBox,SIGNAL(clicked()), this, SLOT(changeFind()));

	initSocket();

	if(!myPlayer->loadVideo())
	{
		QMessageBox::warning(this, "Error", "Dont see any cameras");
	}
	else
	{
		myPlayer->Play();
	}
}

// socket initialisation
void Robot_Connect::initSocket()
{
	udpSocket = new QUdpSocket(this);
	udpSocket->bind(QHostAddress::Any, 7755);

	connect(udpSocket, SIGNAL(readyRead()),
		this, SLOT(readPendingDatagrams()));
}

// Paint the pics on Qt form
void Robot_Connect::updatePlayerUI(QImage img, QImage img_korr)
{
	if (!img.isNull())
	{
		ui->label->setAlignment(Qt::AlignCenter);
		ui->label->setPixmap(QPixmap::fromImage(img).scaled(ui->label->size(), Qt::KeepAspectRatio, Qt::FastTransformation));
	}
	if (myPlayer->isConnected)
	{
		if (!img_korr.isNull())
		{
			ui->label_korr->setAlignment(Qt::AlignCenter);
			ui->label_korr->setPixmap(QPixmap::fromImage(img_korr).scaled(ui->label->size(), Qt::KeepAspectRatio, Qt::FastTransformation));
		}
	}
	str = "Cars position:";
	//transmition
	for (int i=0;i<myPlayer->carCoord.size();i++)
	{
		str += " ";
		str += QString::number(myPlayer->carCoord[i].x); 
		str += ",";
		str += QString::number(myPlayer->carCoord[i].y);
	}

	str += "Points position:";
	for (int i=0;i<myPlayer->pointCoord.size();i++)
	{
		str += " ";
		str += QString::number(myPlayer->pointCoord[i].x); 
		str += ",";
		str += QString::number(myPlayer->pointCoord[i].y);
	}
	

	QByteArray data;
	data.append(str);
	udpSocket->writeDatagram(data, QHostAddress::LocalHost, 7755);

}
// Set some points as a target for cars
void Robot_Connect::getPoints()
{
	namedWindow("Set points");

	myPlayer->pointCoord.clear();
	toSend.points		.clear();
	toSend.frame = myPlayer->getCurFr();

	setMouseCallback("Set points", myMouseCallback, (void*)&toSend);
	char c;
	while(true)
	{
		for(int i = 0;i<myPlayer->pointCoord.size();i++)
			drawMarker(toSend.frame, myPlayer->pointCoord[i],cv::Scalar(250, 255, 255));
		imshow("Set points",toSend.frame);
		c = cvWaitKey(33);
		myPlayer->pointCoord = toSend.points;

		if (c == 27) { // если нажата ESC - выходим
			destroyWindow("Set points");
			break;
		}
	}


}
// function after mouse click
void myMouseCallback( int event, int x, int y, int flags, void* param )
{
	Kostil *kostil = (Kostil*) param; 

	switch( event ){
	case CV_EVENT_MOUSEMOVE: 
		break;

	case CV_EVENT_LBUTTONDOWN:
		{
			kostil->points<<cvPoint(x,y);
			cv::drawMarker(kostil->frame, cvPoint(x,y),cv::Scalar(250, 255, 255));
			break;
		}


	case CV_EVENT_LBUTTONUP:
		break;
	}
}
// Change the mrthod of recognising
void Robot_Connect::changeFind()
{
	if (ui->checkBox->isChecked())
	{
		myPlayer->isHough = true;
	}else{
		myPlayer->isHough = false;
	}
}