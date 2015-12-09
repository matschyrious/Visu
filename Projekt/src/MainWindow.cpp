
#include "MainWindow.h"

#include <QFileDialog>
#include <QPainter>
#include <QApplication>
#include <QMessageBox>
#include <QtOpenGL>
#include <QGLWidget>
#include <QGLBuffer>
#include <QGLShader>

#include <gl\GL.h>


class GLWidget : public QGLWidget
{
public:
	GLWidget(const QGLFormat& format, QWidget *parent, Volume volume);

protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();

	virtual void keyPressEvent(QKeyEvent * e);

private:
	QGLShaderProgram program;
	QGLBuffer m_vertexBuffer;
	Volume volume;
	QMatrix4x4 matrix;
	std::vector<float> volumeData;
	std::vector<float> texCoords;
	std::vector<GLfloat> vertices;
};

GLWidget::GLWidget(const QGLFormat& format, QWidget *parent, Volume volume)
	: QGLWidget(format), volume(volume), m_vertexBuffer(QGLBuffer::VertexBuffer)
{
	setWindowTitle(tr("GPU Raycasting"));

}

void GLWidget::initializeGL()
{
	float halfW = volume.width() / 2;
	float halfH = volume.height() / 2;
	float halfD = volume.depth() / 2;

	vertices;
	
	for (int x = 0; x < volume.width(); x++){
		for (int y = 0; y < volume.height(); y++){
			for (int z = 0; z < volume.depth(); z++){
				vertices.push_back((float)(-halfW + x) / halfW);
				vertices.push_back((float)(-halfH + y) / halfH);
				vertices.push_back((float)(-halfD + z) / halfD);
				vertices.push_back(1.0);
				volumeData.push_back(volume.voxel(x, y, z).getValue());
				/*texCoords.push_back(x);
				texCoords.push_back(y);
				texCoords.push_back(z);*/
			}
		}
	}

	
	if (!QGLWidget::format().sampleBuffers())
		qWarning() << "Could not enable sample buffers";

	// Set the clear color to black
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	// Prepare a complete shader program…
	program.addShaderFromSourceCode(QGLShader::Vertex,
		"#version 330 \n"
		"in vec4 vertex;\n"
		//"in vec3 texCoords;\n"
		"uniform mat4 matrix;\n"
		"out vec3 texCoordsV;\n"
		"void main(void)\n"
		"{ \n"
		"vec4 n_vertex = normalize(vertex);\n"
		"texCoordsV = vec3(n_vertex);\n"
		"gl_Position = matrix *n_vertex;\n"
		"}");
	program.addShaderFromSourceCode(QGLShader::Fragment,
		"#version 330 \n"
		"in vec3 texCoordsV;\n"
		"layout(location = 0, index = 0) out vec4 fragColor;\n"
		"uniform sampler1D intensityTexture;\n"
		"void main(void)\n"
		"{ \n"
		"vec4 intensity = texture(intensityTexture, texCoordsV.x+texCoordsV.y+texCoordsV.z);\n"
		"fragColor = vec4(1.0,0,0, 1.0);\n"
		"}");

	program.link();

	m_vertexBuffer.create();
	m_vertexBuffer.setUsagePattern(QGLBuffer::StaticDraw);

	if (!m_vertexBuffer.bind())
	{
		qWarning() << "Could not bind vertex buffer to the context";
		return;
	}
	m_vertexBuffer.allocate(&vertices[0], vertices.size()* sizeof(float));

	// Bind the shader program so that we can associate variables from
	// our application to the shaders
	if (!program.bind())
	{
		qWarning() << "Could not bind shader program to context";
		return;
	}

	// Enable the "vertex" attribute to bind it to our currently bound
	// vertex buffer.
	matrix = QMatrix4x4(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
		
	program.setUniformValue("matrix", matrix);
	program.setAttributeBuffer("vertex", GL_FLOAT, 0, 4);
	program.enableAttributeArray("vertex");
	QGLFunctions glFuncs(QGLContext::currentContext());
	m_vertexBuffer.bind();
	

}

void GLWidget::resizeGL(int w, int h)
{
	// Set the viewport to window dimensions
	glViewport(0, 0, w, qMax(h, 1));
}

void GLWidget::paintGL()
{

	// Clear the buffer with the current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*Texture
	glEnable(GL_TEXTURE_3D);
	QGLFunctions glFuncs(QGLContext::currentContext());
	glFuncs.glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, volumeData[0]);*/
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_1D, texture);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, volume.width() * volume.height() * volume.depth(), 0, GL_RGBA, GL_FLOAT, volumeData.data());


	// Draw stuff

	glDrawArrays(GL_POINTS, 0, volume.size());

}

void GLWidget::keyPressEvent(QKeyEvent* e)
{
	switch (e->key())
	{
	case Qt::Key_Escape:
		QCoreApplication::instance()->quit();
		break;

	case Qt::Key_X:
		matrix.rotate(10, 1,0,0);
		updateGL();
		break;
	case Qt::Key_Y:
		matrix.rotate(10, 0, 1, 0);
		updateGL();
		break;
	case Qt::Key_Z:
		matrix.rotate(10, 0, 0, 1);
		updateGL();
		break;
	default:
		QGLWidget::keyPressEvent(e);
	}
}




MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent), m_Volume(0), m_VectorField(0)
{
	m_Ui = new Ui_MainWindow();
	m_Ui->setupUi(this);

	connect(m_Ui->actionOpen, SIGNAL(triggered()), this, SLOT(openFileAction()));
	connect(m_Ui->actionClose, SIGNAL(triggered()), this, SLOT(closeAction()));
	connect(m_Ui->pushButtonCPU, SIGNAL(clicked()), this, SLOT(cpuRaycasting()));
	connect(m_Ui->pushButtonGPU, SIGNAL(clicked()), this, SLOT(gpuRaycasting()));

	if (m_Volume == nullptr){
		m_Ui->pushButtonCPU->setDisabled(true);
		m_Ui->pushButtonGPU->setDisabled(true);
	}
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

			m_Ui->pushButtonCPU->setDisabled(false);
			m_Ui->pushButtonGPU->setDisabled(false);

			//calculateGradient();
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

	mipData = new std::vector<float>(m_Volume->width()*m_Volume->height(), 0);

	QImage mip = QImage(m_Volume->width(), m_Volume->height(), QImage::Format::Format_RGB32);

	QImage alphaImg = QImage(m_Volume->width(), m_Volume->height(), QImage::Format::Format_ARGB32);
	QImage alphaImgA = alphaImg.alphaChannel();


	for (int x = 0; x < m_Volume->width(); x++){
		for (int y = 0; y < m_Volume->height(); y++){

			float alpha = 0;
			float prevAlpha = 0;
			float r = 0;
			float g = 0;
			float b = 0;

			for (int d = 0; d < m_Volume->depth(); d++){

				float voxel = m_Volume->voxel(x, y, d).getValue();

				
				//MIP
				mipData->at(x + (y*m_Volume->width())) =
					std::fmax(mipData->at(x + (y*m_Volume->width())), voxel);
				


				//alpha 
				
				if (alpha < 255){
					if (voxel > 0){
						r = (255 * voxel);
						g = 255 - (255 * voxel);
						b = 255- (255 * voxel);

						alphaImg.setPixel(x, y, alphaImg.pixel(x,y)+qRgb(r,g,b));
						
						float actAlpha = alpha;
						alpha += (255 - prevAlpha) * voxel;
						prevAlpha = (255-actAlpha) * voxel;
						
						if (alpha <= 255){
							alphaImgA.setPixel(x, y, alpha);
						}
						else{
							alphaImgA.setPixel(x, y, 255);
						}
						
						alphaImg.setAlphaChannel(alphaImgA);
					}
				}
			}

			

			float intensity = mipData->at(x + (y*m_Volume->width()));
			mip.setPixel(x, y, qRgb(0, 0, 255*intensity));
			

		}
	}

	
	m_Ui->labelAlpha->setPixmap(QPixmap::fromImage(alphaImg.scaled(alphaImg.size()*2, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
	m_Ui->labelAlpha->setFixedSize(alphaImg.size()*2);

	m_Ui->label->setPixmap(QPixmap::fromImage(mip.scaled(mip.size()*2, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
	m_Ui->label->setFixedSize(mip.size()*2);

}

void MainWindow::gpuRaycasting(){

	if (m_Volume->size() > 0){
		QGLFormat f = QGLFormat::defaultFormat();
		f.setSampleBuffers(true);
		QGLFormat::setDefaultFormat(f);
		if (!QGLFormat::hasOpenGL()) {
			QMessageBox::information(0, "OpenGL samplebuffers",
				"This system does not support OpenGL.");
		}

		QGLFormat glFormat;
		glFormat.setVersion(3, 3);
		glFormat.setProfile(QGLFormat::CoreProfile); // Requires >=Qt-4.8.0
		glFormat.setSampleBuffers(true);

		// Create a GLWidget requesting our format
		GLWidget widget(glFormat, 0, *m_Volume);

		if (!widget.format().sampleBuffers()) {
			QMessageBox::information(0, "OpenGL samplebuffers",
				"This system does not have sample buffer support.");
		}

		widget.resize(640, 480);
		
		do{
			widget.show();
			qApp->processEvents();
		} while (widget.isVisible());
		
	}
}


void MainWindow::calculateGradient(){
	int gx, gy,gz;
	float sum;

	QImage gradient = QImage(m_Volume->width(), m_Volume->height(), QImage::Format::Format_RGB32);
	//gradient_Volume = *m_Volume;
	gradient_Volume = new std::vector<float>(m_Volume->width()*m_Volume->height()*m_Volume->depth(),0);
	
	for (int x = 1; x < m_Volume->width()-1; x++){
		for (int y = 1; y < m_Volume->height()-1; y++){
			for (int z = 1; z < m_Volume->depth()-1; z++){

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
				if (sum > 0){
					gradient_Volume->at(x + (y *m_Volume->width()) + (z*m_Volume->width()*m_Volume->height())) = sum;
					gradient.setPixel(x, y, qRgb(0, 255, 0));
				}
				
			}
		}
	}

	m_Ui->labelGradient->setPixmap(QPixmap::fromImage(gradient));
	m_Ui->labelGradient->setFixedSize(gradient.size());

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
