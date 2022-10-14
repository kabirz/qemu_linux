#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <iostream>
#include <stdint.h>
#include <Command.h>
#include <Common.h>

using namespace std;

class SpiDevice {
public:
    static SpiDevice instance;
    SpiDevice(): fd(-1) {}
    int Open(const char* name) {
	if (fd >= 0) Close();
	fd = open(name, O_RDWR);
	return fd;
    };

    int Write(const uint8_t* tx_buf, uint32_t size) {
	if (fd < 0) return -1;
	return write(fd, tx_buf, size);
    }

    int Read(uint8_t* rx_buf, uint32_t size) {
	if (fd < 0) return -1;
	return read(fd, rx_buf, size);
    }

    int Xfer(const uint8_t* tx_buf, uint8_t* rx_buf, uint32_t size) {
	if (fd < 0) return -1;
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx_buf,
		.rx_buf = (unsigned long)rx_buf,
		.len = size,
	};
	return ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    }

    int SetMode(int mode) {
	if (fd < 0) return -1;
	return ioctl(fd, SPI_IOC_WR_MODE, &mode);
    }

    int GetMode() {
	if (fd < 0) return -1;
	int mode = 0;
	int ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret < 0)
		return ret;
	return mode;
    }

    int SetBitPerWord(int bit) {
	if (fd < 0) return -1;
	return ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bit);
    }

    int GetBitPerWord() {
	if (fd < 0) return -1;
	int bit;
	int ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bit);
	if (ret < 0)
		return ret;
	return bit;
    }

    int SetSpeed(int speed) {
	if (fd < 0) return -1;
	return ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    }

    int GetSpeed() {
	if (fd < 0) return -1;
	int speed;
	int ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret < 0)
		return ret;
	return speed;
    }

    int SetLsb(int lsb) {
	if (fd < 0) return -1;
	return ioctl(fd, SPI_IOC_WR_LSB_FIRST, &lsb);
    }

    int GetLsb() {
	if (fd < 0) return -1;
	int lsb;
	int ret = ioctl(fd, SPI_IOC_RD_LSB_FIRST, &lsb);
	if (ret < 0)
		return ret;
	return lsb;
    }

    int Close() {
	if(close(fd))
		return -1;
	else
		fd = -1;
	return 0;
    }
private:
    int fd;
};

constexpr char SpiName[] = "spi";
SpiDevice SpiDevice::instance;

