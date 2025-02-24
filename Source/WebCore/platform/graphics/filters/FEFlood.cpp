/*
 * Copyright (C) 2004, 2005, 2006, 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005 Rob Buis <buis@kde.org>
 * Copyright (C) 2005 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2009 Dirk Schulze <krit@webkit.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "FEFlood.h"

#include "Filter.h"
#include "GraphicsContext.h"
#include <wtf/text/TextStream.h>

namespace WebCore {

FEFlood::FEFlood(Filter& filter, const Color& floodColor, float floodOpacity)
    : FilterEffect(filter)
    , m_floodColor(floodColor)
    , m_floodOpacity(floodOpacity)
{
}

Ref<FEFlood> FEFlood::create(Filter& filter, const Color& floodColor, float floodOpacity)
{
    return adoptRef(*new FEFlood(filter, floodColor, floodOpacity));
}

bool FEFlood::setFloodColor(const Color& color)
{
    if (m_floodColor == color)
        return false;
    m_floodColor = color;
    return true;
}

bool FEFlood::setFloodOpacity(float floodOpacity)
{
    if (m_floodOpacity == floodOpacity)
        return false;
    m_floodOpacity = floodOpacity;
    return true;
}

void FEFlood::platformApplySoftware()
{
    ImageBuffer* resultImage = createImageBufferResult();
    if (!resultImage)
        return;

    auto color = floodColor().colorWithAlphaMultipliedBy(floodOpacity());
    resultImage->context().fillRect(FloatRect(FloatPoint(), absolutePaintRect().size()), color);
}

TextStream& FEFlood::externalRepresentation(TextStream& ts, RepresentationType representation) const
{
    ts << indent << "[feFlood";
    FilterEffect::externalRepresentation(ts, representation);
    ts << " flood-color=\"" << floodColor().nameForRenderTreeAsText() << "\" "
       << "flood-opacity=\"" << floodOpacity() << "\"]\n";
    return ts;
}

} // namespace WebCore
