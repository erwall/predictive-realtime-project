#include "ui.h"
#include <QApplication>
#include <iostream>
#include <thread>
#include <unistd.h>

#define MINYAW (-5.0)
#define MAXYAW (5.0)
#define MINPITCH (-5.0)
#define MAXPITCH (5.0)
#define UPDATE_FREQUENCY (0.050) /* In seconds */
#define UPDATE_FREQUENCY_US (50000U) /* In microseconds */
#define TIMESCALE (30.0) /* In seconds */

/******** GUI IMPLEMENTATION ********/

GUI::GUI(regul_t *regul, QWidget *parent) : QMainWindow(parent)
{
	/* Initialization */
	this->regul = regul;
	running = true;
	central_widget = new QWidget(this);
	grid_layout = new QGridLayout(central_widget);
	yaw_label = new QLabel(QStringLiteral("yawRef:"), central_widget);
	pitch_label = new QLabel(QStringLiteral("pitchRef:"), central_widget);
	yaw_edit = new QLineEdit(QStringLiteral("0"), central_widget);
	pitch_edit = new QLineEdit(QStringLiteral("0"), central_widget);
	yaw_button = new QPushButton(QStringLiteral("Set"), central_widget);
	pitch_button = new QPushButton(QStringLiteral("Set"), central_widget);
	on_button = new QPushButton(QStringLiteral("Off"), central_widget);
	yaw_validator = new QDoubleValidator(MINYAW, MAXYAW, 2, central_widget);
	pitch_validator = new QDoubleValidator(MINPITCH, MAXPITCH, 2,
						central_widget);

	/* Window and layout settings */
	this->setCentralWidget(central_widget);
	central_widget->setLayout(grid_layout);
	this->setWindowTitle(QStringLiteral("Reference editor"));
	this->resize(300, 90);
	central_widget->setGeometry(QRect(20, 10, 260, 70));
	grid_layout->setSpacing(6);
	grid_layout->setContentsMargins(10, 10, 10, 10);

	/* Line edit settings */
	yaw_edit->setAlignment(Qt::AlignRight);
	pitch_edit->setAlignment(Qt::AlignRight);
	yaw_validator->setNotation(QDoubleValidator::StandardNotation);
	pitch_validator->setNotation(QDoubleValidator::StandardNotation);
	yaw_validator->setLocale(QLocale::c());
	pitch_validator->setLocale(QLocale::c());
	yaw_edit->setValidator(yaw_validator);
	pitch_edit->setValidator(pitch_validator);

	/* Button settings */
	on_button->setCheckable(true);

	/* Map button action */
	QObject::connect(yaw_button, SIGNAL(clicked()), SLOT(setYaw()));
	QObject::connect(pitch_button, SIGNAL(clicked()), SLOT(setPitch()));
	QObject::connect(on_button, SIGNAL(toggled(bool)), SLOT(setOn(bool)));

	/* Add widgets to layout */
	grid_layout->addWidget(yaw_label, 0, 0, 1, 1);
	grid_layout->addWidget(pitch_label, 1, 0, 1, 1);
	grid_layout->addWidget(yaw_edit, 0, 1, 1, 1);
	grid_layout->addWidget(pitch_edit, 1, 1, 1, 1);
	grid_layout->addWidget(yaw_button, 0, 2, 1, 1);
	grid_layout->addWidget(pitch_button, 1, 2, 1, 1);
	grid_layout->addWidget(on_button, 2, 1, 1, 1);
};

void GUI::setYaw()
{
	pthread_mutex_lock(regul->mutex);
	regul->yaw_ref = yaw_edit->text().toDouble();
	std::cout << regul->yaw_ref << std::endl;
	pthread_mutex_unlock(regul->mutex);
}

void GUI::setPitch()
{
	pthread_mutex_lock(regul->mutex);
	regul->pitch_ref = pitch_edit->text().toDouble();
	std::cout << regul->pitch_ref << std::endl;
	pthread_mutex_unlock(regul->mutex);
}

void GUI::setOn(bool checked)
{
	pthread_mutex_lock(regul->mutex);
	regul->on = (char)checked;
	pthread_mutex_unlock(regul->mutex);
	if (checked)
		on_button->setText(QStringLiteral("On"));
	else
		on_button->setText(QStringLiteral("Off"));
	std::cout << "Regulator is " << ((checked) ? ("on.") : ("off."));
	std::cout << std::endl;
}

void GUI::closeEvent(QCloseEvent *event)
{
	running = false;
	event->accept();
}

/******** PLOTTER IMPLEMENTATION ********/

