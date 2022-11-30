# lynx-workshop-joystick

This is the code I use on the left hand joystick/mouse/keyboard from [Lynx Workshop](https://www.lynx-workshop.com/) I printed and built.

Many props to him for such a great idea and the wonderful design. I use this controller for most of my gaming sessions now!

On the hardware level, the only thing I changed was adding simple diodes on the matrix to eliminate ghosting and support pressing multiple keys simultaneously.

I started from the original template from [Lynx Workshop](https://5f2fc3fa-16c8-4939-878d-c368ff270bdb.filesusr.com/archives/67f963_722028bf48664c0d82f5bbe289d8461b.zip?dn=cat_joystick_lefthand_version_001.zip), but I suppose there is not much left from it. However I reused most of the logic from the excellent [Arduino Keypad library](https://github.com/Chris--A/Keypad).

The 3 buttons under the pinky finger are dedicated to switching modes. One click on one of these buttons select the related emulation mode (Keyboard, Joystick, Mouse) or switch key mapping if this mode is already active.

TODO / WIP:
- [X] Keyboard emulation
- [ ] Joystick emulation
- [ ] Mouse emulation
- [ ] Add assembly pictures (showing diodes and my poor soldering skills)
