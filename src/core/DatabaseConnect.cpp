//
// Created by mistlight on 1/1/2017.
//

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QSqlError>
#include <QtWidgets/QMessageBox>
#include "DatabaseConnect.h"
#include "Util.h"
#include <QtCore/QDir>
#include <QDebug>

bool ::Core::openDb() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    // ensure the path gets created for settings, if not already exists
    createSettingPathIfNotExists();

    auto dbFilePath = QDir(getSettingPath() + "/nodoka.db").absolutePath();
    db.setDatabaseName(dbFilePath);

    qDebug() << "db file set to: " << dbFilePath;

    if(!db.open()) {
        return false;
    }

    QSqlQuery query;
    bool pragmaResponse = false;
    pragmaResponse = query.exec("PRAGMA journal_mode=WAL;");

    if(!pragmaResponse) {
        return false;
    }


    bool response = false;
    response = query.exec("CREATE TABLE IF NOT EXISTS metadata ("
                       "key text PRIMARY KEY,"
                       "value text"
                       ")");

    if(!response) {
        return false;
    }

    response = query.exec("CREATE TABLE IF NOT EXISTS directories("
                       "full_path text PRIMARY KEY,"
                       "created_at text,"
                       "last_scanned text"
                       ")");

    if(!response) {
        return false;
    }

    response = query.exec("CREATE TABLE IF NOT EXISTS audiobooks("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "directory TEXT,"
                       "name TEXT,"
                       "full_path TEXT,"
                       "completeness INTEGER,"
                       "default_order INTEGER,"
                       "selected_file TEXT,"
                       "created_at TEXT"
                       ")");

    if(!response) {
        QMessageBox::critical(0, "Warning", "Failed to create audiobook config: " + query.lastError().databaseText());
        return false;
    }

    response = query.exec("CREATE TABLE IF NOT EXISTS audiobook_file("
                       "audiobook_id INTEGER,"
                       "name TEXT,"
                       "full_path TEXT PRIMARY KEY,"
                       "length_of_file TEXT,"
                       "seek_position TEXT,"
                       "position INTEGER,"
                       "completeness INTEGER,"
                       "file_exists BOOL,"
                       "created_at TEXT"
                       ")");

    if(!response) {
        QMessageBox::critical(0, "Warning", "Failed to create audiobook file config");
        return false;
    }

    query.exec("CREATE INDEX IF NOT EXISTS audiobook_dir_index ON audiobooks(directory)");
    query.exec("CREATE INDEX IF NOT EXISTS audiobook_full_path_index ON audiobooks(full_path)");
    query.exec("CREATE INDEX IF NOT EXISTS audiobook_ab_id_index ON audiobook_file(audiobook_id)");
    query.exec("CREATE INDEX IF NOT EXISTS audiobook_file_dir_index ON audiobook_file(full_path)");

    return true;
}
