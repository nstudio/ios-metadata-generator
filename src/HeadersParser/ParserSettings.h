#pragma once

#include <string>
#include <vector>

namespace HeadersParser {

    class ParserSettings {

    public:
        ParserSettings(std::string sysroot, std::string arch, std::string iPhoneOsVersionMin, std::string target,
                       std::string std, std::vector<std::string> headerSearchPaths, std::vector<std::string> frameworkSearchPaths)
                : _sysroot(sysroot),
                  _arch(arch),
                  _iPhoneOsVersionMin(iPhoneOsVersionMin),
                  _target(target),
                  _std(std),
                  _headerSearchPaths(headerSearchPaths),
                  _frameworkSearchPaths(frameworkSearchPaths) {}

        std::string getSysRoot() {
            return this->_sysroot;
        }

        std::string getArch() {
            return this->_arch;
        }

        std::string getIPhoneOsVersionMin() {
            return this->_iPhoneOsVersionMin;
        }

        std::string getTarget() {
            return this->_target;
        }

        std::string getStd() {
            return this->_std;
        }

        std::vector<std::string> getHeaderSearchPaths() {
            return this->_headerSearchPaths;
        }

        std::vector<std::string> getFrameworkSearchPaths() {
            return this->_frameworkSearchPaths;
        }

    private:
        std::string _sysroot;
        std::string _arch;
        std::string _iPhoneOsVersionMin;
        std::string _target;
        std::string _std;
        std::vector<std::string> _headerSearchPaths;
        std::vector<std::string> _frameworkSearchPaths;
    };
}