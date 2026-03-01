cmake_minimum_required (VERSION 3.14)

project (embedded-device-core
    VERSION 1.0.0 
    LANGUAGES C
)

find_package(PkgConfig REQUIRED)

pkg_check_modules(BLKID REQUIRED IMPORTED_TARGET blkid)

subdirs (src)
