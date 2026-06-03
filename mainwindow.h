#pragma once

#include <QMainWindow>
#include <QStringList>

#include "smart_ptr.h"

class QListWidget;
class QListWidgetItem;
class QLineEdit;
class QLabel;
class QPushButton;
class ImageRepository;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void chooseFolder();
    void scanFolder();
    void loadFromCache();
    void showSelectedImage(QListWidgetItem* item);

private:
    void buildUi();
    void populateList(const QStringList& paths);
    void showImage(const QString& path);

    SmartPtr<ImageRepository> repo_;
    QStringList paths_;
    QString currentImagePath_;

    QLineEdit* folderEdit_;
    QListWidget* listWidget_;
    QLabel* previewLabel_;
    QLabel* infoLabel_;
};