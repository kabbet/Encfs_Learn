// defines needed for RedHat 7.3...
#ifdef __linux__
#define _XOPEN_SOURCE 500  // make sure pwrite() is pulled in
#endif
#define _BSD_SOURCE      // pick up setenv on RH7.3
#define _DEFAULT_SOURCE  // Replaces _BSD_SOURCE

#include "easylogging++.h"
#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <list>
#ifdef __APPLE__
#include <sys/mount.h>
#include <sys/param.h>
#endif
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <tinyxml2.h>
#include <unistd.h>
#include <vector>

#include "BlockNameIO.h"
#include "Cipher.h"
#include "CipherKey.h"
#include "ConfigReader.h"
#include "ConfigVar.h"
#include "Context.h"
#include "DirNode.h"
#include "Error.h"
#include "FSConfig.h"
#include "FileUtils.h"
#include "Interface.h"
#include "NameIO.h"
#include "Range.h"
#include "XmlReader.h"
#include "autosprintf.h"
#include "base64.h"
#include "config.h"
#include "i18n.h"
#include "intl/gettext.h"
#include "readpassphrase.h"

using namespace std;
using gnu::autosprintf;

namespace encfs {

static const int DefaultBlockSize = 1024;
// The maximum length of text passwords.  If longer are needed,
// use the extpass option, as extpass can return arbitrary length binary data.
static const int MaxPassBuf = 512;

static const int NormalKDFDuration = 500;     // 1/2 a second
static const int ParanoiaKDFDuration = 3000;  // 3 seconds

// environment variable names for values encfs stores in the environment when
// calling an external password program.
static const char ENCFS_ENV_ROOTDIR[] = "encfs_root";
static const char ENCFS_ENV_STDOUT[] = "encfs_stdout";
static const char ENCFS_ENV_STDERR[] = "encfs_stderr";

// static int V5SubVersion = 20040518;
// static int V5SubVersion = 20040621; // add external IV chaining
static int V5SubVersion = 20040813;  // fix MACFileIO block size issues
static int V5SubVersionDefault = 0;

// 20080813 was really made on 20080413 -- typo on date..
// const int V6SubVersion = 20080813; // switch to v6/XML, add allowHoles option
// const int V6SubVersion = 20080816; // add salt and iteration count
/*
 * In boost 1.42+, serial numbers change to 8 bit, which means the date
 * numbering scheme does not work any longer.
 * boost-versioning.h implements a workaround that sets the version to
 * 20 for boost 1.42+. */
const int V6SubVersion = 20100713;  // add version field for boost 1.42+

struct ConfigInfo {
  const char *fileName;
  ConfigType type;
  const char *environmentOverride;
  bool (*loadFunc)(const char *fileName, EncFSConfig *config, ConfigInfo *cfg);
  bool (*saveFunc)(const char *fileName, const EncFSConfig *config);
  int currentSubVersion;
  int defaultSubVersion;
} ConfigFileMapping[] = {
    // current format
    {".encfs6.xml", Config_V6, "ENCFS6_CONFIG", readV6Config, writeV6Config,
     V6SubVersion, 0},
    // backward compatible support for older versions
    {".encfs5", Config_V5, "ENCFS5_CONFIG", readV5Config, writeV5Config,
     V5SubVersion, V5SubVersionDefault},
    {".encfs4", Config_V4, nullptr, readV4Config, writeV4Config, 0, 0},
    // no longer support earlier versions
    {".encfs3", Config_V3, nullptr, nullptr, nullptr, 0, 0},
    {".encfs2", Config_Prehistoric, nullptr, nullptr, nullptr, 0, 0},
    {".encfs", Config_Prehistoric, nullptr, nullptr, nullptr, 0, 0},
    {nullptr, Config_None, nullptr, nullptr, nullptr, 0, 0}};

EncFS_Root::EncFS_Root() = default;

EncFS_Root::~EncFS_Root() = default;

bool fileExists(const char *fileName) {
  struct stat buf;
  return lstat(fileName, &buf) == 0;
}

bool fileExists(const char* fileName) {
  struct stat buf;
  return lstat(fileName, &buf) == 0;
}

bool isDirectory(const char* fileName) {
  struct stat buf;
  if (lstat(fileName, &buf) == 0) {
    return S_ISDIR(buf.st_mode);
  }
  return false;
}

bool isAbsolutePath(const char* fileName) {
  return (fileName != nullptr) && fileName[0] != '\0' && fileName[0] == '/';
}

const char* lastPathElement(const char* name) {
  const char* loc = strrchr(name, '/');
  return loc != nullptr ? loc + 1 : name;
}

std::string parentDirectory(const std::string& path) {
  size_t last = path.find_last_of('/');
  if (last == string::npos) {
    return string("");
  }
  return path.substr(0, last);
}

bool userAllowMkdir(const char* path, mode_t mode) {
  return userAllowMkdir(0, path, mode);
}

bool userAllowMkdir(int prompto, const char* path, mode_t mode) {
  // TODO: can we internationalize the y/n names ? Seems strange to prompt in
  // their own language but then have to respond 'y' or 'n'
  // xgroup(setup)
  cerr << autosprintf(
    _("The directory \"%s\" does not exist. Should it be created? "
      "(y, N) "),
    path
  );
  char answer[10];

  char* res;
  switch (promptno) {
    case 1:
      cerr << endl << "$PROMPT$ create_root_dir" << endl;
    case 2:
      cerr << endl << "$pROMPT$ create_mount_point" << endl;
  }
}

}  // namespace encfs
