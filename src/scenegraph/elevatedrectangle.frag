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

// This is based on the 2D SDF functions provided by Inigo Quilez:
// https://iquilezles.org/www/articles/distfunctions2d/distfunctions2d.htm

uniform lowp float opacity;
uniform lowp float elevation;
uniform lowp float radius;
uniform lowp vec4 color;
uniform lowp vec4 shadowColor;
uniform lowp vec2 offset;
uniform lowp vec2 aspect;

varying lowp vec2 uv;

const lowp float minimum_shadow_radius = 0.05;

// Calculate the distance to a rectangle with rounded corners.
// \param point The point to calculate the distance of.
// \param rect The rectangle to calculate the distance of.
// \param translation The amount of translation to apply to the rectangle.
// \param radius A vec4 with the radius of each corner.
lowp float sdf_rounded_rectangle(in lowp vec2 point, in lowp vec2 rect, in lowp vec2 translation, in lowp vec4 radius)
{
    radius.xy = (point.x > 0.0) ? radius.xy : radius.zw;
    radius.x = (point.y > 0.0) ? radius.x : radius.y;
    lowp vec2 d = abs(point - translation) - rect + radius.x;
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - radius.x;
}

void main()
{
    // Scaling factor that is the inverse of the amount of scaling applied to the geometry.
    lowp float inverse_scale = 1.0 / (1.0 + elevation + length(offset * 2.0));

    // Correction factor to round the corners of a larger shadow.
    // We want to account for elevation in regards to shadow radius, so that a larger shadow is
    // more rounded, but only if we are not already rounding the corners due to corner radius.
    lowp float elevation_factor = 0.5 * (minimum_shadow_radius / max(radius, minimum_shadow_radius));

    lowp float shadowRadius = radius + elevation * elevation_factor;

    lowp vec4 col = vec4(0.0);

    // Calculate the shadow's distance field.
    lowp float shadow = sdf_rounded_rectangle(uv, aspect * inverse_scale, offset * inverse_scale, vec4(shadowRadius * inverse_scale));
    // Render it, interpolating the color over the distance.
    col = mix(col, shadowColor * sign(elevation), shadowColor.a * (1.0 - smoothstep(-elevation * 0.5, elevation * 0.5, shadow)));

    // Calculate the main rectangle distance field.
    lowp float rect = sdf_rounded_rectangle(uv, aspect * inverse_scale, vec2(0.0), vec4(radius * inverse_scale));
    // Render it as a normal rectangle.
    lowp float g = fwidth(rect);
    col = mix(col, color, color.a * (1.0 - smoothstep(0.001 - g, 0.001 + g, rect)));

    gl_FragColor = col;
}
