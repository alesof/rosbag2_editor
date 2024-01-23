#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "newprojectdialog.h"

#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    status_bar_palette_ = statusBar()->palette();

    ui->inputList->setRowCount(0);
    ui->inputList->setColumnCount(2);

    ui->inputList->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->inputList->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->inputList->setColumnWidth(0, 300);
    ui->inputList->setColumnWidth(1, 299);

    ui->inputList->setHorizontalHeaderLabels(QStringList() << "Topic" << "Type");

    ui->inputList->setSelectionBehavior(QAbstractItemView::SelectRows);

    out_rows_ = 0;

    ui->outputList->setRowCount(out_rows_);
    ui->outputList->setColumnCount(2);

    ui->outputList->setHorizontalHeaderLabels(QStringList() << "Topic" << "Rename");
    ui->outputList->setColumnWidth(0, 300);
    ui->outputList->setColumnWidth(1, 299);

    ui->outputList->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->outputList->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->outputList->setSelectionBehavior(QAbstractItemView::SelectRows);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_loadBtn_clicked()
{

    QString filePath = QFileDialog::getOpenFileName(this, "Open Rosbag File", QDir::homePath(), "Rosbag Files (*.db3)");

    if(!filePath.isEmpty()){
        input_path_ = filePath;
        load(input_path_);
    }

}

void MainWindow::load(const QString& filePath){

    topic_whitelist_.clear();
    out_rows_ = 0;
    ui->inputList->clearContents();
    ui->outputList->clearContents();

    if (!filePath.isEmpty()) {
        //qDebug() << "Selected file: " << filePath;
    }

    extractRosbagMetadata(filePath);
    getTimeInfo();

    ui->beginTime->setDateTime(startDateTime_);
    ui->endTime->setDateTime(endDateTime_);

    ui->outBeginTime->setDateTime(startDateTime_);
    ui->outEndTime->setDateTime(endDateTime_);

}

void MainWindow::extractRosbagMetadata(const QString &filePath)
{

    rosbag2_storage::StorageOptions storage_options;
    storage_options.uri = filePath.toStdString();

    rosbag2_cpp::ConverterOptions converter_options;
    converter_options.input_serialization_format = "cdr";
    converter_options.output_serialization_format = "cdr";

    std::shared_ptr<rosbag2_storage::SerializedBagMessage> msg;



    try {
        reader.open(storage_options, converter_options);
        const auto metadata = reader.get_metadata();

        msg = reader.read_next();

        //qDebug() << msg->time_stamp;
        //qDebug() << msg->topic_name;

        ////qDebug() << "Bag duration: " << metadata.duration;

        int nRow = static_cast<int>(std::size(metadata.topics_with_message_count));

        ui->inputList->setRowCount(nRow);

        // Iterate through topics
        int i = 0;

        for (const auto &topic_metadata : metadata.topics_with_message_count) {

            QString topic_name = QString::fromStdString(topic_metadata.topic_metadata.name);
            QString topic_type = QString::fromStdString(topic_metadata.topic_metadata.type);

            //QString topic_count = QString::fromStdString(std::to_string(topic_metadata.message_count));

            QTableWidgetItem *item_name = new QTableWidgetItem(topic_name);
            QTableWidgetItem *item_type = new QTableWidgetItem(topic_type);
            //QTableWidgetItem *item_count = new QTableWidgetItem(topic_count);

            ui->inputList->setItem(i, 0, item_name);
            ui->inputList->setItem(i, 1, item_type);
            //ui->inputList->setItem(i, 2, item_count);

            i++;
            ////qDebug() << "Topic: " << topic_metadata.topic_metadata.name;
            ////qDebug() << "  Type: " << topic_metadata.topic_metadata.type;
            ////qDebug() << "  Message count: " << topic_metadata.message_count;
        }

        setPaletteOk();
        statusBar()->showMessage("Loaded rosbag: " + filePath, 3000);
        setPaletteNormal();

    } catch (const std::exception &e) {
        //qDebug() << "Error opening or reading bag file: " << e.what();
    }
}

void MainWindow::setPaletteError(){

    status_bar_palette_.setColor(QPalette::WindowText, Qt::red);
    statusBar()->setPalette(status_bar_palette_);

}

void MainWindow::setPaletteNormal(){

    status_bar_palette_.setColor(QPalette::WindowText, Qt::white);
    statusBar()->setPalette(status_bar_palette_);

}

void MainWindow::setPaletteOk(){

    status_bar_palette_.setColor(QPalette::WindowText, Qt::green);
    statusBar()->setPalette(status_bar_palette_);

}


void MainWindow::on_applyButton_clicked()
{

    QList<QTableWidgetItem*> selectedItems = ui->inputList->selectedItems();

    if(selectedItems.isEmpty()) return;

    bool toprocess=true;

    for(auto it: selectedItems){

        if(toprocess){

            if(topic_whitelist_.find(it->text().toStdString()) == topic_whitelist_.end()){

                ui->outputList->setRowCount(++out_rows_);
                QTableWidgetItem *output_item = new QTableWidgetItem (*it);

                topic_whitelist_.insert(it->text().toStdString());

                ui->outputList->setItem(out_rows_-1, 0, output_item);
                output_item->setFlags(output_item->flags() & ~Qt::ItemIsEditable);

            }
        }

        toprocess=!toprocess;

    }

}