class Spi: public RegisteredDevice<Spi, SpiName>
{
public:
    explicit Spi(Menu* menu)
    {
        auto subMenu = make_unique<Menu>(Name(), "spi operator");
	bool Opened = false;
        subMenu->Insert(
                "open", {"bus", "id"},
                [](ostream& out, int bus, int id) {
		    char name[64];
		    sprintf(name, "/dev/spidev%d.%d", bus, id);
                    int ret = SpiDevice::instance.Open(name);
		    if (ret < 0) {
		        out << "Device(" << name << ") open failed:";
		        perror("");
			out << endl;
		    } else {
		        out << "Device(" << name << ") open successfully" << endl;
		    }
		},
                "Open device node");
        subMenu->Insert(
                "close",
                [](std::ostream& out) {
                    int ret = SpiDevice::instance.Close();
		    if (ret)
			out << "Device close failed" << endl; 
                },
                "Close device node");
        subMenu->Insert(
                "write", {"data"},
                [](std::ostream& out, const string &data) {
		    vector<uint8_t> tx =  HexToBytes(data);
		    uint32_t len = tx.size();
		    DumpArray(out, tx.data(), tx.size(), "write");
		    int ret = SpiDevice::instance.Write(tx.data(), len);
		    if (ret < 0)
		        out << "spi write failed" << endl;

                },
                "spi write data");
        subMenu->Insert(
                "read", {"size"},
                [](std::ostream& out, uint32_t size) {
		    uint8_t* data = new uint8_t[size];
		    int ret = SpiDevice::instance.Read(data, size);
		    if (ret < 0) {
		        out << "spi read failed" << endl;
			delete[] data;
			return;
		    }
		    DumpArray(out, data, size, "read");
		    delete[] data;
                },
                "spi read data");
        subMenu->Insert(
                "xfer", {"data"},
                [](std::ostream& out, const string &data) {
		    vector<uint8_t> tx = HexToBytes(data);
		    uint8_t* rx_buf = new uint8_t[tx.size()];
		    int ret = SpiDevice::instance.Xfer(tx.data(), rx_buf, tx.size());
		    if (ret < 0) {
		        out << "spi xfer failed" << endl;
			delete[] rx_buf;
			return;
		    }
		    DumpArray(out, tx.data(), tx.size(), "tx");
		    DumpArray(out, rx_buf, tx.size(), "rx");
		    delete[] rx_buf;
                },
                "spi xfer data");
        subMenu->Insert(
                "setspeed", {"speed"},
                [](std::ostream& out, int speed) {
		    int ret = SpiDevice::instance.SetSpeed(speed);
		    if (ret < 0) {
		        out << "set speed failed" << endl;
		    }
                },
                "spi set speed");
        subMenu->Insert(
                "getspeed",
                [](std::ostream& out) {
		    int speed = SpiDevice::instance.GetSpeed();
		    if (speed < 0)
		        out << "get speed failed" << endl;
		    else
		        out << "speed is: " << speed << endl;
                },
                "spi get speed");
        subMenu->Insert(
                "setmode", {"mode"},
                [](std::ostream& out, uint32_t mode) {
		    if (mode > SPI_MODE_3) {
		        out << "mode should in (0, 1, 2, 3)" << endl;
			return;
		    }
		    int ret = SpiDevice::instance.SetMode(mode);
		    if (ret < 0) {
		        out << "set mode failed" << endl;
		    }
                },
                "spi set mode");
        subMenu->Insert(
                "getmode",
                [](std::ostream& out) {
		    int mode = SpiDevice::instance.GetMode();
		    if (mode < 0)
		        out << "get mode failed" << endl;
		    else
		        out << "mode is: " << mode << endl;
                },
                "spi get mode");
        subMenu->Insert(
                "setlsb", {"lsb"},
                [](std::ostream& out, uint32_t lsb) {
		    if (lsb > SPI_MODE_3) {
		        out << "lsb should in (0, 1, 2, 3)" << endl;
			return;
		    }
		    int ret = SpiDevice::instance.SetLsb(lsb);
		    if (ret < 0) {
		        out << "set lsb failed" << endl;
		    }
                },
                "spi set lsb");
        subMenu->Insert(
                "getlsb",
                [](std::ostream& out) {
		    int lsb = SpiDevice::instance.GetLsb();
		    if (lsb < 0)
		        out << "get lsb failed" << endl;
		    else
		        out << "lsb is: " << lsb << endl;
                },
                "spi get lsb");
        subMenu->Insert(
                "setbitperword", {"bit"},
                [](std::ostream& out, uint32_t bit) {
		    if (bit > SPI_MODE_3) {
		        out << "bit should in (0, 1, 2, 3)" << endl;
			return;
		    }
		    int ret = SpiDevice::instance.SetBitPerWord(bit);
		    if (ret < 0) {
		        out << "set bit failed" << endl;
		    }
                },
                "spi set bit per word");
        subMenu->Insert(
                "getbit",
                [](std::ostream& out) {
		    int bit = SpiDevice::instance.GetBitPerWord();
		    if (bit < 0)
		        out << "get bit failed" << endl;
		    else
		        out << "bit is: " << bit << endl;
                },
                "spi get bit per word");


        menuHandler = menu->Insert(move(subMenu));
    }
    ~Spi() override
    {
        SpiDevice::instance.Close();
        menuHandler.Remove();
    }
private:
    CmdHandler menuHandler;
    int fd;
};
