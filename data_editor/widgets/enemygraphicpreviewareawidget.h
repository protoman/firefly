#ifndef ENEMYGRAPHICPREVIEWAREAWIDGET_H
#define ENEMYGRAPHICPREVIEWAREAWIDGET_H

#include <QWidget>

class EnemyGraphicPreviewAreaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EnemyGraphicPreviewAreaWidget(QWidget *parent = nullptr);
    void updateGraphic(int w, int h, std::string filename);
    void setDataDirectory(std::string directory);

protected:
    void paintEvent(QPaintEvent *event);


signals:

private:
    QWidget *myParent;
    int zoom = 3;
    int size_w = 16;
    int size_h = 16;
    std::string graphic_filename;
    QPixmap graphic_image;
    std::string game_data_directory;
};

#endif // ENEMYGRAPHICPREVIEWAREAWIDGET_H
