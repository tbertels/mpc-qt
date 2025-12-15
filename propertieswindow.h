#ifndef PROPERTIESWINDOW_H
#define PROPERTIESWINDOW_H

#include <QDateTime>
#include <QDialog>
#include <QVariantList>
#include <QVariantMap>
#include <QShowEvent>

namespace Ui {
class PropertiesWindow;
}

class PropertiesWindow : public QDialog
{
    Q_OBJECT

public:
    explicit PropertiesWindow(QWidget *parent = nullptr);
    ~PropertiesWindow();

signals:
    void artistAndTitleChanged(QString artistAndTitle, QString filename);

public slots:
    void setFileName(const QString &filename);
    void setFileFormat(const QString &format);
    void setFileSize(const int64_t &bytes);
    void setMediaLength(double time);
    void setVideoSize(const QSize &sz);
    void setFileModifiedTime(const QUrl &file);
    void setTracks(const QVariantList &tracks);
    void setMediaTitle(const QString &title);
    void setFilePath(const QString &path);
    void setMetaData(QVariantMap data);
    void setChapters(const QVariantList &chapters);

private slots:
    void on_save_clicked();
    void updateSaveVisibility();

protected:
    void showEvent(QShowEvent *event) override;

private:
    void updateLastTab();
    QString generalDataText();
    QString sectionText(const QString &header, const QVariantMap &fields);
    void ensureUiInitialized();
    void updateIconAsync();

    Ui::PropertiesWindow *ui;
    bool uiInitialized = false;

    QString filename;
    QString pendingFilename;
    QVariantMap generalData;
    QString trackText;
    QString chapterText;
};

#endif // PROPERTIESWINDOW_H
