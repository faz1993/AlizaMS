#ifndef HELPWIDGET___H
#define HELPWIDGET___H

#include "ui_helpwidget.h"
#include <QWidget>
#ifdef __APPLE__
#include <QShortcut>
#endif

class HelpWidget: public QWidget, private Ui::HelpWidget
{
Q_OBJECT
public:
	HelpWidget();
	~HelpWidget();

private:
	void set_text();
#ifdef __APPLE__
	QShortcut * minimaze_sc;
	QShortcut * fullsceen_sc;
	QShortcut * normal_sc;
#endif
};

#endif // HELPWIDGET___H
