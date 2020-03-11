/*
 * Copyright 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

uniform highp mat4 matrix;
uniform lowp vec2 aspect;
uniform lowp vec2 offset;

attribute highp vec4 in_vertex;
attribute mediump vec2 in_uv;

varying mediump vec2 uv;

void main() {
    uv = (-1.0 + 2.0 * in_uv) * aspect;
    gl_Position = matrix * in_vertex;
}
