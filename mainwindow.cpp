#include "mainwindow.h"
#include "image_repository.h"

#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSplitter>
#include <QStatusBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QPixmap>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , repo_(new ImageRepository)
    , folderEdit_(nullptr)
    , listWidget_(nullptr)
    , previewLabel_(nullptr)
    , infoLabel_(nullptr)
{
    buildUi();
    loadFromCache();
}

void MainWindow::buildUi() {
    setWindowTitle("Просмотр изображений");
    resize(1100, 700);

    auto* central = new QWidget(this);
    auto* rootLayout = new QVBoxLayout(central);

    auto* topLayout = new QHBoxLayout();

    folderEdit_ = new QLineEdit(this);
    folderEdit_->setPlaceholderText("Выберите папку для поиска изображений...");

    auto* browseButton = new QPushButton("Открыть папку", this);
    auto* scanButton = new QPushButton("Сканировать", this);
    auto* cacheButton = new QPushButton("Загрузить кэш", this);

    topLayout->addWidget(folderEdit_, 1);
    topLayout->addWidget(browseButton);
    topLayout->addWidget(scanButton);
    topLayout->addWidget(cacheButton);

    auto* splitter = new QSplitter(Qt::Horizontal, this);

    listWidget_ = new QListWidget(this);
    listWidget_->setViewMode(QListView::IconMode);
    listWidget_->setIconSize(QSize(96, 96));
    listWidget_->setResizeMode(QListWidget::Adjust);
    listWidget_->setMovement(QListView::Static);
    listWidget_->setSpacing(8);

    auto* previewContainer = new QWidget(this);
    auto* previewLayout = new QVBoxLayout(previewContainer);

    previewLabel_ = new QLabel("Выберите изображение", this);
    previewLabel_->setAlignment(Qt::AlignCenter);
    previewLabel_->setMinimumSize(400, 400);
    previewLabel_->setStyleSheet("QLabel { background: #202020; color: white; border: 1px solid #555; }");

    infoLabel_ = new QLabel(this);
    infoLabel_->setWordWrap(true);

    previewLayout->addWidget(previewLabel_, 1);
    previewLayout->addWidget(infoLabel_);

    splitter->addWidget(listWidget_);
    splitter->addWidget(previewContainer);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);

    rootLayout->addLayout(topLayout);
    rootLayout->addWidget(splitter, 1);

    setCentralWidget(central);
    statusBar()->showMessage("Готово");

    connect(browseButton, &QPushButton::clicked, this, &MainWindow::chooseFolder);
    connect(scanButton, &QPushButton::clicked, this, &MainWindow::scanFolder);
    connect(cacheButton, &QPushButton::clicked, this, &MainWindow::loadFromCache);
    connect(listWidget_, &QListWidget::itemClicked, this, &MainWindow::showSelectedImage);
}

void MainWindow::chooseFolder() {
    const QString dir = QFileDialog::getExistingDirectory(this, "Выберите папку");
    if (!dir.isEmpty()) {
        folderEdit_->setText(dir);
    }
}

void MainWindow::scanFolder() {
    const QString dir = folderEdit_->text().trimmed();
    if (dir.isEmpty() || !QDir(dir).exists()) {
        QMessageBox::warning(this, "Ошибка", "Укажите существующую папку.");
        return;
    }

    statusBar()->showMessage("Идёт поиск изображений...");
    paths_ = repo_->scanDirectory(dir);
    repo_->saveCache(paths_);
    populateList(paths_);

    statusBar()->showMessage(QString("Найдено файлов: %1").arg(paths_.size()));
}

void MainWindow::loadFromCache() {
    paths_ = repo_->loadCache();
    populateList(paths_);

    statusBar()->showMessage(QString("Загружено из кэша: %1").arg(paths_.size()));
}

void MainWindow::populateList(const QStringList& paths) {
    listWidget_->clear();

    for (const QString& path : paths) {
        const QFileInfo fi(path);

        auto* item = new QListWidgetItem(QIcon(path), fi.fileName());
        item->setData(Qt::UserRole, path);
        item->setToolTip(path);

        listWidget_->addItem(item);
    }

    if (!paths.isEmpty()) {
        showImage(paths.first());
    } else {
        previewLabel_->setText("Изображения не найдены");
        infoLabel_->clear();
    }
}

void MainWindow::showSelectedImage(QListWidgetItem* item) {
    if (!item) return;
    showImage(item->data(Qt::UserRole).toString());
}

void MainWindow::showImage(const QString& path) {
    currentImagePath_ = path;

    QPixmap pix(path);
    if (pix.isNull()) {
        previewLabel_->setText("Не удалось открыть изображение");
        infoLabel_->setText(path);
        return;
    }

    const QSize target = previewLabel_->size().expandedTo(QSize(400, 400));
    previewLabel_->setPixmap(pix.scaled(target, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    const QFileInfo fi(path);
    infoLabel_->setText(
        QString("Файл: %1\nРазмер: %2 байт\nПуть: %3")
            .arg(fi.fileName())
            .arg(fi.size())
            .arg(fi.absoluteFilePath())
        );
}