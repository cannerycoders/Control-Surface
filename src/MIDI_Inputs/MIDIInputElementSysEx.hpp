#if 0

#pragma once

#include "MIDIInputElement.hpp"
#include <Helpers/LinkedList.hpp>

#if defined(ESP32)
#include <mutex>
#define GUARD_LIST_LOCK std::lock_guard<std::mutex> _guard(mutex)
#else
#define GUARD_LIST_LOCK
#endif

BEGIN_CS_NAMESPACE

/**
 * @brief   Class for objects that listen for incoming MIDI SysEx events.
 * 
 * @ingroup MIDIInputElements
 */
class MIDIInputElementSysEx : public DoublyLinkable<MIDIInputElementSysEx> {
  protected:
    /**
     * @brief   Constructor.
     * @todo    Documentation.
     */
    MIDIInputElementSysEx(const MIDICNChannelAddress &address)
        : MIDIInputElement{address} {
        GUARD_LIST_LOCK;
        elements.append(this);
    }

  public:
    /**
     * @brief   Destructor.
     * @todo    Documentation.
     */
    virtual ~MIDIInputElementSysEx() {
        GUARD_LIST_LOCK;
        elements.remove(this);
    }

    /**
     * @brief   Initialize all MIDIInputElementSysEx elements.
     * 
     * @see     MIDIInputElementSysEx#begin
     */
    static void beginAll() {
        GUARD_LIST_LOCK;
        for (MIDIInputElementSysEx &e : elements)
            e.begin();
    }

    /**
     * @brief   Update all MIDIInputElementSysEx elements.
     * 
     * @see     MIDIInputElementSysEx#update
     */
    static void updateAll() {
        GUARD_LIST_LOCK;
        for (MIDIInputElementSysEx &e : elements)
            e.update();
    }

    /** 
     * @brief   Reset all MIDIInputElementSysEx elements to their initial state.
     * 
     * @see     MIDIInputElementSysEx#reset
     */
    static void resetAll() {
        GUARD_LIST_LOCK;
        for (MIDIInputElementSysEx &e : elements)
            e.reset();
    }

    /**
     * @brief   Update all MIDIInputElementSysEx elements with a new MIDI 
     *          message.
     * 
     * @see     MIDIInputElementSysEx#updateWith
     */
    static void updateAllWith(const ChannelMessageMatcher &midimsg) {
        for (MIDIInputElementSysEx &e : elements)
            if (e.updateWith(midimsg)) {
                e.moveDown();
                return;
            }
        // No mutex required:
        // e.moveDown may alter the list, but if it does, it always returns,
        // and we stop iterating, so it doesn't matter.
    }

  private:
    /**
     * @brief   Move down this element in the linked list of elements.
     * 
     * This means that the element will be checked earlier on the next
     * iteration.
     */
    void moveDown() {
        GUARD_LIST_LOCK;
        elements.moveDown(this);
    }

    static DoublyLinkedList<MIDIInputElementSysEx> elements;
#ifdef ESP32
    static std::mutex mutex;
#endif
};

#undef GUARD_LIST_LOCK

END_CS_NAMESPACE

#endif