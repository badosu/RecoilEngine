+++
title = 'Default Ui Keys Reference'
date = 2025-05-10T01:14:58-07:00
draft = false
+++

Quick Notes:

1. The built-in default bindings are always loaded. If you wish to override them, use the unbindall and unbind commands to delete them at the beginning of your 'uikeys.txt'.
2. More then one action can be bound to a specific keyset. The actions for any given keyset are tried in the order that they were bound. The first currently available command that matches an action is used.
3. As a slight modification to the above note, keysets that use the 'Any' modifier are tried after those that do not use it.
4. A keyset using the Shift modifier should be bound with its unshifted key  (ex: bind  "Shift+."  instead of  "Shift+\>" )


Commands that can be used in this file:

unbindall
- removes all bindings, and adds "bind enter chat"
    (one of the other unbind commands can get rid of that one)
- good for deleting the default bindings

keysym \<name\> \<keycode\>
- add a custom key symbol  (the default key symbols can not be overridden)
- name must start with a letter, and only contain letters, numbers, and '_'
- the \<keycode\> can be a currently recognized keysym
    (ex:  "keysym menu 0x13F"  or  "keysym radar r")

fakemeta \<keysym\>
- assign an auxiliary key for the Meta modifier ("space" is a good choice)
- use "fakemeta none" to disable this feature

bind \<keyset\> \<action\>
- appends the action to the keyset's list of actions
- the action can be just a command, or a command with arguments

unbind \<keyset\> \<action\>
- removes the action from the keyset's list of actions
- both the action and the keyset must match

unbindkeyset \<keyset\>
- removes all bindings that use the keyset

unbindaction \<action\>
- removes all bindings that use the action  (command that is, a misnomer)


* NOTE: These commands can also be run from the chat line in-game using
        the slash command syntax  (/bind, /unbind, etc...)




Keyset Format:

A keyset is a combination of keys, the main key and its modifiers.

Here are some examples:

    bind            a  fake_action
    bind       Ctrl+a  fake_action
    bind          C+a  fake_action
    bind Ctrl+Shift+a  fake_action
    bind          *+a  fake_action

The format then goes like this:

    [\<Modifier\>+]...[\<Modifier\>+]\<keysym\>


The modifiers (and their abbreviations), are:

    Any   (*)
    Alt   (A)
    Ctrl  (C)
    Meta  (M)
    Shift (S)

The special 'Any' modifier makes it so that the keyset matches
regardless of the current state of the real modifiers.

The known keysyms (key symbols), are listed at the end of this file.
If you want to use a key that is unknown to Spring, then you may use
the hexadecimal notation. Here are two equivalent bindings:

    bind Ctrl+0x20  firestate 0   hold fire
    bind Ctrl+space firestate 0   hold fire



Keychains (new in 98.0):

A keychain is a timed combination of keysets,
i.e. "press A, then B".

Here are some examples:

    bind            a,b  fake_action
    bind Ctrl+a, Ctrl+b  fake_action

The format then goes like this:

    \<keyset\>, \<keyset\>, ...




Extra Run-Time Commands

/keyload   : loads the uikeys.txt bindings  (does not clear current bindings)
/keyreload : loads the uikeys.txt bindings  (clears current bindings first)
/keysave   : save current bindings to 'uikeys.tmp'  (NOTE: 'tmp' vs. 'txt')
/keysyms   : prints the known keysyms to standard out
/keycodes  : prints the known keycodes to standard out
/keyprint  : prints the current bindings to standard out
/keydebug  : prints debugging information to standard out (for each keystroke)




Default Bindings:
see rts/Game/UI/KeyBindings.cpp

