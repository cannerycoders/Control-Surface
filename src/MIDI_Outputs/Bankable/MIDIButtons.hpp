#pragma once

#include <Banks/BankableMIDIOutputAddressable.hpp>
#include <Hardware/Button.h>
#include <MIDI_Outputs/AbstractMIDIOutput.hpp>
#include <Helpers/ArrayWrapper.hpp>

namespace BankableNS {

/**
 * @brief   An abstract class for momentary push buttons that send MIDI events.
 * 
 * The buttons are debounced.
 * 
 * @see     Button
 */
template <uint8_t NUMBER_OF_BUTTONS, class Sender>
class MIDIButtons : public BankableMIDIOutputAddressable,
                    public AbstractMIDIOutput {
  public:
    /**
     * @brief   Construct a new MIDIButtons_Bankable.
     * 
     * @todo    Documentation
     */
    MIDIButtons(const ArrayWrapper<Button, NUMBER_OF_BUTTONS> &buttons,
                uint8_t baseAddress, uint8_t baseChannel,
                uint8_t addressIncrement, uint8_t channelIncrement)
        : buttons{buttons}, baseAddress(baseAddress), baseChannel(baseChannel),
          increment(((channelIncrement & 0xF) << 4) |
                    (addressIncrement & 0xF)) {}

    void update() final override {
        uint8_t channel = getChannel(baseChannel);
        uint8_t address = getAddress(baseAddress);
        uint8_t channelIncrement = getChannelIncrement();
        uint8_t addressIncrement = getAddressIncrement();
        for (Button &button : buttons) {
            Button::State state = button.getState();
            if (state == Button::Falling) {
                if (!activeButtons)
                    lock(); // Don't allow changing of the bank setting
                activeButtons++;
                Sender::sendOn(channel, address);
            } else if (state == Button::Rising) {
                Sender::sendOff(channel, address);
                activeButtons--;
                if (!activeButtons)
                    unlock();
            }
            channel += channelIncrement;
            address += addressIncrement;
        }
    }

    uint8_t getChannelIncrement() const { return increment >> 4; }
    uint8_t getAddressIncrement() const { return increment & 0xF; }

  private:
    ArrayWrapper<Button, NUMBER_OF_BUTTONS> buttons;
    const uint8_t baseAddress;
    const uint8_t baseChannel;
    const uint8_t increment;
    uint8_t activeButtons = 0;
};

} // namespace BankableNS