#pragma once

#include <QMainWindow>
#include <QFileDialog>
#include <QString>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>
#include <QTimer>
#include <QDateTime>

#include <rosbag2_cpp/readers/sequential_reader.hpp>
#include <rosidl_typesupport_introspection_cpp/message_introspection.hpp>
#include <rosbag2_cpp/typesupport_helpers.hpp>
#include "newprojectdialog.h"
#include <iostream>
#include "geometry_msgs/msg/pose.hpp"
#include "rosbag_parser.h"

#include <memory>


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

    void on_exportBtn_clicked();

private:
    Ui::MainWindow *ui;
    QPalette status_bar_palette_;
    QTimer cooldown_timer_;

    std::shared_ptr<Rosbag2Parser> parser_;

    std::unordered_set<std::string> topic_whitelist_;
    std::map<std::string, std::string> topic_rename_;
    QString input_path_;

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
    void enableSaveButton();

};
