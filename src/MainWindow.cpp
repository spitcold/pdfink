#include "MainWindow.h"
#include <QToolBar>
#include <QAction>
#include <QFileDialog>
#include <QPdfDocument>
#include <QHBoxLayout>
#include <QImage>
#include <QPainter>
#include <QResizeEvent>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), pdfDoc(new QPdfDocument(this)), currentPage(0)
{
    central = new QWidget(this);
    setCentralWidget(central);

    pdfLabel = new QLabel;
    pdfLabel->setBackgroundRole(QPalette::Base);
    pdfLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pdfLabel->setAlignment(Qt::AlignCenter);

    overlay = new OverlayWidget(pdfLabel);

    QHBoxLayout* layout = new QHBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(pdfLabel);
    central->setLayout(layout);

    overlay->setParent(pdfLabel);
    overlay->setGeometry(pdfLabel->rect());
    overlay->show();

    QToolBar* tb = addToolBar("Main");
    QAction* openAct = tb->addAction("Open PDF");
    connect(openAct, &QAction::triggered, this, &MainWindow::openPdf);

    QAction* freeAct = tb->addAction("Freehand");
    connect(freeAct, &QAction::triggered, [this](){ overlay->setTool(OverlayWidget::Tool::Freehand); });
    QAction* lineAct = tb->addAction("Line");
    connect(lineAct, &QAction::triggered, [this](){ overlay->setTool(OverlayWidget::Tool::Line); });
    QAction* rectAct = tb->addAction("Rect");
    connect(rectAct, &QAction::triggered, [this](){ overlay->setTool(OverlayWidget::Tool::Rect); });
    QAction* ellipseAct = tb->addAction("Ellipse");
    connect(ellipseAct, &QAction::triggered, [this](){ overlay->setTool(OverlayWidget::Tool::Ellipse); });
    QAction* triAct = tb->addAction("Triangle");
    connect(triAct, &QAction::triggered, [this](){ overlay->setTool(OverlayWidget::Tool::Triangle); });
}

void MainWindow::resizeEvent(QResizeEvent* ev)
{
    QMainWindow::resizeEvent(ev);
    if (pdfLabel && overlay) {
        overlay->setGeometry(pdfLabel->rect());
    }
}

void MainWindow::openPdf()
{
    QString file = QFileDialog::getOpenFileName(this, "Open PDF", QString(), "PDF files (*.pdf)");
    if (file.isEmpty()) return;
    pdfDoc->load(file);
    currentPage = 0;
    renderPage(currentPage);
}

void MainWindow::renderPage(int page)
{
    if (!pdfDoc || pdfDoc->status() != QPdfDocument::Status::Ready) return;
    if (page < 0 || page >= pdfDoc->pageCount()) return;

    QSize targetSize = pdfLabel->size();
    if (targetSize.width() <= 0 || targetSize.height() <= 0) {
        targetSize = QSize(800, 600);
    }

    QImage image = pdfDoc->render(page, targetSize);

    if (image.isNull()) {
        pdfLabel->setText("Failed to render PDF page. Ensure Qt Pdf module is available.");
    } else {
        pdfLabel->setPixmap(QPixmap::fromImage(image));
        overlay->setBackgroundImage(image);
        overlay->setGeometry(pdfLabel->rect());
    }
}