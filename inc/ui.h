#ifndef UI_H_
#define UI_H_

#include <QMainWindow>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDoubleValidator>
#include <pthread.h>
#include "qcustomplot.h"
#include "run_gui.h"
#include "regulator.h"

class GUI : public QMainWindow
{
	Q_OBJECT
public:
	explicit GUI(regul_t *regul, QWidget *parent = 0);
	void closeEvent(QCloseEvent *event);
	bool running;
public slots:
	void setYaw();
	void setPitch();
	void setOn(bool checked);
private:
	QWidget *central_widget;
	QGridLayout *grid_layout;
	QLabel *yaw_label, *pitch_label;
	QLineEdit *yaw_edit, *pitch_edit;
	QPushButton *yaw_button, *pitch_button, *on_button;
	QDoubleValidator *yaw_validator, *pitch_validator;
	regul_t *regul;
};

class Plotter : public QMainWindow
{
	Q_OBJECT
public:
	explicit Plotter(data_t *data, regul_t *regul, QWidget *parent = 0);
	void closeEvent(QCloseEvent *event);
	void replot();
	bool running;
private:
	QWidget *central_widget;
	QGridLayout *grid_layout;
	QCustomPlot *control_plot, *measurement_plot;
	QCPGraph *u_yaw_plot, *u_pitch_plot, *y_yaw_plot, *y_pitch_plot,
		*ref_yaw_plot, *ref_pitch_plot;
	data_t *data;
	regul_t *regul;
	double u_yaw, u_pitch, y_yaw, y_pitch, ref_yaw, ref_pitch, first_time,
		latest_time;
};

#endif /* UI_H_ */
