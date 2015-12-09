
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_MainWindow.h"
#include "Volume.h"
#include "VectorField.h"
#include "MultiSet.h"
#include "Vector.h"

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QStatusBar>
#include <QVariant>

//#include <glm.hpp>


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:

	MainWindow(QWidget *parent = 0);
	~MainWindow();


protected:
	void calculateGradient();               //Gradientenberechnung
	float xGradient(int x, int y, int z);
	float yGradient(int x, int y, int z);
	float zGradient(int x, int y, int z);
	float xGradientMiddle(int x, int y, int z);
	float yGradientMiddle(int x, int y, int z);
	float zGradientPlus(int x, int y, int z);

	protected slots :

	void								 openFileAction();
	void								 closeAction();
	//own slots
	void cpuRaycasting();
	void gpuRaycasting();

	void firstHitRaycasting();


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

	std::vector<float> *mipData; //for saving max values
	
	std::vector<float> *alphaData; //for saving alpha values
	std::vector<float> *firstHitData; //for saving max values

	std::vector<float> volume_points;


	//Volume	gradient_Volume;
	std::vector<float> *gradient_Volume;

};

#endif
