# Building `lbm-micropython`

```sh
# Install tools
mkdir -p tools
wget -qO- https://github.com/earlephilhower/esp-quick-toolchain/releases/download/3.2.0-gcc10.3/x86_64-linux-gnu.xtensa-lx106-elf-c791b74.230224.tar.gz | tar xz -C ./tools
wget -qO- https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz | tar xz -C ./tools
wget -qO- https://github.com/vshymanskyy/micropython/archive/refs/heads/mpy-ld-link-archives.tar.gz | tar xz -C ./tools

sudo apt update
sudo apt install build-essential gcc-multilib gcc-arm-none-eabi

pip3 install -U requests pyelftools ar tabulate

# Prepare environment
export PATH=$PWD/tools/xtensa-lx106-elf/bin:$PATH
export PATH=$PWD/tools/xtensa-esp32-elf/bin:$PATH
export MPY_DIR=$PWD/tools/micropython-mpy-ld-link-archives

# Build
make ARCH=xtensawin
```

As a result, `lbm.mpy` should be produced.

> [!WARNING]
> **This is WIP and not runnable at the moment**
