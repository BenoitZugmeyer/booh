#ifndef FRAMECAPTURE_H
#define FRAMECAPTURE_H

#include <QtWebKitWidgets>

class FrameCapture : public QObject
{
    Q_OBJECT

public:
    FrameCapture();
    void load(const QUrl &url, const QString &outputFileName);

signals:
    void finished();

private slots:
    void printProgress(int percent);
    void saveResult(bool ok);

private:
    QWebPage m_page;
    QString m_fileName;
    int m_percent;

    void saveFrame(QWebFrame *frame);
};

#endif
