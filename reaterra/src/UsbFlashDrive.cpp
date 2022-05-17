#include "UsbFlashDrive.h"

#include <sys/stat.h> 		// stat, mkdir, S_ISDIR
#include <sys/statfs.h>
#include <unistd.h>			// unlink close

#include <regex>
#include <thread>

#include "Logger.h"


void UsbFlashDrive::fileCopy(std::string src, std::string dst) {
//	driveMount();

	if (src.find("/media/usb") != std::string::npos) {
		src = std::regex_replace(src, std::regex("/media/usb"), m_path);
	}
	if (dst.find("/media/usb") != std::string::npos) {
		dst = std::regex_replace(dst, std::regex("/media/usb"), m_path);
	}

	int ret = unlink(dst.c_str());
	if (ret && errno != ENOENT) {
		LE("Destination file " + dst + " already existed and can't be rewritten");
		return;
	}

	{
		std::ifstream  src_file(src, std::ios::binary);
		std::ofstream  dst_file(dst, std::ios::binary);
		dst_file << src_file.rdbuf();
	}

//	driveUmount();
}

unsigned long int UsbFlashDrive::getFreeSpace() {
	driveMount();

	struct statfs fsb;

	unsigned long size = 0;
	if(statfs(m_path.c_str(), &fsb) == 0) {
		size = fsb.f_bfree * fsb.f_bsize;
		LD("Free space: " + std::to_string(size));
	} else {
		LE("Error! Can't get free space of flash drive");
	}

	driveUmount();
	return size;
}

UsbFlashDrive::UsbFlashDrive(std::string dev_node)
:	m_valid {true},
	m_dev_node {dev_node},
	m_path {"/media/usb"}
{
	if (m_dev_node == "/dev/sda1") {
		m_path = "/media/usb1";
	}
	LD("Ctor");
}

UsbFlashDrive::~UsbFlashDrive() {
	LD("Dtor");
}

bool UsbFlashDrive::isValid() const {
	return m_valid;
}

void UsbFlashDrive::driveMount() {
	if (!isValid()) {
		LE("Usb drive is not ready");
	}

	int ret;
	for (int j = 0; j < 2; ++j) {
		for (int i = 0; i < 10; ++i) {
			ret = system(("mount " + m_dev_node).c_str());
			LD("Mount /media/usb" + m_dev_node + "  ret: " + std::to_string(ret));
			if (!ret || ret == 8192) {
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
		if (ret) {
			LE("Mount error /media/usb :" + m_dev_node);
			driveUmount();
		} else {
			break;
		}
	}
}

void UsbFlashDrive::driveUmount() {
	int ret = system(("umount " + m_dev_node).c_str());
	LD("Umount ret: " + std::to_string(ret));
	if (ret) {
		LE("Unmount error /media/usb :" + m_dev_node);
	}
}

bool UsbFlashDrive::isFileExisted(std::string name) {
	driveMount();
	struct stat buffer;
	std::string path = m_path + "/" + name;
	bool ret = stat(path.c_str(), &buffer) == 0;
	driveUmount();
	return ret;
}
