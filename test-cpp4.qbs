import qbs.base 1.0

CppApplication {
    consoleApplication: true
    Depends { name: "Qt.core" }
    cpp.cxxLanguageVersion: "c++11"
    files: ["*.cpp", "*.h"]
}
