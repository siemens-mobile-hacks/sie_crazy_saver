# SieCrazySaver – Additional ELF Screensaver for Siemens Mobile Phones

SieCrazySaver is a native ELF screensaver for Siemens mobile phones. It can display the current track name from the media player and supports illumination.

## Building
The SDK must be located at `../sdk` relative to the project root.
```bash
mkdir build && cd build
cmake ..
make
```

## Dependencies
Before using SieCrazySaver, the following patches **must** be installed on your phone:

1. [Using ELFs screensavers](https://patches.kibab.com/patches/search.php5?action=search&kw=Using+ELFs+screensavers)
2. [Fade light off faster](https://patches.kibab.com/patches/search.php5?action=search&kw=Fade+light+off+faster)
3. [**NSG**] [Not to hide mediaplayer at keylock to keyboard after roll up XTask](https://patches.kibab.com/patches/search.php5?action=search&kw=Not+to+hide+mediaplayer+at+keylock+to+keyboard+after+roll+up+XTask)

Additionally, for the **PNG digital clock** you need:
- Copy `digits.png` and `digit_bg.png` from the `img` folder to `0:\zbin\img\SieCrazySaver\` on your phone.

## Screenshots
![Built-In digital clock](screenshots/built_in_digital_clock.png)
![PNG digital clock](screenshots/png_digital_clock.png)
![MP](screenshots/mp.png)