Plotter::Plotter(data_t *data, regul_t *regul, QWidget *parent)
	: QMainWindow(parent)
{
	/* Initialization */
	this->data = data;
	this->regul = regul;
	running = true;
	u_yaw = 0;
	u_pitch = 0;
	y_yaw = 0;
	y_pitch = 0;
	ref_yaw = 0;
	ref_pitch = 0;
	first_time = 0;
	latest_time = 0;
	central_widget = new QWidget(this);
	grid_layout = new QGridLayout(central_widget);
	control_plot = new QCustomPlot(central_widget);
	measurement_plot = new QCustomPlot(central_widget);

	/* Window and layout settings */
	this->setCentralWidget(central_widget);
	central_widget->setLayout(grid_layout);
	this->setWindowTitle(QStringLiteral("Plotter"));
	this->resize(900, 600);
	central_widget->setGeometry(QRect(20, 10, 880, 580));
	grid_layout->setSpacing(6);
	grid_layout->setContentsMargins(10, 10, 10, 10);

	/* Setup plots */
	u_pitch_plot = control_plot->addGraph();
	u_pitch_plot->setPen(QPen(Qt::red));
	u_pitch_plot->setName(QStringLiteral("Pitch"));
	u_yaw_plot = control_plot->addGraph();
	u_yaw_plot->setPen(QPen(Qt::blue));
	u_yaw_plot->setName(QStringLiteral("Yaw"));
	y_pitch_plot = measurement_plot->addGraph();
	y_pitch_plot->setPen(QPen(Qt::red));
	y_pitch_plot->setName(QStringLiteral("Pitch"));
	y_yaw_plot = measurement_plot->addGraph();
	y_yaw_plot->setPen(QPen(Qt::blue));
	y_yaw_plot->setName(QStringLiteral("Yaw"));
	ref_pitch_plot = measurement_plot->addGraph();
	ref_pitch_plot->setPen(QPen(Qt::red));
	ref_pitch_plot->setName(QStringLiteral("Pitch ref."));
	ref_yaw_plot = measurement_plot->addGraph();
	ref_yaw_plot->setPen(QPen(Qt::blue));
	ref_yaw_plot->setName(QStringLiteral("Yaw ref."));
	control_plot->xAxis->setLabel("Time (s)");
	control_plot->yAxis->setLabel("Voltage (V)");
	control_plot->xAxis->setRange(0.0, TIMESCALE);
	control_plot->yAxis->setRange(-10.0, 10.0);
	control_plot->setLocale(QLocale::c());
	control_plot->legend->setVisible(true);
	control_plot->axisRect()->insetLayout()->setInsetAlignment(
					0, Qt::AlignTop|Qt::AlignLeft);
	control_plot->plotLayout()->insertRow(0);
	control_plot->plotLayout()->addElement(0, 0,
			new QCPPlotTitle(control_plot, "Control signal"));
	measurement_plot->xAxis->setLabel("Time (s)");
	measurement_plot->yAxis->setLabel("Angle sensor reading (V)");
	measurement_plot->xAxis->setRange(0.0, TIMESCALE);
	measurement_plot->yAxis->setRange(MINYAW, MAXYAW);
	measurement_plot->setLocale(QLocale::c());
	measurement_plot->legend->setVisible(true);
	measurement_plot->axisRect()->insetLayout()->setInsetAlignment(
					0, Qt::AlignTop|Qt::AlignLeft);
	measurement_plot->plotLayout()->insertRow(0);
	measurement_plot->plotLayout()->addElement(0, 0,
			new QCPPlotTitle(measurement_plot,
					"Measurement and reference"));

	/* Add widgets to layout */
	grid_layout->addWidget(control_plot, 0, 0, 1, 1);
	grid_layout->addWidget(measurement_plot, 1, 0, 1, 1);
}

void Plotter::replot()
{
	/* Read data from regulator */
	pthread_mutex_lock(data->mutex);
	u_yaw = data->u_yaw;
	u_pitch = data->u_pitch;
	y_yaw = data->y_yaw;
	y_pitch = data->y_pitch;
	pthread_mutex_unlock(data->mutex);

	/* Read references */
	pthread_mutex_lock(regul->mutex);
	ref_yaw = regul->yaw_ref;
	ref_pitch = regul->pitch_ref;
	pthread_mutex_unlock(regul->mutex);

	/* Add new datapoint in all plots */
	u_yaw_plot->addData(latest_time, u_yaw);
	u_pitch_plot->addData(latest_time, u_pitch);
	y_yaw_plot->addData(latest_time, y_yaw);
	y_pitch_plot->addData(latest_time, y_pitch);
	ref_yaw_plot->addData(latest_time, ref_yaw);
	ref_pitch_plot->addData(latest_time, ref_pitch);
	latest_time += UPDATE_FREQUENCY;

	if (latest_time > TIMESCALE) {
		/* Remove oldest datapoint in all plots */
		u_yaw_plot->removeData(first_time);
		u_pitch_plot->removeData(first_time);
		y_yaw_plot->removeData(first_time);
		y_pitch_plot->removeData(first_time);
		ref_yaw_plot->removeData(first_time);
		ref_pitch_plot->removeData(first_time);
		first_time += UPDATE_FREQUENCY;

		/* Update axis ranges */
		control_plot->xAxis->setRange(first_time, latest_time);
		measurement_plot->xAxis->setRange(first_time, latest_time);
	}

	/* Refresh plot */
	control_plot->replot();
	measurement_plot->replot();
}

void Plotter::closeEvent(QCloseEvent *event)
{
	running = false;
	event->accept();
}

void runGUI(thread_args_t *thread_args)
{
	int argc(1);
	char *argv[1];
	*argv = '\0';
	/* Create Qt application and windows */
	QApplication app(argc, argv);
	GUI gui(thread_args->regul);
	Plotter plotter(thread_args->data, thread_args->regul);

	/* Position windows */
	plotter.setGeometry(QStyle::alignedRect(Qt::LeftToRight,
				Qt::AlignCenter, plotter.size(),
				app.desktop()->availableGeometry(&plotter)));

	gui.show();
	plotter.show();

	while(gui.running && plotter.running) {
		app.processEvents();
		plotter.replot();
		usleep(UPDATE_FREQUENCY_US);
	}
	app.quit();
	thread_args->run = 0;
}

void *run_gui(void *arg)
{
	runGUI((thread_args_t*)arg);
	return NULL;
}
