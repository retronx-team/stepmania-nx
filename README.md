StepMania Switch Port
==================

<p align="center"><img src="icon.jpg"></p>

## Usage

### Grabbing Binaries

Download the latest release,
and extract the archive in the `/switch` folder on your SD Card.
Then, run the game from the [hbmenu](https://github.com/switchbrew/nx-hbmenu)
using [hbl](https://github.com/switchbrew/nx-hbloader).

### Default controls

|  ACTION   | Key 1  | Key 2 |
|:---------:|:------:|:-----:|
|**Left**   | DPAD L | Y     |
|**Right**  | DPAD R | A     |
|**Up**     | DPAD U | X     |
|**Down**   | DPAD D | B     |
|**UpLeft** | L      |       |
|**UpRight**| R      |       |
|**Back**   | Minus  |       |
|**Start**  | Plus   |       |
|**Select** | ZL     |       |
|**Coin**   | ZR     |       |

## Development

### Build dependencies

For building, you need the devkitA64 from devkitPro setup, along with switch portlibs and libnx.
Documentation to setup that can be found [here](https://switchbrew.org/wiki/Setting_up_Development_Environment).

**Note:** Release uses a customised FFMPEG [build](https://gist.github.com/p-sam/30b4b535f055e1f9b0eab2dd1ce89fd4)
from the [PPSSPP repo](https://github.com/hrydgard/ppsspp-ffmpeg) to allow a few more commonly used codecs and performance.

### Compiling from source

Run one of the following commands from the project root to build:

```
# Release
./build.sh

# Debug
./build.sh "debug"
```
