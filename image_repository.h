#pragma once

#include <QString>
#include <QStringList>

class ImageRepository {
public:
    QStringList scanDirectory(const QString& rootPath);
    bool saveCache(const QStringList& paths) const;
    QStringList loadCache() const;

    QString cacheFilePath() const;

private:
    bool isImageFile(const QString& filePath) const;
};