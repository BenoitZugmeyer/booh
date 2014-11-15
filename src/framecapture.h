#ifndef FRAMECAPTURE_H
#define FRAMECAPTURE_H

#include <QtWebKitWidgets>

class CapturePage : public QWebPage
{
    Q_OBJECT

public:
    CapturePage(QObject* parent = 0) : QWebPage(parent) {}

    bool extension(
            Extension extension,
            const ExtensionOption* option = 0,
            ExtensionReturn *output = 0);
    bool supportsExtension(Extension extension) const;
};

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
    CapturePage m_page;
    QString m_fileName;
    int m_percent;

    void saveFrame(QWebFrame *frame);
};

#endif
