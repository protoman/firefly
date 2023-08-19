#include "npcdialogeditor.h"
#include "ui_npcdialogeditor.h"


NpcDialogEditor::NpcDialogEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NpcDialogEditor)
{
    ui->setupUi(this);
}

NpcDialogEditor::~NpcDialogEditor()
{
    delete ui;
}

void NpcDialogEditor::set_npc_id(int id)
{
    npc_id = id;
    std::string currentText = fio.load_npc_dialog(0, npc_id);
    QString text = QString(currentText.c_str());
    ui->textEdit->setPlainText(text);
}


void NpcDialogEditor::on_buttonBox_accepted()
{
    if (npc_id == -1) {
        return;
    }
    fio.save_npc_dialog(ui->languageSelectorComboBox->currentIndex(), npc_id, ui->textEdit->toPlainText().toStdString());
}

