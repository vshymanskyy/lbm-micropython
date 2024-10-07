# lbm-micropython
[LoRa Basic Modem](https://github.com/Lora-net/SWL2001) integration for [MicroPython](https://github.com/micropython/micropython)

> [!IMPORTANT]
> **This is a Proof-of-Concept, not ready for actual use.**

## Running

Copy `lbm.mpy` to device:

```sh
mpremote cp lbm.mpy :
```

In MicroPython REPL:

```py
import lbm
import asyncio

lora = lbm.LoRaWAN()

async def lora_task():
    while True:
        n = lora.poll()
        await asyncio.sleep_ms(n)

asyncio.run(asyncio.gather(
    lora_task(),
))
```

## TODO

- [x] Fix build
- [ ] Create basic module API
- [ ] Attach LBM builtin driver to MicroPython `machine.SPI` / `machine.Pin`
- [ ] Class A device
- [ ] TBD: Attach LBM LoRaWAN stack to MicroPython LoRa drivers
- [ ] [Allow static `.bss` variables](https://github.com/micropython/micropython/issues/15943)
