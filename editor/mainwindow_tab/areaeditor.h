#ifndef AREAEDITOR_H
#define AREAEDITOR_H

#include <QWidget>

namespace Ui {
class AreaEditor;
}

class AreaEditor : public QWidget
{
    Q_OBJECT

public:
    explicit AreaEditor(QWidget *parent = nullptr);
    ~AreaEditor();

private:
    Ui::AreaEditor *ui;
};

#endif // AREAEDITOR_H
