/*
 * Copyright (C) 2011, 2014 Apple Inc. All rights reserved.
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <wtf/Deque.h>
#include <wtf/HashMap.h>
#include <wtf/HashSet.h>
#include <wtf/RetainPtr.h>
#include <wtf/Vector.h>

#if PLATFORM(WPE)
#include <wpe/wpe.h>
#endif

#if PLATFORM(COCOA)
OBJC_CLASS NSEvent;
#endif

namespace WTR {

class TestController;

class EventSenderProxy {
    WTF_MAKE_FAST_ALLOCATED;
public:
    explicit EventSenderProxy(TestController*);
    ~EventSenderProxy();

    WKPoint position() const { return m_position; }

    void mouseDown(unsigned button, WKEventModifiers);
    void mouseUp(unsigned button, WKEventModifiers);
    void mouseForceDown();
    void mouseForceUp();
    void mouseForceChanged(float);
    void mouseForceClick();
    void startAndCancelMouseForceClick();
    void mouseMoveTo(double x, double y);
    void mouseScrollBy(int x, int y);
    void mouseScrollByWithWheelAndMomentumPhases(int x, int y, int phase, int momentum);
    void continuousMouseScrollBy(int x, int y, bool paged);

    void leapForward(int milliseconds);

    void keyDown(WKStringRef key, WKEventModifiers, unsigned location);

#if PLATFORM(COCOA)
    unsigned mouseButtonsCurrentlyDown() const { return m_mouseButtonsCurrentlyDown; }
#endif

#if ENABLE(TOUCH_EVENTS)
    // Touch events.
    void addTouchPoint(int x, int y);
    void updateTouchPoint(int index, int x, int y);
    void setTouchModifier(WKEventModifiers, bool enable);
    void setTouchPointRadius(int radiusX, int radiusY);
    void touchStart();
    void touchMove();
    void touchEnd();
    void touchCancel();
    void clearTouchPoints();
    void releaseTouchPoint(int index);
    void cancelTouchPoint(int index);
#endif

private:
    TestController* m_testController;

    double currentEventTime() { return m_time; }
    void updateClickCountForButton(int button);

    void sendMouseDownToStartPressureEvents();
#if PLATFORM(COCOA)
    enum class PressureChangeDirection { Increasing, Decreasing };
    RetainPtr<NSEvent> beginPressureEvent(int stage);
    RetainPtr<NSEvent> pressureChangeEvent(int stage, PressureChangeDirection);
    RetainPtr<NSEvent> pressureChangeEvent(int stage, float pressure, PressureChangeDirection);
#endif

#if PLATFORM(WPE)
    Vector<struct wpe_input_touch_event_raw> getUpdatedTouchEvents();
    void removeUpdatedTouchEvents();
    void prepareAndDispatchTouchEvent(enum wpe_input_touch_event_type);
#endif

#if PLATFORM(WIN)
    LRESULT dispatchMessage(UINT message, WPARAM, LPARAM);
    POINT positionInPoint() const { return { static_cast<LONG>(m_position.x), static_cast<LONG>(m_position.y) }; }
#endif

    double m_time;
    WKPoint m_position;
    bool m_leftMouseButtonDown;
    int m_clickCount;
    double m_clickTime;
    WKPoint m_clickPosition;
    WKEventMouseButton m_clickButton;
    unsigned m_mouseButtonsCurrentlyDown { 0 };
#if PLATFORM(COCOA)
    int eventNumber;
#elif PLATFORM(WPE)
    uint32_t m_buttonState;
    Vector<struct wpe_input_touch_event_raw> m_touchEvents;
    HashSet<unsigned, DefaultHash<unsigned>::Hash, WTF::UnsignedWithZeroKeyHashTraits<unsigned>> m_updatedTouchEvents;
#endif
};

} // namespace WTR