| Key Symbol    | Key Code |
|---------------|----------|
| !             | 0x021    |
| "             | 0x022    |
| #             | 0x023    |
| $             | 0x024    |
| %             | 0x025    |
| &             | 0x026    |
| '             | 0x027    |
| (             | 0x028    |
| )             | 0x029    |
| *             | 0x02A    |
| +             | 0x02B    |
| ,             | 0x02C    |
| -             | 0x02D    |
| .             | 0x02E    |
| /             | 0x02F    |
| 0             | 0x030    |
| 1             | 0x031    |
| 2             | 0x032    |
| 3             | 0x033    |
| 4             | 0x034    |
| 5             | 0x035    |
| 6             | 0x036    |
| 7             | 0x037    |
| 8             | 0x038    |
| 9             | 0x039    |
| :             | 0x03A    |
| ;             | 0x03B    |
| <             | 0x03C    |
| =             | 0x03D    |
| >             | 0x03E    |
| ?             | 0x03F    |
| @             | 0x040    |
| [             | 0x05B    |
| \             | 0x05C    |
| ]             | 0x05D    |
| ^             | 0x05E    |
| _             | 0x05F    |
| `             | 0x060    |
| a             | 0x061    |
| alt           | 0x134    |
| b             | 0x062    |
| backspace     | 0x008    |
| c             | 0x063    |
| clear         | 0x00C    |
| ctrl          | 0x132    |
| d             | 0x064    |
| delete        | 0x07F    |
| down          | 0x112    |
| e             | 0x065    |
| end           | 0x117    |
| enter         | 0x00D    |
| esc           | 0x01B    |
| escape        | 0x01B    |
| f             | 0x066    |
| f1            | 0x11A    |
| f10           | 0x123    |
| f11           | 0x124    |
| f12           | 0x125    |
| f13           | 0x126    |
| f14           | 0x127    |
| f15           | 0x128    |
| f2            | 0x11B    |
| f3            | 0x11C    |
| f4            | 0x11D    |
| f5            | 0x11E    |
| f6            | 0x11F    |
| f7            | 0x120    |
| f8            | 0x121    |
| f9            | 0x122    |
| g             | 0x067    |
| h             | 0x068    |
| home          | 0x116    |
| i             | 0x069    |
| insert        | 0x115    |
| j             | 0x06A    |
| joy0          | 0x12C    |
| joy1          | 0x12D    |
| joy2          | 0x12E    |
| joy3          | 0x12F    |
| joy4          | 0x130    |
| joy5          | 0x131    |
| joy6          | 0x132    |
| joy7          | 0x133    |
| joydown       | 0x141    |
| joyleft       | 0x142    |
| joyright      | 0x143    |
| joyup         | 0x140    |
| joyw          | 0x193    |
| joyx          | 0x190    |
| joyy          | 0x191    |
| joyz          | 0x192    |
| k             | 0x06B    |
| l             | 0x06C    |
| left          | 0x114    |
| m             | 0x06D    |
| meta          | 0x136    |
| n             | 0x06E    |
| numpad*       | 0x10C    |
| numpad+       | 0x10E    |
| numpad-       | 0x10D    |
| numpad.       | 0x10A    |
| numpad/       | 0x10B    |
| numpad0       | 0x100    |
| numpad1       | 0x101    |
| numpad2       | 0x102    |
| numpad3       | 0x103    |
| numpad4       | 0x104    |
| numpad5       | 0x105    |
| numpad6       | 0x106    |
| numpad7       | 0x107    |
| numpad8       | 0x108    |
| numpad9       | 0x109    |
| numpad=       | 0x110    |
| numpad_enter  | 0x10F    |
| o             | 0x06F    |
| p             | 0x070    |
| pagedown      | 0x119    |
| pageup        | 0x118    |
| pause         | 0x013    |
| printscreen   | 0x13C    |
| q             | 0x071    |
| r             | 0x072    |
| return        | 0x00D    |
| right         | 0x113    |
| s             | 0x073    |
| shift         | 0x130    |
| space         | 0x020    |
| t             | 0x074    |
| tab           | 0x009    |
| u             | 0x075    |
| up            | 0x111    |
| v             | 0x076    |
| w             | 0x077    |
| x             | 0x078    |
| y             | 0x079    |
| z             | 0x07A    |
| {             | 0x07B    |
| &#124;        | 0x07C    |
| }             | 0x07D    |
| ~             | 0x07E    |
