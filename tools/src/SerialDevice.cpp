#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termio.h>
#include <sys/ioctl.h>
#include <iostream>
#include <stdint.h>
#include <Command.h>
#include <Common.h>
#include <map>

using namespace std;

map<uint32_t, uint32_t> bauds = {
	{57600,   B57600},
	{115200,  B115200},
	{230400,  B230400},
	{460800,  B460800},
	{500000,  B500000},
	{576000,  B576000},
	{921600,  B921600},
	{1000000, B1000000},
	{1152000, B1152000},
	{1500000, B1500000},
	{2000000, B2000000},
	{2500000, B2500000},
	{3000000, B3000000},
	{3500000, B3500000},
	{4000000, B4000000},
};

map<uint32_t, uint32_t> databits = {
	{5, CS5},
	{6, CS6},
	{7, CS7},
	{8, CS8},
};

map<char, uint32_t> paritys = {
	{'o', PARODD},
	{'e', 0},
	{'m', CMSPAR | PARODD},
	{'s', CMSPAR},
};

map<char, uint32_t> flows = {
	{'n', 0},
	{'h', 1},
	{'s', 2},
	{'m', 3},
};

class SerialDevice {
public:
    static SerialDevice instance;
    SerialDevice(): fd(-1) {}
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

    int SetConfig(uint32_t baud, uint32_t dataBit, uint32_t stopBit, char parity, char flow) {
	if (fd < 0) return -1;
	struct termios term;
	uint32_t _baud = bauds[baud];
	uint32_t _dataBit = databits[dataBit];
	uint32_t _stopBit = stopBit == 1 ? 0 : CSTOPB;
	uint32_t _parity = paritys[parity];
	uint32_t parity_stat = parity == 'n' ? 0 : PARENB;

	if (_baud == 0) _baud = B115200;
	if (_dataBit == 0) _dataBit = CS8;

	term.c_cflag = _baud | _dataBit | _stopBit | parity_stat | _parity | CLOCAL | CREAD;
	if ((flow == 'h') || (flow == 'm'))
		term.c_cflag |= CRTSCTS;
	if (flow == 's')
		term.c_iflag = INPCK | IXOFF;
	else
		term.c_iflag = INPCK;
	term.c_oflag = 0;
	term.c_lflag = 0;
	term.c_cc[VMIN]=1;
	term.c_cc[VTIME]=0;
	tcflush(fd, TCIFLUSH);
	return tcsetattr(fd,TCSANOW, &term);
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

constexpr char SerialName[] = "serial";
SerialDevice SerialDevice::instance;

class Serial: public RegisteredDevice<Serial, SerialName>
{
public:
    explicit Serial(Menu* menu)
    {
        auto subMenu = make_unique<Menu>(Name(), "serial operator");
	bool Opened = false;
        subMenu->Insert(
                "open", {"device_node"},
                [](ostream& out, const string& name) {
                    int ret = SerialDevice::instance.Open(name.c_str());
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
                    int ret = SerialDevice::instance.Close();
		    if (ret)
			out << "Device close failed" << endl; 
                },
                "Close device node");
        subMenu->Insert(
                "config", {"baud[57600/115200..]", "dataBit[5/6/7/8]", "stopBit[1/2]", "parity[o/e/m/s]", "flow[n/h/s/m]"},
                [](std::ostream& out, uint32_t baud, uint32_t dataBit, uint32_t stopBit, char parity, char flow) {
                    int ret = SerialDevice::instance.SetConfig(baud, dataBit, stopBit, parity, flow);
		    if (ret)
			out << "serial config error!" << endl; 
                },
                "config serial");
        subMenu->Insert(
                "write", {"data"},
                [](std::ostream& out, const string &data) {
		    vector<uint8_t> tx =  HexToBytes(data);
		    uint32_t len = tx.size();
		    DumpArray(out, tx.data(), tx.size(), "write");
		    int ret = SerialDevice::instance.Write(tx.data(), len);
		    if (ret < 0)
		        out << "serial write failed" << endl;

                },
                "serial write data");
        subMenu->Insert(
                "read", {"size"},
                [](std::ostream& out, uint32_t size) {
		    uint8_t* data = new uint8_t[size];
		    int ret = SerialDevice::instance.Read(data, size);
		    if (ret < 0) {
		        out << "serial read failed" << endl;
			delete[] data;
			return;
		    }
		    DumpArray(out, data, size, "read");
		    delete[] data;
                },
                "serial read data");
        subMenu->Insert(
                "write_str", {"data"},
                [](std::ostream& out, const string &data) {
		    int ret = SerialDevice::instance.Write((uint8_t *)data.c_str(), data.size());
		    if (ret < 0)
		        out << "serial write failed" << endl;
		cout << "Write: " << data << endl;
                },
                "serial write string");
        subMenu->Insert(
                "read_str", {"size"},
                [](std::ostream& out, uint32_t size) {
		    char* data = new char[size+1];
		    memset(data, 0, size + 1);
		    int ret = SerialDevice::instance.Read((uint8_t *)data, size);
		    if (ret < 0) {
		        out << "serial read failed" << endl;
			delete[] data;
			return;
		    }
		    cout << "Read: " << data << endl;
		    delete[] data;
                },
                "serial read string");


        menuHandler = menu->Insert(move(subMenu));
    }
    ~Serial() override
    {
        SerialDevice::instance.Close();
        menuHandler.Remove();
    }
private:
    CmdHandler menuHandler;
    int fd;
};
