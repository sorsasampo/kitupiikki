/*
   Copyright (C) 2018 Arto Hyvättinen

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "pdfscene.h"

#include <poppler/qt5/poppler-qt5.h>
#include <QGraphicsPixmapItem>
#include <QPrinter>
#include <QPainter>

PdfScene::PdfScene(QObject *parent)
    : NaytinScene (parent)
{

}

PdfScene::PdfScene(const QByteArray &pdfdata, QObject *parent) :
    PdfScene( parent)
{
    naytaPdf(pdfdata);
}

QString PdfScene::otsikko() const
{
    return otsikko_;
}

bool PdfScene::naytaPdf(const QByteArray &pdfdata)
{
    if( pdfdata.startsWith("%PDF"))
    {
        data_ = pdfdata;
        return true;
    }
    return false;

}

void PdfScene::piirraLeveyteen(double leveyteen)
{
    setBackgroundBrush(QBrush(Qt::gray));
    clear();

    Poppler::Document *pdfDoc = Poppler::Document::loadFromData( data_ );
    pdfDoc->setRenderHint(Poppler::Document::TextAntialiasing);
    pdfDoc->setRenderHint(Poppler::Document::Antialiasing);

    otsikko_ = pdfDoc->info("Title") ;

    double ypos = 0.0;
    double leveys = 0.0;

    // Monisivuisen pdf:n sivut pinotaan päällekkäin
    for( int sivu = 0; sivu < pdfDoc->numPages(); sivu++)
    {
        Poppler::Page *pdfSivu = pdfDoc->page(sivu);

        if( !pdfSivu )
            continue;

        double pdfleveys = pdfSivu->pageSizeF().width();
        double skaala = leveyteen / pdfleveys * 72.0;

        QImage image = pdfSivu->renderToImage(skaala, skaala);
        QPixmap kuva = QPixmap::fromImage( image, Qt::DiffuseAlphaDither);

        addRect(2, ypos+2, kuva.width(), kuva.height(), QPen(Qt::NoPen), QBrush(Qt::black) );

        QGraphicsPixmapItem *item = addPixmap(kuva);
        item->setY( ypos );
        addRect(0, ypos, kuva.width(), kuva.height(), QPen(Qt::black), Qt::NoBrush );

        if( kuva.width() > leveys)
            leveys = kuva.width();

        ypos += kuva.height() + 10.0;


        delete pdfSivu;
    }

    setSceneRect(-5.0, -5.0, leveys + 10.0, ypos + 5.0  );

    delete pdfDoc;
}

void PdfScene::tulosta(QPrinter *printer)
{
    QPainter painter(printer);

    Poppler::Document *document = Poppler::Document::loadFromData(data_);
    document->setRenderBackend(Poppler::Document::ArthurBackend);

    int pageCount = document->numPages();
    for(int i=0; i < pageCount; i++)               
    {
        Poppler::Page *page = document->page(i);

        double vaakaResoluutio = printer->pageRect(QPrinter::Point).width() / page->pageSizeF().width() * printer->resolution();
        double pystyResoluutio = printer->pageRect(QPrinter::Point).height() / page->pageSizeF().height() * printer->resolution();

        double resoluutio = vaakaResoluutio < pystyResoluutio ? vaakaResoluutio : pystyResoluutio;

        page->renderToPainter( &painter, resoluutio, resoluutio,
                                            0,0,page->pageSize().width(), page->pageSize().height());
        if( i < pageCount - 1)
            printer->newPage();

        delete page;
    }
    painter.end();
    delete document;
}


