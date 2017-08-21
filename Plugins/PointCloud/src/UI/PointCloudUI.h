#ifndef POINTCLOUDUI_H
#define POINTCLOUDUI_H

#include <QFrame>

namespace Ui
{
    class PointCloudUI;
}

namespace PointCloudPlugin
{
    class PointCloudPluginC;
}

class PointCloudUI : public QFrame
{
    Q_OBJECT
    friend class PointCloudPlugin::PointCloudPluginC;

public:
    explicit PointCloudUI(QWidget *parent = 0);
    ~PointCloudUI();

signals:
    void changeNeighSize(int size);
    void changeUseNormal(bool useNormal);
    void changeDepthThresh(double dThresh);
    void changeDepthCalc(int index);

private slots:
    void on_NeighSize_valueChanged(int arg1);
    void on_UseNormal_clicked(bool checked);
    void on_DepthTreshold_valueChanged(double arg1);
    void on_DepthCalc_currentIndexChanged(int index);

private:
    Ui::PointCloudUI *ui;
};

#endif // POINTCLOUDUI_H
