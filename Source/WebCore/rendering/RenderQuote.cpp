/*
 * Copyright (C) 2011 Nokia Inc.  All rights reserved.
 * Copyright (C) 2012 Google Inc. All rights reserved.
 * Copyright (C) 2013, 2017 Apple Inc. All rights reserved.
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
 *
 */

#include "config.h"
#include "RenderQuote.h"

#include "QuotesData.h"
#include "RenderTextFragment.h"
#include "RenderTreeBuilder.h"
#include "RenderView.h"
#include <wtf/IsoMallocInlines.h>
#include <wtf/unicode/CharacterNames.h>

namespace WebCore {
using namespace WTF::Unicode;

WTF_MAKE_ISO_ALLOCATED_IMPL(RenderQuote);

RenderQuote::RenderQuote(Document& document, RenderStyle&& style, QuoteType quote)
    : RenderInline(document, WTFMove(style))
    , m_type(quote)
    , m_text(emptyString())
{
}

RenderQuote::~RenderQuote()
{
    // Do not add any code here. Add it to willBeDestroyed() instead.
}

void RenderQuote::insertedIntoTree()
{
    RenderInline::insertedIntoTree();
    view().setHasQuotesNeedingUpdate(true);
}

void RenderQuote::willBeRemovedFromTree()
{
    view().setHasQuotesNeedingUpdate(true);
    RenderInline::willBeRemovedFromTree();
}

void RenderQuote::styleDidChange(StyleDifference diff, const RenderStyle* oldStyle)
{
    RenderInline::styleDidChange(diff, oldStyle);
    if (diff >= StyleDifference::Layout) {
        m_needsTextUpdate = true;
        view().setHasQuotesNeedingUpdate(true);
    }
}

const unsigned maxDistinctQuoteCharacters = 16;

#if ASSERT_ENABLED

static void checkNumberOfDistinctQuoteCharacters(UChar character)
{
    ASSERT(character);
    static UChar distinctQuoteCharacters[maxDistinctQuoteCharacters];
    for (unsigned i = 0; i < maxDistinctQuoteCharacters; ++i) {
        if (distinctQuoteCharacters[i] == character)
            return;
        if (!distinctQuoteCharacters[i]) {
            distinctQuoteCharacters[i] = character;
            return;
        }
    }
    ASSERT_NOT_REACHED();
}

#endif // ASSERT_ENABLED

struct QuotesForLanguage {
    const char* language;
    UChar open1;
    UChar close1;
    UChar open2;
    UChar close2;
};

static int quoteTableLanguageComparisonFunction(const void* a, const void* b)
{
    return strcmp(static_cast<const QuotesForLanguage*>(a)->language,
        static_cast<const QuotesForLanguage*>(b)->language);
}

static const QuotesForLanguage* quotesForLanguage(const String& language)
{
    // Table of quotes from http://www.whatwg.org/specs/web-apps/current-work/multipage/rendering.html#quotes
    static const QuotesForLanguage quoteTable[] = {
        { "af",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "agq",        0x201e, 0x201d, 0x201a, 0x2019 },
        { "ak",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "am",         0x00ab, 0x00bb, 0x2039, 0x203a },
        { "ar",         0x201d, 0x201c, 0x2019, 0x2018 },
        { "asa",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "az-cyrl",    0x00ab, 0x00bb, 0x2039, 0x203a },
        { "bas",        0x00ab, 0x00bb, 0x201e, 0x201c },
        { "bem",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "bez",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "bg",         0x201e, 0x201c, 0x201a, 0x2018 },
        { "bm",         0x00ab, 0x00bb, 0x201c, 0x201d },
        { "bn",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "br",         0x00ab, 0x00bb, 0x2039, 0x203a },
        { "brx",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "bs-cyrl",    0x201e, 0x201c, 0x201a, 0x2018 },
        { "ca",         0x201c, 0x201d, 0x00ab, 0x00bb },
        { "cgg",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "chr",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "cs",         0x201e, 0x201c, 0x201a, 0x2018 },
        { "da",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "dav",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "de",         0x201e, 0x201c, 0x201a, 0x2018 },
        { "de-ch",      0x00ab, 0x00bb, 0x2039, 0x203a },
        { "dje",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "dua",        0x00ab, 0x00bb, 0x2018, 0x2019 },
        { "dyo",        0x00ab, 0x00bb, 0x201c, 0x201d },
        { "dz",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "ebu",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "ee",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "el",         0x00ab, 0x00bb, 0x201c, 0x201d },
        { "en",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "en-gb",      0x201c, 0x201d, 0x2018, 0x2019 },
        { "es",         0x201c, 0x201d, 0x00ab, 0x00bb },
        { "et",         0x201e, 0x201c, 0x201a, 0x2018 },
        { "eu",         0x201c, 0x201d, 0x00ab, 0x00bb },
        { "ewo",        0x00ab, 0x00bb, 0x201c, 0x201d },
        { "fa",         0x00ab, 0x00bb, 0x2039, 0x203a },
        { "ff",         0x201e, 0x201d, 0x201a, 0x2019 },
        { "fi",         0x201d, 0x201d, 0x2019, 0x2019 },
        { "fr",         0x00ab, 0x00bb, 0x00ab, 0x00bb },
        { "fr-ca",      0x00ab, 0x00bb, 0x2039, 0x203a },
        { "fr-ch",      0x00ab, 0x00bb, 0x2039, 0x203a },
        { "gsw",        0x00ab, 0x00bb, 0x2039, 0x203a },
        { "gu",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "guz",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "ha",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "he",         0x0022, 0x0022, 0x0027, 0x0027 },
        { "hi",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "hr",         0x201e, 0x201c, 0x201a, 0x2018 },
        { "hu",         0x201e, 0x201d, 0x00bb, 0x00ab },
        { "id",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "ig",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "it",         0x00ab, 0x00bb, 0x201c, 0x201d },
        { "ja",         0x300c, 0x300d, 0x300e, 0x300f },
        { "jgo",        0x00ab, 0x00bb, 0x2039, 0x203a },
        { "jmc",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "kab",        0x00ab, 0x00bb, 0x201c, 0x201d },
        { "kam",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "kde",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "kea",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "khq",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "ki",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "kkj",        0x00ab, 0x00bb, 0x2039, 0x203a },
        { "kln",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "km",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "kn",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "ko",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "ksb",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "ksf",        0x00ab, 0x00bb, 0x2018, 0x2019 },
        { "lag",        0x201d, 0x201d, 0x2019, 0x2019 },
        { "lg",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "ln",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "lo",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "lt",         0x201e, 0x201c, 0x201e, 0x201c },
        { "lu",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "luo",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "luy",        0x201e, 0x201c, 0x201a, 0x2018 },
        { "lv",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "mas",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "mer",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "mfe",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "mg",         0x00ab, 0x00bb, 0x201c, 0x201d },
        { "mgo",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "mk",         0x201e, 0x201c, 0x201a, 0x2018 },
        { "ml",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "mr",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "ms",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "mua",        0x00ab, 0x00bb, 0x201c, 0x201d },
        { "my",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "naq",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "nb",         0x00ab, 0x00bb, 0x2018, 0x2019 },
        { "nd",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "nl",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "nmg",        0x201e, 0x201d, 0x00ab, 0x00bb },
        { "nn",         0x00ab, 0x00bb, 0x2018, 0x2019 },
        { "nnh",        0x00ab, 0x00bb, 0x201c, 0x201d },
        { "nus",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "nyn",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "pl",         0x201e, 0x201d, 0x00ab, 0x00bb },
        { "pt",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "pt-pt",      0x00ab, 0x00bb, 0x201c, 0x201d },
        { "rn",         0x201d, 0x201d, 0x2019, 0x2019 },
        { "ro",         0x201e, 0x201d, 0x00ab, 0x00bb },
        { "rof",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "ru",         0x00ab, 0x00bb, 0x201e, 0x201c },
        { "rw",         0x00ab, 0x00bb, 0x2018, 0x2019 },
        { "rwk",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "saq",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "sbp",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "seh",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "ses",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "sg",         0x00ab, 0x00bb, 0x201c, 0x201d },
        { "shi",        0x00ab, 0x00bb, 0x201e, 0x201d },
        { "shi-tfng",   0x00ab, 0x00bb, 0x201e, 0x201d },
        { "si",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "sk",         0x201e, 0x201c, 0x201a, 0x2018 },
        { "sl",         0x201e, 0x201c, 0x201a, 0x2018 },
        { "sn",         0x201d, 0x201d, 0x2019, 0x2019 },
        { "so",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "sq",         0x201e, 0x201c, 0x201a, 0x2018 },
        { "sr",         0x201e, 0x201c, 0x201a, 0x2018 },
        { "sr-latn",    0x201e, 0x201c, 0x201a, 0x2018 },
        { "sv",         0x201d, 0x201d, 0x2019, 0x2019 },
        { "sw",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "swc",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "ta",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "te",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "teo",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "th",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "ti-er",      0x2018, 0x2019, 0x201c, 0x201d },
        { "to",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "tr",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "twq",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "tzm",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "uk",         0x00ab, 0x00bb, 0x201e, 0x201c },
        { "ur",         0x201d, 0x201c, 0x2019, 0x2018 },
        { "vai",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "vai-latn",   0x201c, 0x201d, 0x2018, 0x2019 },
        { "vi",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "vun",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "xh",         0x2018, 0x2019, 0x201c, 0x201d },
        { "xog",        0x201c, 0x201d, 0x2018, 0x2019 },
        { "yav",        0x00ab, 0x00bb, 0x00ab, 0x00bb },
        { "yo",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "zh",         0x201c, 0x201d, 0x2018, 0x2019 },
        { "zh-hant",    0x300c, 0x300d, 0x300e, 0x300f },
        { "zu",         0x201c, 0x201d, 0x2018, 0x2019 },
    };

    const unsigned maxLanguageLength = 8;

#if ASSERT_ENABLED
    // One time check that the table meets the constraints that the code below relies on.

    static bool didOneTimeCheck = false;
    if (!didOneTimeCheck) {
        didOneTimeCheck = true;

        checkNumberOfDistinctQuoteCharacters(quotationMark);
        checkNumberOfDistinctQuoteCharacters(apostrophe);

        for (unsigned i = 0; i < WTF_ARRAY_LENGTH(quoteTable); ++i) {
            ASSERT(strlen(quoteTable[i].language) <= maxLanguageLength);

            if (i)
                ASSERT(strcmp(quoteTable[i - 1].language, quoteTable[i].language) < 0);

            for (unsigned j = 0; UChar character = quoteTable[i].language[j]; ++j)
                ASSERT(isASCIILower(character) || character == '-');

            checkNumberOfDistinctQuoteCharacters(quoteTable[i].open1);
            checkNumberOfDistinctQuoteCharacters(quoteTable[i].close1);
            checkNumberOfDistinctQuoteCharacters(quoteTable[i].open2);
            checkNumberOfDistinctQuoteCharacters(quoteTable[i].close2);
        }
    }
#endif // ASSERT_ENABLED

    unsigned length = language.length();
    if (!length || length > maxLanguageLength)
        return 0;

    char languageKeyBuffer[maxLanguageLength + 1];
    for (unsigned i = 0; i < length; ++i) {
        UChar character = toASCIILower(language[i]);
        if (!(isASCIILower(character) || character == '-'))
            return 0;
        languageKeyBuffer[i] = static_cast<char>(character);
    }
    languageKeyBuffer[length] = 0;

    QuotesForLanguage languageKey = { languageKeyBuffer, 0, 0, 0, 0 };

    return static_cast<const QuotesForLanguage*>(bsearch(&languageKey,
        quoteTable, WTF_ARRAY_LENGTH(quoteTable), sizeof(quoteTable[0]), quoteTableLanguageComparisonFunction));
}

static StringImpl* stringForQuoteCharacter(UChar character)
{
    // Use linear search because there is a small number of distinct characters, thus binary search is unneeded.
    ASSERT(character);
    struct StringForCharacter {
        UChar character;
        StringImpl* string;
    };
    static StringForCharacter strings[maxDistinctQuoteCharacters];
    for (unsigned i = 0; i < maxDistinctQuoteCharacters; ++i) {
        if (strings[i].character == character)
            return strings[i].string;
        if (!strings[i].character) {
            strings[i].character = character;
            strings[i].string = &StringImpl::create8BitIfPossible(&character, 1).leakRef();
            return strings[i].string;
        }
    }
    ASSERT_NOT_REACHED();
    return StringImpl::empty();
}

static inline StringImpl* quotationMarkString()
{
    static StringImpl* quotationMarkString = stringForQuoteCharacter(quotationMark);
    return quotationMarkString;
}

static inline StringImpl* apostropheString()
{
    static StringImpl* apostropheString = stringForQuoteCharacter(apostrophe);
    return apostropheString;
}

static RenderTextFragment* quoteTextRenderer(RenderObject* lastChild)
{
    if (!lastChild)
        return nullptr;

    if (!is<RenderTextFragment>(lastChild))
        return nullptr;

    return downcast<RenderTextFragment>(lastChild);
}

void RenderQuote::updateTextRenderer(RenderTreeBuilder& builder)
{
    ASSERT_WITH_SECURITY_IMPLICATION(document().inRenderTreeUpdate());
    String text = computeText();
    if (m_text == text)
        return;
    m_text = text;
    if (auto* renderText = quoteTextRenderer(lastChild())) {
        renderText->setContentString(m_text);
        renderText->dirtyLineBoxes(false);
        return;
    }
    builder.attach(*this, createRenderer<RenderTextFragment>(document(), m_text));
}

String RenderQuote::computeText() const
{
    if (m_depth < 0)
        return emptyString();
    bool isOpenQuote = false;
    switch (m_type) {
    case QuoteType::NoOpenQuote:
    case QuoteType::NoCloseQuote:
        return emptyString();
    case QuoteType::OpenQuote:
        isOpenQuote = true;
        FALLTHROUGH;
    case QuoteType::CloseQuote:
        if (const QuotesData* quotes = style().quotes())
            return isOpenQuote ? quotes->openQuote(m_depth).impl() : quotes->closeQuote(m_depth).impl();
        if (const QuotesForLanguage* quotes = quotesForLanguage(style().specifiedLocale()))
            return stringForQuoteCharacter(isOpenQuote ? (m_depth ? quotes->open2 : quotes->open1) : (m_depth ? quotes->close2 : quotes->close1));
        // FIXME: Should the default be the quotes for "en" rather than straight quotes?
        return m_depth ? apostropheString() : quotationMarkString();
    }
    ASSERT_NOT_REACHED();
    return emptyString();
}

bool RenderQuote::isOpen() const
{
    switch (m_type) {
    case QuoteType::OpenQuote:
    case QuoteType::NoOpenQuote:
        return true;
    case QuoteType::CloseQuote:
    case QuoteType::NoCloseQuote:
        return false;
    }
    ASSERT_NOT_REACHED();
    return false;
}

void RenderQuote::updateRenderer(RenderTreeBuilder& builder, RenderQuote* previousQuote)
{
    ASSERT_WITH_SECURITY_IMPLICATION(document().inRenderTreeUpdate());
    int depth = -1;
    if (previousQuote) {
        depth = previousQuote->m_depth;
        if (previousQuote->isOpen())
            ++depth;
    }

    if (!isOpen())
        --depth;
    else if (depth < 0)
        depth = 0;

    if (m_depth == depth && !m_needsTextUpdate)
        return;

    m_depth = depth;
    m_needsTextUpdate = false;
    updateTextRenderer(builder);
}

} // namespace WebCore
