#ifndef WIDGETSEARCHRESULTS_H
#define WIDGETSEARCHRESULTS_H

#include <QMainWindow>

namespace Ui {
    class WidgetSearchResults;
}

class WidgetSearchResults : public QMainWindow {
    Q_OBJECT
public:
    WidgetSearchResults(QWidget *parent = 0);
    ~WidgetSearchResults();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetSearchResults *ui;
};

#endif // WIDGETSEARCHRESULTS_H
