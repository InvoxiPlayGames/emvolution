# Emvolution

A work-in-progress Wii game file replacement engine written for the BrainSlug game patching utility.

Tested and confirmed to be working on Mario Kart Wii (RMCP, RMCE, RMCJ).

## TODO

- Improve symbols in `dvd.xml`
- Add DVDReadAsyncPrio support
- Allow for custom end user specified files (e.g. "My Stuff" folder)
- Add support for additional DVD functions (such as directories), may be required for some games
- Add USB support (requires creation of a libfat-usb driver for BrainSlug)

## For end users:

Right now, there is no way for an end user to take advantage of Emvolution unless it is included with a distribution with everything prepared. In the future, there may be a version allowing you to use a folder on your SD card out of the box from a release download.

## For developers: How to use

1. [Install the latest devkitPPC and libogc.](https://devkitpro.org/wiki/Getting_Started)
2. Download the latest [BrainSlug](https://github.com/Chadderz121/brainslug-wii) source code and run `make install` to install the required files.
3. Edit `replacements.h` - simply add your replacements in the form of `{ "/file/to/replace", "sd:/replacement/file" }` into the replacements array. For example:
    ```c
    const char* replacements[][2] = {
        { "/Boot/Strap/eu/English.szs", "sd:/custom/Strap.szs" },
        { "/Scene/UI/MenuSingle.szs", "sd:/custom/MenuSingle.szs" },
        { "/Scene/UI/MenuSingle_E.szs", "sd:/custom/MenuSingle_E.szs" },
        { "/rel/lecode-PAL.bin", "sd:/custom/lecode-PAL.bin" },
    };
    ```
4. Run `make` inside the Emvolution project directory.
5. Load the resulting `emvolution.mod` module with your BrainSlug channel.
    - Make sure you have the `dvd.xml` file loaded as a symbol as well as the default BrainSlug symbols, and make sure that you have `libfat.mod`, `libsd.mod` and `libfat-sd.mod` loaded too.

**Note:** Unlike Riivolution, you can not replace the main executable (main.dol) of a game using this method. Please use a BrainSlug module to apply the necessary patches to the game executable. Files dynamically linked into the game (.rel) can still be replaced, however it's advised to also use a BrainSlug module to patch these files upon OSLink to avoid redistributing game content, and to keep downloads smaller.