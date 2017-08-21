#include "PointCloudUI.h"
#include "ui_PointCloudUI.h"
#include <cstdlib>

PointCloudUI::PointCloudUI(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::PointCloudUI)
{
    ui->setupUi(this);
}

PointCloudUI::~PointCloudUI()
{
    delete ui;
}

void PointCloudUI::on_NeighSize_valueChanged(int x)
{
    emit changeNeighSize(x);
}

void PointCloudUI::on_ShowPos_clicked(bool checked)
{
     emit changeShowPos(checked);
}

void PointCloudUI::on_planeFit_clicked(bool checked)
{
    emit changePlaneFit(checked);
}

void PointCloudUI::on_DepthTreshold_valueChanged(double x)
{
    emit changeDepthThresh(x);
}

void PointCloudUI::on_Radius_textChanged(const QString st)
{
    bool ok;
    double x = st.toDouble(&ok);
    if (!ok) x = 0;
    emit changeRadius(x);
}


void PointCloudUI::on_DepthCalc_currentIndexChanged(int index)
{
    emit changeDepthCalc(index);
}
