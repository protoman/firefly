#ifndef ENEMYGRAPHICPREVIEWAREAWIDGET_H
#define ENEMYGRAPHICPREVIEWAREAWIDGET_H

#include <QWidget>

struct st_selected_point {
    int x = 0;
    int y = 0;
};

class EnemyGraphicPreviewAreaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EnemyGraphicPreviewAreaWidget(QWidget *parent = nullptr);
    void updateGraphic(int w, int h, std::string filename);
    void setDataDirectory(std::string directory);
    st_selected_point get_st_selected_point();

protected:
    void paintEvent(QPaintEvent *event);
    void mouseReleaseEvent(QMouseEvent * event);

signals:

private:
    QWidget *myParent;
    int zoom = 3;
    int size_w = 16;
    int size_h = 16;
    std::string graphic_filename;
    QPixmap graphic_image;
    std::string game_data_directory;
    st_selected_point selected_point;
};

#endif // ENEMYGRAPHICPREVIEWAREAWIDGET_H
