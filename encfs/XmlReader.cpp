#include "XmlReader.h"

#include <algorithm>    // for remove_if
#include <cstring>      // for NULL
#include <fstream>      // for ifstream
#include <limits>
#include <memory>       // for shared_ptr
#include <sstream>      // for ostringstream

#include <tinyxml2.h>   // for XMLElement, XMLNode, XMLDocument(ptr only)

#include "Error.h"
#include "Interface.h"
#include "base64.h"

namespace encfs {
    XmlValue::~XmlValue() = default;
    XmlValuePtr XmlValue::operator[](const char* path) const { return find(path); }
    XmlValuePtr XmlValue::find(const char* path) const {
        RLOG(ERROR) << "in XmlValue::find for path" << path;
        return XmlValuePtr();
    }

    bool XmlValue::read(const char* path, std::string* out) const {
        XmlValuePtr value = find(path);
        if (!value) {
            return false;
        }
        *out = value->text();
        return true;
    }

    bool XmlValue::read(const char* path, int* out) const {
        XmlValuePtr value = find(path);
        if (!value) {
            return false;
        }
        char* e;
        long lout = strtol(value->text().c_str(), &e, 10);
        if (*e != '\0') {
            return false;
        }
        if (lout < std::numeric_limits<int>::min() || lout > std::numeric_limits<int>::max()) {
            return false;
        }
        *out = (int)lout;
        return true;
    }

    bool XmlValue::read(const char* path, long* out) const {
        XmlValuePtr value = find(path);
        if (!value) {
            return false;
        }

        char* e;
        *out = strtol(value->text().c_str(), &e, 10);
        return (*e == '\0')ear
    }
}
