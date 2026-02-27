#ifndef ENEMYEDIT_H
#define ENEMYEDIT_H

#include <QComboBox>
#include <QMainWindow>
#include "cereal/cereal.hpp"
#include "cereal/archives/json.hpp"
#include "file/data/file_data_enemy.hpp"

namespace Ui {
class EnemyEdit;
}

class EnemyEdit : public QMainWindow
{
    Q_OBJECT

public:
    explicit EnemyEdit(QWidget *parent = nullptr);
    ~EnemyEdit();
    void start(std::string data_directory);
    void loadData();

public slots:
    void handleSelectorIndexChanged(int index);

private slots:
    void on_actionAdd_triggered();
    void on_actionSave_triggered();
    void on_nameLineEdit_textChanged(const QString &arg1);
    void on_HPSpinBox_valueChanged(int arg1);
    void on_shieldTypeComboBox_currentIndexChanged(int index);
    void on_moveSpeedHorizontalDoubleSpinBox_valueChanged(double arg1);
    void on_jumpSpeedDoubleSpinBox_valueChanged(double arg1);
    void on_rangeSpinBox_valueChanged(int arg1);
    void on_GraphicFilenameComboBox_currentTextChanged(const QString &arg1);
    void on_spriteSizeWidthSpinBox_valueChanged(int arg1);
    void on_spriteSizeHeightSpinBox_valueChanged(int arg1);

    void on_projectileOriginXSpinBox_valueChanged(int arg1);

    void on_projectileOriginYSpinBox_valueChanged(int arg1);

private:
    void fillSelectorCombobox();
    void fillFormWithData(int selected_enemy);

private:
    Ui::EnemyEdit *ui;
    data::file_enemies enemies;
    QComboBox* selectorCombobox;
    std::string game_data_directory;
};

#endif // ENEMYEDIT_H