void MainWindow::on_inputList_itemSelectionChanged()
{


}
void MainWindow::on_removeButton_clicked()
{
    QList<QTableWidgetItem*> selectedItems = ui->outputList->selectedItems();

    if(selectedItems.isEmpty()) return;

    int selectedRow = selectedItems.first()->row();
    QString removed_topic = selectedItems.first()->text();

    //qDebug() << "Shifting items and deleting last row";

    auto it = topic_whitelist_.find(removed_topic.toStdString());

    if (it != topic_whitelist_.end()) {
        topic_whitelist_.erase(it);
        //qDebug() << "Deleting item from whitelist";
    }

    for (int row = selectedRow; row < out_rows_-1; row++) {

        QTableWidgetItem* currentItem = ui->outputList->item(row+1,0);

        ui->outputList->item(row,0)->setText(currentItem->text());
    }


    ui->outputList->removeRow(out_rows_);
    ui->outputList->setRowCount(--out_rows_); //TODO: delete multiple elements

    statusBar()->showMessage("Topic removed from output file: " + removed_topic, 3000);
}

void MainWindow::on_saveBtn_clicked()
{

    ui->saveBtn->setEnabled(false);

    cooldown_timer_.setSingleShot(true);
    connect(&cooldown_timer_, &QTimer::timeout, this, &MainWindow::enableSaveButton);
    cooldown_timer_.start(1000); // 1s

    try{

        QString filePath = input_path_;

        QString outName = QString("rosbag2_edit_") + QDateTime::currentDateTime().toString("yy_MM_dd-hh_mm_ss");


        int append = 1;
        QString baseName = outName;

        try {
            while (QFileInfo::exists(baseName)) {
                baseName = outName + QString("_%1").arg(append++);
            }
        } catch (...) {
            //qDebug() << "Error occurred on output file name.";
            return;
        }

        QFileInfo inputFileInfo(input_path_);
        QString outputPath = inputFileInfo.path();
        QString fullFilePath = outputPath + "/" + baseName;

        //qDebug() << "Output file path: " << fullFilePath;


        reader.reset_filter();

        rosbag2_storage::StorageOptions storage_options;
        storage_options.uri = fullFilePath.toStdString();
        storage_options.storage_id = "sqlite3";

        auto storage_factory = std::make_shared<rosbag2_storage::StorageFactory>();
        auto storage = storage_factory->open_read_write(storage_options);

        const auto metadata = reader.get_metadata();

        for(const auto &topic_metadata : metadata.topics_with_message_count){

            if(topic_whitelist_.find(topic_metadata.topic_metadata.name)!= topic_whitelist_.end()){

                auto it = topic_rename_.find(topic_metadata.topic_metadata.name);
                if(it!=topic_rename_.end()){
                    rosbag2_storage::TopicMetadata modified_topic_metadata = topic_metadata.topic_metadata;
                    modified_topic_metadata.name = it->second;
                    storage->create_topic(modified_topic_metadata);
                    //qDebug() << "Creating: "<<modified_topic_metadata.name;
                }else{
                    storage->create_topic(topic_metadata.topic_metadata);
                    //qDebug() << "Creating: "<<topic_metadata.topic_metadata.name;
                }
            }
        }

        reader.reset_filter();

        while (reader.has_next()) {

            auto bag_message = reader.read_next();
            auto message_timestamp = bag_message->time_stamp;

            double message_timestamp_seconds = static_cast<double>(message_timestamp) * 1e-9;
            QDateTime message_datetime = QDateTime::fromSecsSinceEpoch(message_timestamp_seconds, Qt::LocalTime);

            auto nanoseconds = std::chrono::nanoseconds(message_timestamp); //TODO: uniform code for ms extraction
            auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(nanoseconds % std::chrono::seconds(1));
            message_datetime = message_datetime.addMSecs(milliseconds.count());


            if (message_datetime >= trimStart_ && message_datetime <= trimEnd_) {

                if (topic_whitelist_.find(bag_message->topic_name) != topic_whitelist_.end()) {

                    auto it = topic_rename_.find(bag_message->topic_name);
                    if (it != topic_rename_.end()){bag_message->topic_name.assign(topic_rename_[bag_message->topic_name]);}

                    storage->write(bag_message);

                }

            }else{

                ////qDebug() << "Trimming";

            }

        }

        statusBar()->showMessage("Finished writing output rosbag.", 3000);

    }
    catch (...) {
        //qDebug() << "Error during write.";
        return;
    }


}

void MainWindow::enableSaveButton()
{
    ui->saveBtn->setEnabled(true);
}


