/*
 * Copyright (C) 2016 ~ 2018 Wuhan Deepin Technology Co., Ltd.
 *
 * Author:     Iceyer <me@iceyer.net>
 *
 * Maintainer: Iceyer <me@iceyer.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "infodialog.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFileInfo>
#include <QTimer>
#include <QScrollArea>

#include <DApplication>
#include "dplatformwindowhandle.h"
#include "dblureffectwidget.h"
#include <DImageButton>
#include <DFrame>
#include <DFloatingWidget>
#include <DHiDPIHelper>
#include <DWindowCloseButton>
#include <DFontSizeManager>
#include <DArrowLineDrawer>

#include <dwindowclosebutton.h>

#include "../../core/metasearchservice.h"
#include "../helper/widgethellper.h"
#include "cover.h"
#include "musicimagebutton.h"

DWIDGET_USE_NAMESPACE

static const int CoverSize = 142;

class InfoDialogPrivate
{
public:
    InfoDialogPrivate(InfoDialog *parent) : q_ptr(parent) {}

    void initUI();
    void initConnection();
    void updateLabelSize();

//    DBlurEffectWidget   *bgBlurWidget   = nullptr;
    DFrame              *infoGridFrame  = nullptr;
    Cover               *cover          = nullptr;
    DLabel              *title          = nullptr;
    DWindowCloseButton  *closeBt        = nullptr;
    DLabel              *basicinfo      = nullptr;
    QList<DLabel *>     valueList;
    QList<DLabel *>     keyList;
    int                 frameHeight     = 0;
    InfoDialog *q_ptr;
    MetaPtr             meta            = nullptr;
    bool                DoubleElements  = false;
    bool                isExPand          = true;
    Q_DECLARE_PUBLIC(InfoDialog)
};

void InfoDialogPrivate::initUI()
{
    Q_Q(InfoDialog);

    meta = MetaPtr(new MediaMeta);
    q->setObjectName("InfoDialog");
    q->setFixedSize(320, 500);
    q->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    auto layout = new QVBoxLayout(q);
    layout->setSpacing(0);
    layout->setContentsMargins(10, 50, 10, 10);

    closeBt = new DWindowCloseButton( q);
    closeBt->setFocusPolicy(Qt::NoFocus);
    closeBt->setFixedSize(50, 50);
    closeBt->setIconSize(QSize(50, 50));
    closeBt->move(q->width() - 50, 0);

    cover = new Cover(q);
    cover->setContentsMargins(0, 0, 0, 0);
    cover->setObjectName("InfoCover");
    cover->setFixedSize(CoverSize, CoverSize);
    cover->move(89, 60);
    title = new DLabel(q);
    title->setObjectName("InfoTitle");
    title->setFixedWidth(290);
    title->setAlignment(Qt::AlignCenter);
    title->setWordWrap(true);
    title->setForegroundRole(DPalette::BrightText);
    title->move(10, 212);

    infoGridFrame = new DFrame(q);
    infoGridFrame->setFocusPolicy(Qt::NoFocus);
    infoGridFrame->setLineWidth(0);
    infoGridFrame->setFrameRounded(true);
    infoGridFrame->setMaximumWidth(300);
    infoGridFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    DPalette pl = infoGridFrame->palette();
    QColor windowColor("#FFFFFF");
    windowColor.setAlphaF(0.7);
    pl.setColor(DPalette::Window, windowColor);
    QColor sbcolor("#000000");
    sbcolor.setAlphaF(0.05);
    pl.setColor(DPalette::Shadow, sbcolor);
    infoGridFrame->setPalette(pl);

    layout->addSpacing(10);
    layout->addWidget(cover, 0, Qt::AlignCenter);
    layout->addSpacing(10);
    layout->addWidget(title, 0, Qt::AlignCenter);
    layout->addSpacing(10);
    layout->addWidget(infoGridFrame);

    basicinfo = new DLabel("   " + InfoDialog::tr("Basic info"),q);
    basicinfo->setForegroundRole(DPalette::TextTitle);
    basicinfo->setFixedWidth(300);

    auto infoLayout = new QVBoxLayout(infoGridFrame);
    infoLayout->setSpacing(0);
    infoLayout->setMargin(0);

    auto infogridLayout = new QGridLayout(infoGridFrame);
    infogridLayout->setMargin(10);
    infogridLayout->setHorizontalSpacing(10);
    infogridLayout->setVerticalSpacing(5);
    infogridLayout->setColumnStretch(0, 60);
    infogridLayout->setColumnStretch(1, 260);

    QStringList infoKeys;
    infoKeys << InfoDialog::tr("Title:") << InfoDialog::tr("Artist:")
             << InfoDialog::tr("Album:") << InfoDialog::tr("Type:")
             << InfoDialog::tr("Size:") << InfoDialog::tr("Duration:")
             << InfoDialog::tr("Path:");

    for (int i = 0; i < infoKeys.length(); ++i) {
        auto infoKey = new DLabel(infoKeys.value(i));
//        auto infoFont = infoKey->font();
//        infoFont.setPointSize(8);
//        infoKey->setFont(infoFont);
        infoKey->setObjectName("InfoKey");
//        infoKey->setMinimumHeight(18);
        infoKey->setForegroundRole(DPalette::WindowText);
        infoKey->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        DFontSizeManager::instance()->bind(infoKey, DFontSizeManager::T8);
        keyList << infoKey;

        auto infoValue = new DLabel(q);
//        infoValue->setFont(infoFont);
        infoValue->setWordWrap(true);
        infoValue->setObjectName("InfoValue");
//        infoValue->setMinimumHeight(28);
        infoValue->setMinimumWidth(200);
        infoValue->setMaximumWidth(260);
        infoValue->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        infoValue->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        infoValue->setForegroundRole(DPalette::WindowText);
        DFontSizeManager::instance()->bind(infoValue, DFontSizeManager::T8);
        valueList << infoValue;

        infogridLayout->addWidget(infoKey);
        infogridLayout->addWidget(infoValue);
    }
    infoLayout->addWidget(basicinfo);
    infoLayout->addLayout(infogridLayout);

    q->connect(closeBt, &MusicImageButton::clicked, q, &DAbstractDialog::hide);
    q->connect(qApp, &QGuiApplication::fontChanged, q, [ = ](const QFont &font) {
        QFontMetrics fm(font);
        for (int i = 0; i < keyList.size();i++) {

            int w = keyList.at(0)->width();
            int value_w = 300 - w;
            valueList.at(i)->setFixedWidth(value_w - 20);
        }
        if ( meta->size > 1.0){
            q->updateInfo(meta);
        }
    });
    q->updateInfo(meta);
}


void InfoDialogPrivate::updateLabelSize()
{
    Q_Q(InfoDialog);
    title->adjustSize();
    auto h = 0;
    for (auto label : valueList) {
//        label->adjustSize();
        h += label->size().height() + 6 + 6;
    }
    infoGridFrame->setFixedHeight(h);
    infoGridFrame->adjustSize();
    q->adjustSize();
}

InfoDialog::InfoDialog(QWidget *parent)
    : DAbstractDialog(parent), d_ptr(new InfoDialogPrivate(this))
{
    Q_D(InfoDialog);
    d->initUI();
}

InfoDialog::~InfoDialog()
{

}

void InfoDialog::resizeEvent(QResizeEvent *event)
{
    //Q_D(InfoDialog);
    Dtk::Widget::DAbstractDialog::resizeEvent(event);
}

void InfoDialog::updateInfo(const MetaPtr meta)
{
    Q_D(InfoDialog);
    d->meta = meta;
    QString artist = meta->artist.isEmpty() ? tr("Unknown artist") : meta->artist;
    QString album = meta->album.isEmpty() ? tr("Unknown album") : meta->album;
    QStringList infoValues;
    infoValues << meta->title << artist << album
               << meta->filetype << DMusic::sizeString(meta->size) << DMusic::lengthString(meta->length)
               << meta->localPath;

    for (int i = 0; i < d->valueList.length(); ++i) {
        if (i != d->valueList.length() - 1) {
            QString str = geteElidedText(d->valueList.value(i)->font(), infoValues.value(i), d->valueList.value(i)->width());
            d->valueList.value(i)->setText(str);
            QFontMetrics fontWidth(d->valueList.value(i)->font());
            QRect rec = fontWidth.boundingRect( d->valueList.value(i)->text());
            d->valueList.value(i)->setFixedHeight(rec.height());
        } else {
            QFontMetrics fontWidth(d->valueList.value(i)->font());
            int width = fontWidth.width(infoValues.value(i));  //计算字符串宽度
            if (width >= d->valueList.value(i)->width()) { //当字符串宽度大于最大宽度时进行转换
                //两行
                d->DoubleElements = true;
                QString str = geteElidedText(d->valueList.value(i)->font(), infoValues.value(i), d->valueList.value(i)->width() * 3 / 2);
                d->valueList.value(i)->setText(str);
                QRect rec = fontWidth.boundingRect( d->valueList.value(i)->text());
                d->valueList.value(i)->setFixedHeight(2 * rec.height());

            } else {
                d->DoubleElements = false;
                //QString str = geteElidedText(d->valueList.value(i)->font(), infoValues.value(i), d->valueList.value(i)->width() / 2);
                d->valueList.value(i)->setText(infoValues.value(i));
                QRect rec = fontWidth.boundingRect( d->valueList.value(i)->text());
                d->valueList.value(i)->setFixedHeight( rec.height());
            }
        }

        QFileInfo fileInfo(meta->localPath);
        QString titleStr(meta->title + "-" + meta->artist + "." + fileInfo.suffix());
        titleStr = geteElidedText(d->title->font(), titleStr, d->title->width());
        d->title->setText(titleStr);

        auto pixmapSize = static_cast<int>(CoverSize * d->cover->devicePixelRatioF());
        auto coverPixmap = QIcon(":/common/image/info_cover.svg").pixmap(QSize(pixmapSize, pixmapSize));
        QImage cover = coverPixmap.toImage();
        auto coverData = MetaSearchService::coverData(meta);
        if (coverData.length() > 0) {
            cover = QImage::fromData(coverData);
        }
        coverPixmap = QPixmap::fromImage(WidgetHelper::cropRect(cover, QSize(pixmapSize, pixmapSize)));
        coverPixmap.setDevicePixelRatio(d->cover->devicePixelRatioF());
        d->cover->setCoverPixmap(coverPixmap);
        d->cover->update();
        d->updateLabelSize();

        d->title->setFocus();
    }

    int h = 0;//one Label Height
    for (int i = 0;i < d->valueList.size(); i++) {
        h = d->valueList.value(i)->height();
        if(h != 0)
            break;
    }
    int title_height = d->title->height();
    int grideframe_height = d->infoGridFrame->height();
    int total = title_height + grideframe_height + 192 + 70;
    if (d->DoubleElements) {
        setFixedHeight(total + h);
    } else {
        setFixedHeight(total + 22);
    }
}

void InfoDialog::setThemeType(int type)
{
    Q_D(InfoDialog);
    QString rStr;
    if (type == 1) {
        rStr = "light";

        DPalette framePl = this->palette();
        QColor frameColor("#F7F7F7");
        frameColor.setAlphaF(0.8);
        framePl.setColor(DPalette::Window, frameColor);
        this->setBackgroundRole(DPalette::Window);

        DPalette pl = d->infoGridFrame->palette();
        QColor windowColor("#FFFFFF");
        windowColor.setAlphaF(0.7);
        pl.setColor(DPalette::Window, windowColor);

        QColor sbcolor("#000000");
        sbcolor.setAlphaF(0.05);
        pl.setColor(DPalette::Shadow, sbcolor);
        d->infoGridFrame->setPalette(pl);
        d->infoGridFrame->setBackgroundRole(DPalette::Window);
    } else {
        rStr = "dark";

        DPalette framePl = this->palette();
        QColor frameColor("#191919");
        frameColor.setAlphaF(0.8);
        framePl.setColor(DPalette::Window, frameColor);
        this->setBackgroundRole(DPalette::Window);


        DPalette pl = d->infoGridFrame->palette();
        QColor windowColor("#282828");
        //windowColor.setAlphaF(0.05);
        pl.setColor(DPalette::Window, windowColor);
        QColor sbcolor("#FFFFFF");
        sbcolor.setAlphaF(0.1);
        pl.setColor(DPalette::Shadow, sbcolor);
        d->infoGridFrame->setPalette(pl);
        d->infoGridFrame->setBackgroundRole(DPalette::Window);
    }
}

QString InfoDialog::geteElidedText(QFont font, QString str, int MaxWidth)
{
    QFontMetrics fontWidth(font);
    int width = fontWidth.width(str) + 10;  //计算字符串宽度,+10提前进入省略，避免右边遮挡
    if (width >= MaxWidth) { //当字符串宽度大于最大宽度时进行转换
        str = fontWidth.elidedText(str, Qt::ElideMiddle, MaxWidth); //中间显示省略号
    }
    return str;
}
