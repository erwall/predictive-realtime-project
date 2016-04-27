#ifndef UI_H_
#define UI_H_

#include <QMainWindow>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDoubleValidator>
#include <pthread.h>
#include "run_gui.h"
#include "regulator.h"

class GUI : public QMainWindow
{
	Q_OBJECT
public:
	explicit GUI(thread_args_t *thread_args, QWidget *parent = 0);
	~GUI();
public slots:
	void setYaw();
	void setPitch();
private:
	QWidget *central_widget;
	QGridLayout *grid_layout;
	QLabel *yaw_label, *pitch_label;
	QLineEdit *yaw_edit, *pitch_edit;
	QPushButton *yaw_button, *pitch_button;
	QDoubleValidator *yaw_validator, *pitch_validator;
	regul_t *regul;
	data_t *data;

};

#endif /* UI_H_ */