void MainWindow::on_actionOpen_Directory_triggered()
{
    QString directoryPath = QFileDialog::getExistingDirectory(this, "Open Directory", QDir::homePath());
    populateTreeWidget(directoryPath);

}

void MainWindow::populateTreeWidget(const QString &path) {
    QDir dir(path);
    QTreeWidgetItem *parentItem = new QTreeWidgetItem(ui->treeWidget);
    parentItem->setText(0, dir.dirName());
    parentItem->setData(0, Qt::UserRole, dir.absolutePath());


    // Filter only .db3 files
    QStringList entries = dir.entryList(QStringList() << "*.db3", QDir::Files | QDir::NoDotAndDotDot);

    connect(ui->treeWidget, &QTreeWidget::itemDoubleClicked, this, &MainWindow::onTreeItemDoubleClicked);

    for (const QString &entry : entries) {
        QTreeWidgetItem *item = new QTreeWidgetItem(parentItem);
        item->setText(0, entry);

        QFileInfo fileInfo(dir.absoluteFilePath(entry));
        item->setText(1, QString::number(fileInfo.size()));
        item->setText(2, fileInfo.suffix().toUpper());

    }

    ui->treeWidget->expandItem(parentItem);

}

void MainWindow::onTreeItemDoubleClicked(QTreeWidgetItem *item, int column) {

    if(item){

        if (item->parent() == nullptr) {
            input_path_ = item->data(0, Qt::UserRole).toString();
        } else {
            QTreeWidgetItem *parentItem = item->parent();
            input_path_ = parentItem->data(0, Qt::UserRole).toString() + QDir::separator() + item->text(0);

        }

        QFileInfo fileInfo(input_path_);
        if (fileInfo.suffix().toLower() == "db3") {
            load(input_path_);
        } else {
            ////qDebug() << "Selected file does not have a .db3 extension.";
        }

    }

}


void MainWindow::on_actionOpen_Rosbag_triggered()
{
    on_loadBtn_clicked();
}


void MainWindow::on_outputList_itemChanged(QTableWidgetItem *item)
{
    auto item_row = ui->outputList->row(item);
    auto item_col = ui->outputList->column(item);

    if (item_col == 1){

        topic_rename_[ui->outputList->item(item_row,0)->text().toStdString()]=item->text().toStdString();
        ////qDebug() << "original name: " <<ui->outputList->item(item_row,0)->text();
        ////qDebug() <<"mapped name: " <<item->text();

    }
}


void MainWindow::on_actionrosbag2csv_triggered()
{
    //qDebug() << "csv output";

}

std::string getFormattedTime(std::time_t time, std::chrono::milliseconds milliseconds) {
    // std::tm timeinfo = *std::localtime(&time);
    // std::ostringstream oss;
    // oss << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S");
    // oss << "." << std::setfill('0') << std::setw(3) << milliseconds.count(); // Include milliseconds
    // return oss.str();
    qDebug() <<"test";
    return "test";
}

void MainWindow::getTimeInfo() {
    const auto metadata = reader.get_metadata();

    auto starting_time = std::chrono::system_clock::to_time_t(metadata.starting_time);
    auto starting_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(metadata.starting_time.time_since_epoch()) % 1000;
    ////qDebug() << "Start Time:" << QString::fromStdString(getFormattedTime(starting_time, starting_milliseconds));

    auto end_time = std::chrono::system_clock::to_time_t(metadata.starting_time + metadata.duration);
    auto ending_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>((metadata.starting_time + metadata.duration).time_since_epoch()) % 1000;
    ////qDebug() << "End Time:" << QString::fromStdString(getFormattedTime(end_time, ending_milliseconds));

    auto starting_time_point = metadata.starting_time;
    auto end_time_point = metadata.starting_time + metadata.duration;

    startDateTime_ = QDateTime::fromSecsSinceEpoch(std::chrono::system_clock::to_time_t(starting_time_point));
    startDateTime_ = startDateTime_.addMSecs(starting_milliseconds.count());

    endDateTime_ = QDateTime::fromSecsSinceEpoch(std::chrono::system_clock::to_time_t(end_time_point));
    endDateTime_ = endDateTime_.addMSecs(ending_milliseconds.count());

    trimStart_ = startDateTime_;
    trimEnd_ = endDateTime_;

}


void MainWindow::on_actionSave_triggered()
{
    try{
        on_saveBtn_clicked();
    }
    catch(...){
        //qDebug() <<"Error on save";
        return;
    }
}


void MainWindow::on_outBeginTime_dateTimeChanged(const QDateTime &dateTime)
{
    trimStart_ = dateTime;
    ////qDebug() << "trimStart changed:"<<dateTime;

}


void MainWindow::on_outEndTime_dateTimeChanged(const QDateTime &dateTime)
{
    trimEnd_ = dateTime;
    ////qDebug() << "trimEnd_ changed:"<<dateTime;
}


void MainWindow::on_actionContacts_triggered()
{
    NewProjectDialog* test = new NewProjectDialog(this);

    int result = test->exec();
}
