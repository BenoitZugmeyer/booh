#include "framecapture.h"

#include <iostream>

bool CapturePage::extension(
        Extension extension,
        const ExtensionOption* option,
        ExtensionReturn *output)
{
    std::cerr << "EXTENSION" << std::endl;
    if (extension != QWebPage::ErrorPageExtension)
        return false;

    ErrorPageExtensionOption *errorOption = (ErrorPageExtensionOption*) option;
    std::cerr << "Error loading " << qPrintable(errorOption->url.toString())  << std::endl;
    if(errorOption->domain == QWebPage::QtNetwork)
        std::cerr << "Network error (" << errorOption->error << "): ";
    else if(errorOption->domain == QWebPage::Http)
        std::cerr << "HTTP error (" << errorOption->error << "): ";
    else if(errorOption->domain == QWebPage::WebKit)
        std::cerr << "WebKit error (" << errorOption->error << "): ";

    std::cerr << qPrintable(errorOption->errorString) << std::endl;

    return false;
}

bool CapturePage::supportsExtension(Extension extension) const
{
    return QWebPage::ErrorPageExtension == extension;
}


FrameCapture::FrameCapture(): QObject(), m_percent(0)
{
    connect(&m_page, SIGNAL(loadProgress(int)), this, SLOT(printProgress(int)));
    connect(&m_page, SIGNAL(loadFinished(bool)), this, SLOT(saveResult(bool)));
}

void FrameCapture::load(const QUrl &url, const QString &outputFileName)
{
    std::cout << "Loading " << qPrintable(url.toString()) << std::endl;
    m_percent = 0;
    int index = outputFileName.lastIndexOf('.');
    m_fileName = (index == -1) ? outputFileName + ".png" : outputFileName;
    m_page.mainFrame()->load(url);
    m_page.mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    m_page.mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    m_page.setViewportSize(QSize(1024, 768));
}

void FrameCapture::printProgress(int percent)
{
    if (m_percent >= percent)
        return;

    while (m_percent++ < percent)
        std::cout << "#" << std::flush;

    if (percent == 100)
        std::cout << std::endl;
}

void FrameCapture::saveResult(bool ok)
{
    if (ok) {
        QSize size = m_page.mainFrame()->contentsSize();
        ok = size.width() != 0 && size.height() != 0;
    }

    std::cout << "saveResult" << std::endl;
    // crude error-checking
    if (!ok) {
        std::cerr << "Failed loading " << qPrintable(m_page.mainFrame()->url().toString()) << std::endl;
        emit finished();
        return;
    }

    // save each frame in different image files
    saveFrame(m_page.mainFrame());

    emit finished();
}

void FrameCapture::saveFrame(QWebFrame *frame)
{
    static int frameCounter = 0;

    std::cout << "saveFrame" << std::endl;
    QString fileName(m_fileName);
    if (frameCounter) {
        int index = m_fileName.lastIndexOf('.');
        fileName = fileName.insert(index, "_frame" + QString::number(frameCounter));
    }

    QImage image(frame->contentsSize(), QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    frame->documentElement().render(&painter);
    painter.end();

    image.save(fileName);

    ++frameCounter;
    foreach(QWebFrame *childFrame, frame->childFrames())
        saveFrame(childFrame);
}
