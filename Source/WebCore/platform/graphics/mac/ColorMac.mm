/*
 * Copyright (C) 2003, 2004, 2005, 2006 Apple Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#import "config.h"
#import "ColorMac.h"

#if USE(APPKIT)

#import "LocalCurrentGraphicsContext.h"
#import <wtf/BlockObjCExceptions.h>
#import <wtf/NeverDestroyed.h>
#import <wtf/RetainPtr.h>
#import <wtf/TinyLRUCache.h>

namespace WTF {

template<> RetainPtr<NSColor> TinyLRUCachePolicy<WebCore::Color, RetainPtr<NSColor>>::createValueForKey(const WebCore::Color& color)
{
    return [NSColor colorWithCGColor:cachedCGColor(color)];
}

} // namespace WTF

namespace WebCore {

static bool useOldAquaFocusRingColor;

Color oldAquaFocusRingColor()
{
    return SimpleColor { 0xFF7DADD9 };
}

void setUsesTestModeFocusRingColor(bool newValue)
{
    useOldAquaFocusRingColor = newValue;
}

bool usesTestModeFocusRingColor()
{
    return useOldAquaFocusRingColor;
}

static SimpleColor makeSimpleColorFromNSColor(NSColor *color)
{
    // FIXME: ExtendedColor - needs to handle color spaces.

    ASSERT_ARG(color, color);

    CGFloat redComponent;
    CGFloat greenComponent;
    CGFloat blueComponent;
    CGFloat alpha;

    BEGIN_BLOCK_OBJC_EXCEPTIONS;
    NSColor *rgbColor = [color colorUsingColorSpace:NSColorSpace.deviceRGBColorSpace];
    if (!rgbColor) {
        // The color space conversion above can fail if the NSColor is in the NSPatternColorSpace.
        // These colors are actually a repeating pattern, not just a solid color. To workaround
        // this we simply draw a one pixel image of the color and use that pixel's color.
        // FIXME: It might be better to use an average of the colors in the pattern instead.
        RetainPtr<NSBitmapImageRep> offscreenRep = adoptNS([[NSBitmapImageRep alloc] initWithBitmapDataPlanes:nil pixelsWide:1 pixelsHigh:1
            bitsPerSample:8 samplesPerPixel:4 hasAlpha:YES isPlanar:NO colorSpaceName:NSDeviceRGBColorSpace bytesPerRow:4 bitsPerPixel:32]);

        GraphicsContext bitmapContext([NSGraphicsContext graphicsContextWithBitmapImageRep:offscreenRep.get()].CGContext);
        LocalCurrentGraphicsContext localContext(bitmapContext);

        [color drawSwatchInRect:NSMakeRect(0, 0, 1, 1)];

        NSUInteger pixel[4];
        [offscreenRep getPixel:pixel atX:0 y:0];

        return makeSimpleColor(pixel[0], pixel[1], pixel[2], pixel[3]);
    }

    [rgbColor getRed:&redComponent green:&greenComponent blue:&blueComponent alpha:&alpha];
    END_BLOCK_OBJC_EXCEPTIONS;

    return makeSimpleColorFromFloats(redComponent, greenComponent, blueComponent, alpha);
}

Color colorFromNSColor(NSColor *color)
{
    if (!color)
        return { };

    // FIXME: ExtendedColor - needs to handle color spaces.

    return makeSimpleColorFromNSColor(color);
}

Color semanticColorFromNSColor(NSColor *color)
{
    if (!color)
        return { };

    // FIXME: ExtendedColor - needs to handle color spaces.

    return Color(makeSimpleColorFromNSColor(color), Color::Semantic);
}

NSColor *nsColor(const Color& color)
{
    if (color.isSimple()) {
        switch (color.asSimple().value()) {
        case Color::transparent.value(): {
            static NSColor *clearColor = [[NSColor colorWithSRGBRed:0 green:0 blue:0 alpha:0] retain];
            return clearColor;
        }
        case Color::black.value(): {
            static NSColor *blackColor = [[NSColor colorWithSRGBRed:0 green:0 blue:0 alpha:1] retain];
            return blackColor;
        }
        case Color::white.value(): {
            static NSColor *whiteColor = [[NSColor colorWithSRGBRed:1 green:1 blue:1 alpha:1] retain];
            return whiteColor;
        }
        }
    }

    static NeverDestroyed<TinyLRUCache<Color, RetainPtr<NSColor>, 32>> cache;
    return cache.get().get(color).get();
}

} // namespace WebCore

#endif // USE(APPKIT)
