/*
 * Copyright 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <https://www.gnu.org/licenses/>
 */

#pragma once

#include <QSGMaterial>
#include <QSGMaterialShader>

class ShadowedRectangleMaterial : public QSGMaterial
{
public:
    ShadowedRectangleMaterial();

    QSGMaterialShader* createShader() const override;
    QSGMaterialType* type() const override;
    int compare(const QSGMaterial* other) const override;

    QVector2D aspect = QVector2D{1.0, 1.0};
    float size = 0.0;
    float radius = 0.0;
    QColor color = Qt::white;
    QColor shadowColor = Qt::black;
    QVector2D offset;
};

class ElevatedRectangleShader : public QSGMaterialShader
{
public:
    ElevatedRectangleShader();

    char const *const *attributeNames() const override;

    void initialize() override;
    void updateState(const QSGMaterialShader::RenderState &state, QSGMaterial *newMaterial, QSGMaterial *oldMaterial) override;

private:
    int m_matrixLocation = -1;
    int m_opacityLocation = -1;
    int m_aspectLocation = -1;
    int m_sizeLocation = -1;
    int m_radiusLocation = -1;
    int m_colorLocation = -1;
    int m_shadowColorLocation = -1;
    int m_offsetLocation = -1;
};
