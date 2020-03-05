#!/bin/sh
echo '"Use -m32 switch to force 32-bit build"'
${CXX:-g++} $* -std=c++17 -Wall -DNDEBUG -O2 -o bin/lists src/date.cpp src/issues.cpp src/status.cpp src/sections.cpp src/mailing_info.cpp src/report_generator.cpp src/lists.cpp
${CXX:-g++} $* -std=c++17 -Wall -o bin/section_data src/section_data.cpp
${CXX:-g++} $* -std=c++17 -Wall -o bin/toc_diff src/toc_diff.cpp
${CXX:-g++} $* -std=c++17 -Wall -DNDEBUG -O2 -o bin/list_issues src/date.cpp src/issues.cpp src/status.cpp src/sections.cpp src/list_issues.cpp
${CXX:-g++} $* -std=c++17 -Wall -DNDEBUG -O2 -o bin/set_status  src/set_status.cpp src/status.cpp
