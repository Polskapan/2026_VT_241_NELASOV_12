#include "image_repository.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QStandardPaths>
#include <QSet>

QString ImageRepository::cacheFilePath() const {
    QString baseDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (baseDir.isEmpty()) {
        baseDir = QDir::currentPath() + "/appdata";
    }

    QDir().mkpath(baseDir);
    return baseDir + "/image_links.json";
}

bool ImageRepository::isImageFile(const QString& filePath) const {
    const QString ext = QFileInfo(filePath).suffix().toLower();
    return ext == "jpg" || ext == "jpeg" || ext == "png" || ext == "bmp";
}

QStringList ImageRepository::scanDirectory(const QString& rootPath) {
    QStringList result;
    QSet<QString> unique;

    QDirIterator it(rootPath, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        const QString path = it.next();
        if (isImageFile(path)) {
            const QString absPath = QFileInfo(path).absoluteFilePath();
            if (!unique.contains(absPath)) {
                unique.insert(absPath);
                result << absPath;
            }
        }
    }

    return result;
}

bool ImageRepository::saveCache(const QStringList& paths) const {
    QFile file(cacheFilePath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }

    QJsonArray arr;
    for (const QString& path : paths) {
        arr.append(path);
    }

    QJsonDocument doc(arr);
    file.write(doc.toJson(QJsonDocument::Indented));
    return true;
}

QStringList ImageRepository::loadCache() const {
    QFile file(cacheFilePath());
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        return {};
    }

    const QByteArray data = file.readAll();
    QJsonParseError err{};
    const QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError || !doc.isArray()) {
        return {};
    }

    QStringList result;
    const QJsonArray arr = doc.array();
    for (const auto& value : arr) {
        const QString path = value.toString();
        if (!path.isEmpty() && QFileInfo::exists(path)) {
            result << path;
        }
    }

    return result;
}