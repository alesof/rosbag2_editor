#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QString>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>

#include <rosbag2_cpp/readers/sequential_reader.hpp>
#include <rosbag2_cpp/typesupport_helpers.hpp>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_loadBtn_clicked();

    void on_applyButton_clicked();

    void on_inputList_itemSelectionChanged();

    void on_removeButton_clicked();

    void on_saveBtn_clicked();

    void on_actionOpen_Directory_triggered();

    void onTreeItemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_actionOpen_Rosbag_triggered();

    void on_outputList_itemChanged(QTableWidgetItem *item);

    void on_actionrosbag2csv_triggered();

    void on_actionSave_triggered();

    void on_outBeginTime_dateTimeChanged(const QDateTime &dateTime);

    void on_outEndTime_dateTimeChanged(const QDateTime &dateTime);

    void on_actionContacts_triggered();

private:
    Ui::MainWindow *ui;
    QPalette status_bar_palette_;

    rosbag2_cpp::readers::SequentialReader reader;
    std::unordered_set<std::string> topic_whitelist_;
    std::map<std::string, std::string> topic_rename_;

    QDateTime endDateTime_;
    QDateTime startDateTime_;

    QDateTime trimStart_;
    QDateTime trimEnd_;

    int out_rows_;

    void populateTreeWidget(const QString &path);

    void load(const QString& filePath);
    void getTimeInfo();

    void extractRosbagMetadata(const QString& file_path);
    void setPaletteError();
    void setPaletteNormal();
    void setPaletteOk();

};
#endif // MAINWINDOW_H
