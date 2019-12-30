#include <QtCore/QCoreApplication>
#include <QDebug>
#include <math.h>

const int allOs=60;
const int timePoints=16;
double a0(int numPoints, double pos[allOs])
{
  double sum=0;
  for(int i=0;i<allOs;i++){
    sum+=pos[i];
  }
  return(sum/numPoints);
}

double ai(int numPoints,int harm, double pos[allOs])
{
  double sum=0;
  for(int i=0;i<allOs;i++){
    sum+=pos[i]*cos(harm*2*M_PI*i/allOs);
  }
  return(sum*2/numPoints);
}
double bi(int numPoints,int harm, double pos[allOs])
{
  double sum=0;
  for(int i=0;i<allOs;i++){
    sum+=pos[i]*sin(harm*2*M_PI*i/allOs);
  }
  return(sum*2/numPoints);
}

void four(double data[allOs][timePoints],double radius[timePoints])
{
  double dd[allOs];
  for(int i=0;i<timePoints;i++){
    for(int j=0;j<allOs;j++) dd[j]=data[j][i];
    radius[i]=a0(60,dd);
  }
}
int main(int argc, char *argv[])
{

    QCoreApplication a(argc, argv);

    double da[allOs][timePoints],r[timePoints];
    for(int i=0;i<allOs;i++)
     for(int j=0;j<timePoints;j++)
       da[i][j]=j;
    double *dd=&da[0][0];
    qDebug()<<"dd"<<*(dd+timePoints*2+1)<<da[2][1];
    qDebug()<<"dsdc";
    four(da,r);
  //  qDebug()<<r;
    qDebug()<<r[0]<<r[1]<<r[2];
    return a.exec();
}
