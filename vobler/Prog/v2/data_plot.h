#ifndef _DATA_PLOT_H
#define _DATA_PLOT_H
#include <QtGui>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>

class TDataPlot : public QwtPlot
{
    Q_OBJECT
public:
    TDataPlot(int nGr=1,QWidget* = NULL,QString title="Current");
    QVector <QwtPlotCurve*> massGr;
    QwtPlotGrid *grid;
    void setGridVisible(bool);
    void setDataForGraph(int numGr,int xySize,QRgb color,const double *dataX, const double *dataY);
    void setAxis(void);
    void setVisibleMap(const bool map[]);
    void replotGraph(void);
private:
    void alignScales();
    int numGraph;
    int dim;
};

#endif
