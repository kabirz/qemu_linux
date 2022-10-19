import time
import random
import sys

address = [0x00, 0x00, 0x00]
erase_cmd = [0x20, *address]
write_cmd = [0x02, *address]
read_cmd = [0x03, *address]


def wait(spi, xfer):
    while True:
        rx = xfer([0x05, 0x00])
        if rx[1] & 1 == 0:
            break
        time.sleep(0.5)


def spi_write(spi, data, xfer):
    # enable write
    xfer([0x06])
    # erase
    xfer(erase_cmd)
    wait(spi, xfer)

    # enable write
    xfer([0x06])
    # write
    print("tx data: ", list(map(hex, data)))
    tx = [*write_cmd, *data]
    xfer(tx)
    wait(spi, xfer)


def spi_read(spi, xfer):
    # read
    tx_w = [*read_cmd, *data]
    rx = xfer(tx_w)
    print("rx data: ", list(map(hex, rx[4:])))


if __name__ == '__main__':
    # setting
    if len(sys.argv) >= 2 and sys.argv[1] == 'spidev':
        # spidev
        print("use spidev")
        import spidev
        spi = spidev.SpiDev()
        spi.open(0, 0)
        spi.max_speed_hz = 50000
        xfer = spi.xfer2
    else:
        # periphery
        print("use periphery.spi")
        from periphery import SPI
        spi = SPI("/dev/spidev0.0", 0, 50000)
        xfer = spi.transfer

    data = random.sample(range(0, 255), 12)
    if len(sys.argv) >= 2:
        if sys.argv[-1] == 'w':
            spi_write(spi, data, xfer)
        elif sys.argv[-1] == 'r':
            spi_read(spi, xfer)
        else:
            spi_write(spi, data, xfer)
            spi_read(spi, xfer)
    else:
        spi_write(spi, data, xfer)
        spi_read(spi, xfer)

    spi.close()
