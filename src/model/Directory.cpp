//
// Created by mistlight on 1/1/2017.
//

#include <QDebug>
#include <QtSql/QSqlRecord>
#include <ctime>
#include <sstream>
#include <QtWidgets/QMessageBox>
#include <QtSql/QSqlError>
#include <QtSql/QSqlField>
#include "Directory.h"

Directory::Directory(QObject *parent) : QSqlTableModel(parent) {
    this->setTable("directories");
    this->setEditStrategy(EditStrategy::OnManualSubmit);
}


void Directory::addDirectory(QString path) {
    auto record = this->getEmptyRecord();

    // create the time string
    std::time_t now = std::time(0);
    std::string timeString;
    std::stringstream s(timeString);
    s << now;

    record.setValue("full_path", path);
    record.setValue("created_at", QString::fromStdString(timeString));

    this->insertRecord(-1, record);
    auto res = this->submitAll();

    if(!res) {
        auto errorObj = this->lastError();
        QMessageBox *messageBox = new QMessageBox();
        messageBox->critical(0, "Error", "Failed to write to config file: " + path + ", error messsage is: " + errorObj.driverText());
    }

}

void Directory::removeDirectory(QString path) {

}

QSqlRecord Directory::getEmptyRecord() {
    QSqlRecord record;

    QSqlField pathField;
    pathField.setName("full_path");
    pathField.setType(QVariant::String);
    record.append(pathField);

    QSqlField createdAtField;
    createdAtField.setName("created_at");
    createdAtField.setType(QVariant::DateTime);
    record.append(createdAtField);

    QSqlField lastScannedField;
    lastScannedField.setName("last_scanned");
    lastScannedField.setType(QVariant::DateTime);
    record.append(lastScannedField);

    return record;
}
