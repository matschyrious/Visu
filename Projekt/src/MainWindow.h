
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_MainWindow.h"
#include "Volume.h"
#include "VectorField.h"
#include "MultiSet.h"

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QStatusBar>
#include <QVariant>


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:

	MainWindow(QWidget *parent = 0);
	~MainWindow();
	void cpuRaycasting();
	

protected:
	void paintEvent(QPaintEvent *evn);      //for painting
	void calculateGradient();               //Gradientenberechnung
	int xGradient(int x, int y, int z);
	int yGradient(int x, int y, int z);
	int zGradient(int x, int y, int z);
	int xGradientMiddle(int x, int y, int z);
	int yGradientMiddle(int x, int y, int z);
	int zGradientPlus(int x, int y, int z);


	protected slots :

	void								 openFileAction();
	void								 closeAction();


private:

	// USER INTERFACE ELEMENTS

	Ui_MainWindow						*m_Ui;


	// DATA 

	enum DataType
	{
		VOLUME = 0,
		VECTORFIELD = 1,
		MULTIVARIATE = 2
	};

	struct FileType
	{
		QString			filename;
		DataType		type;
	}									 m_FileType;

	Volume								*m_Volume;						// for Volume-Rendering
	VectorField							*m_VectorField;					// for Flow-Visualisation
	MultiSet							*m_MultiSet;					// for Multivariate Data
	std::vector<float> *data; //for saving max values

	Volume								gradient_Volume;
	int									sobel[3][3];
};

#endif