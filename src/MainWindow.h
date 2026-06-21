#pragma once
#include <QMainWindow>
#include <QPdfDocument>
#include <QLabel>
#include "OverlayWidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
protected:
    void resizeEvent(QResizeEvent* ev) override;
private slots:
    void openPdf();
    void onPageChanged(int page);
private:
    QWidget* central;
    QLabel* pdfLabel;
    QPdfDocument* pdfDoc;
    OverlayWidget* overlay;
    int currentPage;
    void renderPage(int page);
};