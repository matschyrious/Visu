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
