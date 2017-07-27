#include "PointCloudUI.h"
#include "ui_PointCloudUI.h"

PointCloudUI::PointCloudUI(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::PointCloudUI)
{
    ui->setupUi(this);
    ui->UseNormal->setProperty("pressed", true);
}

PointCloudUI::~PointCloudUI()
{
    delete ui;
}

void PointCloudUI::on_NeighSize_valueChanged(int x)
{
    emit changeNeighSize(x);
}

void PointCloudUI::on_UseNormal_clicked(bool checked)
{
    emit changeUseNormal(checked);
}
void PointCloudUI::on_DepthTreshold_valueChanged(double x)
{
    emit changeDepthThresh(x);
}

