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

uniform lowp float opacity;
uniform lowp float elevation;
uniform lowp float radius;
uniform lowp vec4 color;
uniform lowp vec4 shadowColor;
uniform lowp vec2 offset;
uniform lowp vec2 aspect;

varying lowp vec2 uv;

lowp float sdf_rectangle(in lowp vec2 point, in lowp vec2 rect, in lowp vec2 translation, in lowp float radius)
{
    lowp vec2 d = abs(point - translation) - rect;
    return (length(max(d, vec2(0.0))) + min(max(d.x, d.y), 0.0)) - radius;
}

lowp vec4 sdf_render(in lowp float sdf, in lowp vec4 sourceColor, in lowp vec4 sdfColor, in lowp float smoothing)
{
    lowp float g = fwidth(sdf);
    return mix(sourceColor, sdfColor, sdfColor.a * (1.0 - smoothstep(smoothing - g, smoothing + g, sdf)));
}

void main()
{
    lowp float shadowRadius = radius + elevation * (0.25 * (0.1 / max(radius, 0.1)));
    lowp vec2 aspectOffset = aspect - vec2(1.0);

    lowp vec4 col = vec4(0.0);
    if (elevation > 0.0) {
        lowp float shadow = sdf_rectangle(uv, aspect - shadowRadius - elevation, aspectOffset + offset / 2.0, shadowRadius);
        col = mix(col, shadowColor, 1.0 - smoothstep(-elevation * 0.5, elevation * 0.5, shadow));
    }

    lowp float rect = sdf_rectangle(uv, aspect - radius - elevation, aspectOffset - offset / 2.0, radius);
    col = sdf_render(rect, col, color, 0.001);

    gl_FragColor = col;
}

