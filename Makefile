# The binaries that we want to build
PGMS := bin/lists bin/section_data bin/toc_diff bin/list_issues bin/set_status
CXXFLAGS := -std=c++17 -Wall -g -O2 -D_GLIBCXX_ASSERTIONS

# Running 'make debug' is equivalent to 'make DEBUG=1'
ifeq "$(MAKECMDGOALS)" "debug"
DEBUG := 1
endif

# Running 'make DEBUG=blah' rebuilds all binaries with debug settings
ifdef DEBUG
debug: pgms
CPPFLAGS := -DDEBUG_SUPPORT -D_GLIBCXX_DEBUG
CXXFLAGS += -O0
ifdef LOGGING
CPPFLAGS += -DDEBUG_LOGGING
endif
# Add UBSAN=1 to the command line to enable Undefined Behaviour Sanitizer
ifdef UBSAN
CXXFLAGS += -fsanitize=undefined
endif
# Add ASAN=1 to the command line to enable Address Sanitizer
ifdef ASAN
CXXFLAGS += -fsanitize=address
endif
# Make 'clean' a prerequisite of all object files, so everything is rebuilt:
src/*.o: clean
.DEFAULT_GOAL: all
endif

all: pgms

pgms: $(PGMS)

bin/lists: src/date.o src/issues.o src/status.o src/sections.o src/mailing_info.o src/report_generator.o src/lists.o

bin/section_data: src/section_data.o

bin/toc_diff: src/toc_diff.o

bin/list_issues: src/date.o src/issues.o src/status.o src/sections.o src/list_issues.o

bin/set_status: src/set_status.o src/status.o

$(PGMS):
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f $(PGMS) src/*.o

.PHONY: all pgms clean
