#ifndef NPCDIALOGEDITOR_H
#define NPCDIALOGEDITOR_H

#include <QDialog>
#include "file/fio_strings.h"

namespace Ui {
class NpcDialogEditor;
}

class NpcDialogEditor : public QDialog
{
    Q_OBJECT

public:
    explicit NpcDialogEditor(QWidget *parent = nullptr);
    ~NpcDialogEditor();
    void set_npc_id(int id);


private slots:
    void on_buttonBox_accepted();

private:
    Ui::NpcDialogEditor *ui;
    int npc_id = -1;
    fio_strings fio;
};

#endif // NPCDIALOGEDITOR_H
