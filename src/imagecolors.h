/*
 *  Copyright 2020 Marco Martin <mart@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  2.010-1301, USA.
 */

#pragma once

#include <QObject>
#include <QColor>
#include <QImage>
#include <QQuickItem>
#include <QQuickItemGrabResult>
#include <QPointer>
#include <QQuickWindow>
#include <QFuture>

class QTimer;

struct ImageData {
    struct colorStat {
        QList<QRgb> colors;
        QRgb centroid = 0;
        qreal ratio = 0;
    };

    struct colorSet {
        QColor average;
        QColor text;
        QColor background;
        QColor highlight;
    };

    QList<QRgb> m_samples;
    QList<colorStat> m_clusters;
    QVariantList m_palette;

    bool m_darkPalette = true;
    QColor m_dominant;
    QColor m_dominantContrast;
    QColor m_average;
    QColor m_highlight;

    QColor m_closestToBlack;
    QColor m_closestToWhite;
};

class ImageColors : public QObject
{
    Q_OBJECT
    /**
     * The source which colors should be analyzed, it can be:
     * * Any Item: it will be rendered to an image and the static grab 
     * analyzed. It won't be updated if the item changes, but manual 
     * calls to update() are needed.
     * * A QImage (for example coming from a QAbstractItemModel data role)
     * * A QIcon (for example coming from a QAbstractItemModel data role)
     * * An icon name: an icon name present in the theme.
     */
    Q_PROPERTY(QVariant source READ source WRITE setSource NOTIFY sourceChanged)

    /**
     * A list of the color palette extracted from the image.
     * It uses K-means-clustering tecnique, by averaging groups of
     * "similar" colors https://en.wikipedia.org/wiki/K-means_clustering
     * it's a list of maps containing the following keys:
     * * "color": the color of the cluster
     * * "ratio": the ratio from 0 t0 1 of diffusion of the cluster in the image
     * * "contrastingColor": another color from the clusters (if possible) that is the nearest to its negative
     * The maps are ordered by "ratio": the first element is the dominant color of the image
     */
    Q_PROPERTY(QVariantList palette READ palette NOTIFY paletteChanged)

    /**
     * If true, it should be considered a "Dark" color palette (this if the dominant color is darker than a 50% gray
     */
    Q_PROPERTY(bool isDarkPalette READ isDarkPalette NOTIFY paletteChanged)

    /**
     * The average color of the whole image.
     */
    Q_PROPERTY(QColor average READ average NOTIFY paletteChanged)

    /**
     * The dominant color of the image. This is the color of the cluster which covers the bigger area of the image
     */
    Q_PROPERTY(QColor dominant READ dominant NOTIFY paletteChanged)

    /**
     * Suggested "contrasting" color to the dominant one. It's the color in the palette nearest to the negative of the dominant
     */
    Q_PROPERTY(QColor dominantContrast READ dominantContrast NOTIFY paletteChanged)

    /**
     * An "accent" color extracted from the image, heuristically found most "vibrant" color from the cluster
     */
    Q_PROPERTY(QColor highlight READ highlight NOTIFY paletteChanged)

    /**
     * A color suggested for foreground items over the image
     * * on dark palettes will be closestToWhite if light enough, or a very light gray otherwise
     * * on light palettes will be closestToBlack if dark enough or a very dark gray
     */
    Q_PROPERTY(QColor foreground READ foreground NOTIFY paletteChanged)

    /**
     * A color suggested for background items over the image, like a dialog frame
     * * on light palettes will be closestToWhite if light enough, or a very light gray otherwise
     * * on dark palettes will be closestToBlack if dark enough or a very dark gray
     */
    Q_PROPERTY(QColor background READ background NOTIFY paletteChanged)

    /**
     * The lightest color that was found in clusters
     */
    Q_PROPERTY(QColor closestToWhite READ closestToWhite NOTIFY paletteChanged)

    /**
     * The darkest color found in clusters
     */
    Q_PROPERTY(QColor closestToBlack READ closestToBlack NOTIFY paletteChanged)

public:
    explicit ImageColors(QObject* parent = nullptr);
    ~ImageColors();

    void setSource(const QVariant &source);
    QVariant source() const;

    void setSourceImage(const QImage &image);
    QImage sourceImage() const;

    void setSourceItem(QQuickItem *source);
    QQuickItem *sourceItem() const;

    Q_INVOKABLE void update();

    QVariantList palette() const;
    bool isDarkPalette() const;
    QColor average() const;
    QColor dominant() const;
    QColor dominantContrast() const;
    QColor highlight() const;
    QColor foreground() const;
    QColor background() const;
    QColor closestToWhite() const;
    QColor closestToBlack() const;

Q_SIGNALS:
    void sourceChanged();
    void paletteChanged();

private:
    static inline void positionColor(QRgb rgb, QList<ImageData::colorStat> &clusters);
    static ImageData generatePalette(const QImage &sourceImage);

    // Arbitrary number that seems to work well
    static const int s_minimumSquareDistance = 32000;
    QPointer<QQuickWindow> m_window;
    QVariant m_source;
    QPointer<QQuickItem> m_sourceItem;
    QSharedPointer<QQuickItemGrabResult> m_grabResult;
    QImage m_sourceImage;


    QTimer *m_imageSyncTimer;

    QFutureWatcher<ImageData> *m_futureImageData = nullptr;
    ImageData m_imageData;
};

