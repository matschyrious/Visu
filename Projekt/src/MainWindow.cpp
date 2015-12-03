#include "MainWindow.h"

#include <QFileDialog>

#include <QPainter>




MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent), m_Volume(0), m_VectorField(0)
{
	m_Ui = new Ui_MainWindow();
	m_Ui->setupUi(this);

	connect(m_Ui->actionOpen, SIGNAL(triggered()), this, SLOT(openFileAction()));
	connect(m_Ui->actionClose, SIGNAL(triggered()), this, SLOT(closeAction()));
}

MainWindow::~MainWindow()
{
	delete m_Volume;
	delete m_VectorField;
	delete m_MultiSet;
}


//-------------------------------------------------------------------------------------------------
// Slots
//-------------------------------------------------------------------------------------------------

void MainWindow::openFileAction()
{
	QString filename = QFileDialog::getOpenFileName(this, "Data File", 0, tr("Data Files (*.dat *.gri *.csv)"));

	if (!filename.isEmpty())
	{
		// store filename
		m_FileType.filename = filename;
		std::string fn = filename.toStdString();
		bool success = false;

		// progress bar and top label
		m_Ui->progressBar->setEnabled(true);
		m_Ui->labelTop->setText("Loading data ...");

		// load data according to file extension
		if (fn.substr(fn.find_last_of(".") + 1) == "dat")		// LOAD VOLUME
		{
			// create VOLUME
			m_FileType.type = VOLUME;
			m_Volume = new Volume();

			// load file
			success = m_Volume->loadFromFile(filename, m_Ui->progressBar);
		}
		else if (fn.substr(fn.find_last_of(".") + 1) == "gri")		// LOAD VECTORFIELD
		{
			// create VECTORFIELD
			m_FileType.type = VECTORFIELD;
			m_VectorField = new VectorField();

			// load file
			success = m_VectorField->loadFromFile(filename, m_Ui->progressBar);
		}
		else if (fn.substr(fn.find_last_of(".") + 1) == "csv")		// LOAD MULTIVARIATE DATA
		{
			// create MULTIVARIATE
			m_FileType.type = MULTIVARIATE;
			m_MultiSet = new MultiSet();

			// load file
			success = m_MultiSet->loadFromFile(filename, m_Ui->progressBar);
		}

		m_Ui->progressBar->setEnabled(false);

		// status message
		if (success)
		{
			QString type;
			if (m_FileType.type == VOLUME) type = "VOLUME";
			else if (m_FileType.type == VECTORFIELD) type = "VECTORFIELD";
			else if (m_FileType.type == MULTIVARIATE) type = "MULTIVARIATE";
			m_Ui->labelTop->setText("File LOADED [" + filename + "] - Type [" + type + "]");

			//raycasting aufruf
			cpuRaycasting();

			calculateGradient();
		}
		else
		{
			m_Ui->labelTop->setText("ERROR loading file " + filename + "!");
			m_Ui->progressBar->setValue(0);
		}
	}
}

void MainWindow::closeAction()
{
	close();
}

void MainWindow::cpuRaycasting(){


	data = new std::vector<float>(m_Volume->width()*m_Volume->height(), 0);

	for (int x = 0; x < m_Volume->width(); x++){
		for (int y = 0; y < m_Volume->height(); y++){

			for (int d = 0; d < m_Volume->depth(); d++){
				data->at(x + (y*m_Volume->width())) = std::fmax(data->at(x + (y*m_Volume->width())), m_Volume->voxel(x, y, d).getValue());
			}
		}
	}

	std::cout << "fertig mit max íntensity berechnung" << std::endl;

	repaint();

}



void MainWindow::paintEvent(QPaintEvent *evn){

	setAutoFillBackground(true);
	QColor color = QColor(255, 0, 0);

	if (m_Volume != nullptr && data != nullptr){

		QPainter painter;
		painter.begin(this);
		painter.setRenderHint(QPainter::Antialiasing);

		QPen pen = QPen();
		pen.setColor(color);
		painter.setPen(pen);


		for (int x = 0; x < m_Volume->width(); x++){
			for (int y = 0; y < m_Volume->height(); y++){
				if (data->at(x + (y*m_Volume->width())) > 0){
					color = QColor(255 - (255 * data->at(x + (y*m_Volume->width()))), 0, 0);
					pen.setColor(color);
					painter.setPen(pen);
					painter.drawPoint(x + 10, y + 100);
				}
			}
		}

		painter.end();
	}
}


