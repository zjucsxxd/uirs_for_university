#ifndef ROBOT_CONNECT_H
#define ROBOT_CONNECT_H

#include <QtGui/QMainWindow>
#include "ui_robot_connect.h"
#include "Player.h"
#include <QtNetwork/QUdpSocket>

// Struct will be send to mouseclick func as a (void*)
struct Kostil
{
	QVector<CvPoint>	points;
	Mat					frame;
};
// Main class
class Robot_Connect : public QMainWindow
{
	Q_OBJECT

public:
	Robot_Connect(QWidget *parent = 0, Qt::WFlags flags = 0);
	~Robot_Connect(){};
	QUdpSocket *udpSocket;	// Soket
	QString		str;
private slots:
	void updatePlayerUI(QImage img, QImage img_korr);// Paint the pics on Qt form
	void getPoints ();		// Set some points as a target for cars
	void initSocket();		// socket initialisation
	void changeFind();		// Change the mrthod of recognising

private:
	Ui::Robot_ConnectClass	*ui		 ;
	Player					*myPlayer;
	Kostil					toSend	 ;
};
// function after mouse click
void myMouseCallback( int event, int x, int y, int flags, void* param );

#endif // ROBOT_CONNECT_H
