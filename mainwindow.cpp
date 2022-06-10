#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->Qcp->legend->setVisible(true);  //legend:图例;显示图例
    ui->Qcp->legend->setFont(QFont("Helvetica",9));  //设置图例字体和字号
    // set locale to english, so we get english decimal separator:
    ui->Qcp->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));  //区域设置
    // add confidence band graphs:
    ui->Qcp->addGraph();  //添加图表
    QPen *pen = new QPen;
    pen->setStyle(Qt::DotLine);
    pen->setWidth(1);
    pen->setColor(QColor(180,180,180));
    ui->Qcp->graph(0)->setName("Confidence Band 68%");
    ui->Qcp->graph(0)->setPen(*pen);
    //ui->Qcp->graph(0)->setBrush(QBrush(QColor(255,50,30,20)));

    ui->Qcp->addGraph();
    ui->Qcp->legend->removeItem(ui->Qcp->legend->itemCount()-1); // don't show two confidence band graphs in legend
    ui->Qcp->graph(1)->setPen(*pen);
    ui->Qcp->graph(0)->setChannelFillGraph(ui->Qcp->graph(1));
    // add theory curve graph:
    ui->Qcp->addGraph();
    pen->setStyle(Qt::DashLine);
    pen->setWidth(2);
    pen->setColor(Qt::red);
    ui->Qcp->graph(2)->setPen(*pen);
    ui->Qcp->graph(2)->setName("Theory Curve");
    // add data point graph:
    ui->Qcp->addGraph();
    ui->Qcp->graph(3)->setPen(QPen(Qt::blue));
    ui->Qcp->graph(3)->setLineStyle(QCPGraph::lsNone);
    ui->Qcp->graph(3)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 4));
    // add error bars:
    QCPErrorBars *errorBars = new QCPErrorBars(ui->Qcp->xAxis, ui->Qcp->yAxis);
    errorBars->removeFromLegend();
    errorBars->setAntialiased(false);
    errorBars->setDataPlottable(ui->Qcp->graph(3));
    errorBars->setPen(QPen(QColor(180,180,180)));
    ui->Qcp->graph(3)->setName("Measurement");

    // generate ideal sinc curve data and some randomly perturbed data for scatter plot:
    QVector<double> x0(250), y0(250);
    QVector<double> yConfUpper(250), yConfLower(250);
    for (int i=0; i<250; ++i)
    {
      x0[i] = (i/249.0-0.5)*30+0.01; // by adding a small offset we make sure not do divide by zero in next code line
      y0[i] = qSin(x0[i])/x0[i]; // sinc function
      yConfUpper[i] = y0[i]+0.15;
      yConfLower[i] = y0[i]-0.15;
      x0[i] *= 1000;
    }
    QVector<double> x1(50), y1(50), y1err(50);
    for (int i=0; i<50; ++i)
    {
      // generate a gaussian distributed random number:
      double tmp1 = rand()/(double)RAND_MAX;
      double tmp2 = rand()/(double)RAND_MAX;
      double r = qSqrt(-2*qLn(tmp1))*qCos(2*M_PI*tmp2); // box-muller transform for gaussian distribution
      // set y1 to value of y0 plus a random gaussian pertubation:
      x1[i] = (i/50.0-0.5)*30+0.25;
      y1[i] = qSin(x1[i])/x1[i]+r*0.15;
      x1[i] *= 1000;
      y1err[i] = 0.15;
    }
    // pass data to graphs and let QCustomPlot determine the axes ranges so the whole thing is visible:
    ui->Qcp->graph(0)->setData(x0, yConfUpper);
    ui->Qcp->graph(1)->setData(x0, yConfLower);
    ui->Qcp->graph(2)->setData(x0, y0);
    ui->Qcp->graph(3)->setData(x1, y1);
    errorBars->setData(y1err);
    ui->Qcp->graph(2)->rescaleAxes();
    ui->Qcp->graph(3)->rescaleAxes(true);
    // setup look of bottom tick labels:
    ui->Qcp->xAxis->setTickLabelRotation(30);
    ui->Qcp->xAxis->ticker()->setTickCount(9);
    ui->Qcp->xAxis->setNumberFormat("ebc");
    ui->Qcp->xAxis->setNumberPrecision(1);
    ui->Qcp->xAxis->moveRange(-10);
    // make top right axes clones of bottom left axes. Looks prettier:
    ui->Qcp->axisRect()->setupFullAxesBox();
}

MainWindow::~MainWindow()
{
    delete ui;
}

