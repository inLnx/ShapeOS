//
// Created by KrisnaPranav on 21/01/22.
//

#include "virtualfilesystem.h"
#include <kernel/log.h>
#include <ak/string.h>

using namespace Kernel;
using namespace String;

virtualFileSystem::virtualFileSystem(Disk* disk, uint32_t start, uint32_t size, char* name) {
    this->disk = disk;
    this->sizeInSectors = size;
    this->startLBA = start;
    this->Name = name;
}

virtualFileSystem::~virtualFileSystem() {
    sendLog(Error, "Virtual function called directly %s:%d", __FILE__, __LINE__);
}

bool virtualFileSystem::initialize() {
    return false;
}

int virtualFileSystem::readFile(const char* filename, uint8_t* buffer, uint32_t offset, uint32_t len) {
    sendLog(Error, "Virtual function called directly %s:%d", __FILE__, __LINE__);
    return -1;
}

int virtualFileSystem::writeFile(const char* filename, uint8_t* buffer, uint32_t len, bool create) {
    sendLog(Error, "Virtual function called directly %s:%d", __FILE__, __LINE__);
    return -1;
}

bool virtualFileSystem::fileExists(const char* filename) {
    sendLog(Error, "Virtual function called directly %s:%d", __FILE__, __LINE__);
    return false;
}

bool virtualFileSystem::directoryExists(const char* filename) {
    sendLog(Error, "Virtual function called directly %s:%d", __FILE__, __LINE__);
    return false;
}

int virtualFileSystem::createFile(const char* path) {
    sendLog(Error, "Virtual function called directly %s:%d", __FILE__, __LINE__);
    return -1;
}

int virtualFileSystem::createDirectory(const char* path) {
    sendLog(Error, "Virtual function called directly %s:%d", __FILE__, __LINE__);
    return -1;
}

uint32_t virtualFileSystem::getFileSize(const char* filename) {
    sendLog(Error, "Virtual function called directly %s:%d", __FILE__, __LINE__);
    return -1;
}

List<LibC::vfsEntry>* virtualFileSystem::directoryList(const char* path) {
    sendLog(Error, "Virtual function called directly %s:%d", __FILE__, __LINE__);
    return 0;
}