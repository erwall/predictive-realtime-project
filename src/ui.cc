#include "ui.h"
#include <QApplication>
#include <iostream>

#define MINYAW (0.0)
#define MAXYAW (3.14)
#define MINPITCH (0.0)
#define MAXPITCH (3.14)

GUI::GUI(thread_args_t *thread_args, QWidget *parent) : QMainWindow(parent)
{
	/* Object initialization */
	regul = thread_args->regul;
	data = thread_args->data;
	central_widget = new QWidget(this);
	grid_layout = new QGridLayout(central_widget);
	yaw_label = new QLabel("yawRef:");
	pitch_label = new QLabel("pitchRef:");
	yaw_edit = new QLineEdit("0");
	pitch_edit = new QLineEdit("0");
	yaw_button = new QPushButton("Set");
	pitch_button = new QPushButton("Set");
	yaw_validator = new QDoubleValidator(MINYAW, MAXYAW, 2);
	pitch_validator = new QDoubleValidator(MINPITCH, MAXPITCH, 2);

	/* Window and layout settings */
	this->setWindowTitle(QStringLiteral("Reference editor"));
	this->resize(300, 90);
	central_widget->setGeometry(QRect(20, 10, 260, 70));
	grid_layout->setSpacing(6);
	grid_layout->setContentsMargins(11, 11, 11, 11);
	grid_layout->setContentsMargins(0, 0, 0, 0);

	/* Line edit settings */
	yaw_edit->setAlignment(Qt::AlignRight);
	pitch_edit->setAlignment(Qt::AlignRight);
	yaw_validator->setNotation(QDoubleValidator::StandardNotation);
	pitch_validator->setNotation(QDoubleValidator::StandardNotation);
	yaw_validator->setLocale(QLocale::c());
	pitch_validator->setLocale(QLocale::c());
	yaw_edit->setValidator(yaw_validator);
	pitch_edit->setValidator(pitch_validator);

	/* Map button action */
	QObject::connect(yaw_button, SIGNAL(clicked()), SLOT(setYaw()));
	QObject::connect(pitch_button, SIGNAL(clicked()), SLOT(setPitch()));

	/* Add widgets to layout */
	grid_layout->addWidget(yaw_label, 0, 0, 1, 1);
	grid_layout->addWidget(pitch_label, 1, 0, 1, 1);
	grid_layout->addWidget(yaw_edit, 0, 1, 1, 1);
	grid_layout->addWidget(pitch_edit, 1, 1, 1, 1);
	grid_layout->addWidget(yaw_button, 0, 2, 1, 1);
	grid_layout->addWidget(pitch_button, 1, 2, 1, 1);
};

GUI::~GUI()
{
	delete yaw_label;
	delete pitch_label;
	delete yaw_button;
	delete pitch_button;
	delete yaw_validator;
	delete pitch_validator;
	delete yaw_edit;
	delete pitch_edit;
	delete grid_layout;
	delete central_widget;
}

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

void runGUI(thread_args_t *thread_args)
{
	int argc(1);
	char *argv[1];
	QApplication app(argc, argv);
	GUI gui(thread_args);

	gui.show();
	app.exec();

	thread_args->run = 0;
}

void *run_gui(void *arg)
{
	runGUI((thread_args_t*) arg);
	return NULL;
}