void MainWindow::calculateGradient(){
	int gx, gy,gz;
	float sum;
	gradient_Volume = *m_Volume;
	
	for (int x = 1; x < m_Volume->width(); x++){
		for (int y = 1; y < m_Volume->height(); y++){
			for (int z = 1; z < m_Volume->depth(); z++){

				gx =  xGradient( x, y, z-1);
				gx += xGradientMiddle(x, y, z);
				gx += xGradient(x, y, z + 1);

				gy = yGradient(x, y, z-1);
				gy += yGradientMiddle(x, y, z);
				gy += yGradient(x, y, z + 1);

				gz = zGradient(x, y, z-1);
				//gzMiddle is zero
				gz += zGradientPlus(x, y, z + 1);
				
				sum = abs(gx) + abs(gy) + abs(gz);
				sum = sum > 255 ? 255 : sum;
				sum = sum < 0 ? 0 : sum;
				gradient_Volume->voxel(x, y, z).setValue(sum);

			}
		}
	}
	std::cout << "fertig mit gradient berechnung" << std::endl;

}


// GX
// -1 0 1
// -2 0 2
// -1 0 1
int MainWindow::xGradient(int x, int y, int z)
{   

	return	(-1)* m_Volume->voxel(x - 1, y - 1, z).getValue() +
			(-2)* m_Volume->voxel(x - 1, y, z).getValue() +
			(-1)* m_Volume->voxel(x - 1, y + 1, z).getValue() +
				m_Volume->voxel(x + 1, y - 1, z).getValue() +
			2 * m_Volume->voxel(x + 1, y - 1, z).getValue() +
				m_Volume->voxel(x + 1, y - 1, z).getValue();
		
}
// -2 0 2
// -4 0 4
// -2 0 2
int MainWindow::xGradientMiddle(int x, int y, int z)
{

	return	(-2)* m_Volume->voxel(x - 1, y - 1, z).getValue() +
			(-4)* m_Volume->voxel(x - 1, y, z).getValue() +
			(-2)* m_Volume->voxel(x - 1, y + 1, z).getValue() +
			2*	m_Volume->voxel(x + 1, y - 1, z).getValue() +
			4 * m_Volume->voxel(x + 1, y - 1, z).getValue() +
			2*	m_Volume->voxel(x + 1, y - 1, z).getValue();

}

//GY
// 1 2 1
// 0 0 0
//-1-2-1
int MainWindow::yGradient(int x, int y, int z)
{
	return  m_Volume->voxel(x - 1, y - 1, z).getValue() +
		2 * m_Volume->voxel(x, y - 1, z).getValue() +
			m_Volume->voxel(x + 1, y - 1, z).getValue() -
			m_Volume->voxel(x - 1, y + 1, z).getValue() -
		2 * m_Volume->voxel(x, y + 1, z).getValue() -
			m_Volume->voxel(x + 1, y + 1, z).getValue();

}
// 2 4 2
// 0 0 0
//-2-4-2
int MainWindow::yGradientMiddle(int x, int y, int z)
{
 return 2* m_Volume->voxel(x - 1, y - 1, z).getValue() +
		4* m_Volume->voxel(x, y - 1, z).getValue() +
		2* m_Volume->voxel(x + 1, y - 1, z).getValue() -
		2* m_Volume->voxel(x - 1, y + 1, z).getValue() -
		4* m_Volume->voxel(x, y + 1, z).getValue() -
		2* m_Volume->voxel(x + 1, y + 1, z).getValue();

}

//GZ
//-1-2-1
//-2-4-2
//-1-2-1
int MainWindow::zGradient(int x, int y, int z)
{
	return (-1) * m_Volume->voxel(x - 1, y - 1, z).getValue()
			- 2 * m_Volume->voxel(x, y - 1, z).getValue()
			-     m_Volume->voxel(x + 1, y - 1, z).getValue()
			- 2 * m_Volume->voxel(x - 1, y, z).getValue()
			- 4 * m_Volume->voxel(x, y, z).getValue()
			- 2 * m_Volume->voxel(x + 1, y, z).getValue()
			-     m_Volume->voxel(x - 1, y + 1, z).getValue()
			- 2 * m_Volume->voxel(x, y + 1, z).getValue()
			-     m_Volume->voxel(x + 1, y + 1, z).getValue();
}
//1 2 1
//2 4 2
//1 2 1
int MainWindow::zGradientPlus(int x, int y, int z)
{
	return  m_Volume->voxel(x - 1, y - 1, z).getValue()
		+ 2 * m_Volume->voxel(x, y - 1, z).getValue()
		+ m_Volume->voxel(x + 1, y - 1, z).getValue()
		+ 2 * m_Volume->voxel(x - 1, y, z).getValue()
		+ 4 * m_Volume->voxel(x, y, z).getValue()
		+ 2 * m_Volume->voxel(x + 1, y, z).getValue()
		+ m_Volume->voxel(x - 1, y + 1, z).getValue()
		+ 2 * m_Volume->voxel(x, y + 1, z).getValue()
		+ m_Volume->voxel(x + 1, y + 1, z).getValue();
}
