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

#include "shadowedrectanglematerial.h"

#include <QOpenGLContext>

ShadowedRectangleMaterial::ShadowedRectangleMaterial()
{
    setFlag(QSGMaterial::Blending, true);
}

QSGMaterialShader* ShadowedRectangleMaterial::createShader() const
{
    return new ElevatedRectangleShader{};
}

QSGMaterialType* ShadowedRectangleMaterial::type() const
{
    static QSGMaterialType type;
    return &type;
}

int ShadowedRectangleMaterial::compare(const QSGMaterial *other) const
{
    auto material = static_cast<const ShadowedRectangleMaterial *>(other);

    if (material->color == color
        && material->shadowColor == shadowColor
        && material->offset == offset
        && material->aspect == aspect
        && qFuzzyCompare(material->size, size)
        && qFuzzyCompare(material->radius, radius)) {
        return 0;
    }

    return QSGMaterial::compare(other);
}

ElevatedRectangleShader::ElevatedRectangleShader()
{
    auto header = QOpenGLContext::currentContext()->isOpenGLES() ? QStringLiteral("header_es.glsl") : QStringLiteral("header_desktop.glsl");

    auto shaderRoot = QStringLiteral(":/org/kde/kirigami/shaders/");

    setShaderSourceFiles(QOpenGLShader::Vertex, {
        shaderRoot + header,
        shaderRoot + QStringLiteral("shadowedrectangle.vert")
    });

    setShaderSourceFiles(QOpenGLShader::Fragment, {
        shaderRoot + header,
        shaderRoot + QStringLiteral("shadowedrectangle.frag")
    });
}

const char *const * ElevatedRectangleShader::attributeNames() const
{
    static char const *const names[] = {"in_vertex", "in_uv", nullptr};
    return names;
}

void ElevatedRectangleShader::initialize()
{
    QSGMaterialShader::initialize();
    m_matrixLocation = program()->uniformLocation("matrix");
    m_aspectLocation = program()->uniformLocation("aspect");
    m_opacityLocation = program()->uniformLocation("opacity");
    m_sizeLocation = program()->uniformLocation("size");
    m_radiusLocation = program()->uniformLocation("radius");
    m_colorLocation = program()->uniformLocation("color");
    m_shadowColorLocation = program()->uniformLocation("shadowColor");
    m_offsetLocation = program()->uniformLocation("offset");
}

void ElevatedRectangleShader::updateState(const QSGMaterialShader::RenderState& state, QSGMaterial* newMaterial, QSGMaterial* oldMaterial)
{
    auto p = program();

    if (state.isMatrixDirty()) {
        p->setUniformValue(m_matrixLocation, state.combinedMatrix());
    }

    if (state.isOpacityDirty()) {
        p->setUniformValue(m_opacityLocation, state.opacity());
    }

    if (!oldMaterial || newMaterial->compare(oldMaterial) != 0 || state.isCachedMaterialDataDirty()) {
        auto material = static_cast<ShadowedRectangleMaterial *>(newMaterial);
        p->setUniformValue(m_aspectLocation, material->aspect);
        p->setUniformValue(m_sizeLocation, material->size);
        p->setUniformValue(m_radiusLocation, material->radius);
        p->setUniformValue(m_colorLocation, material->color);
        p->setUniformValue(m_shadowColorLocation, material->shadowColor);
        p->setUniformValue(m_offsetLocation, material->offset);
    }
}
