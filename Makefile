# The binaries that we want to build
PGMS := bin/lists bin/section_data bin/list_issues bin/set_status
CXXSTD := -std=c++20
CXXFLAGS := $(CXXSTD) -Wall -g -O2
CPPFLAGS := -MMD -D_GLIBCXX_ASSERTIONS

# Running 'make debug' is equivalent to 'make DEBUG=1'
ifeq "$(MAKECMDGOALS)" "debug"
DEBUG := 1
endif

# Running 'make DEBUG=blah' rebuilds all binaries with debug settings
ifdef DEBUG
debug: pgms
CPPFLAGS += -DDEBUG_SUPPORT -D_GLIBCXX_DEBUG
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

all: check pgms

pgms: $(PGMS)

-include src/*.d

bin/lists: src/issues.o src/status.o src/sections.o src/mailing_info.o src/report_generator.o src/lists.o src/metadata.o src/html_utils.o

bin/section_data: src/section_data.o

bin/list_issues: src/issues.o src/status.o src/sections.o src/list_issues.o src/metadata.o src/html_utils.o

bin/set_status: src/set_status.o src/status.o

bin/self_test_%: CPPFLAGS += -DSELF_TEST
bin/self_test_%: CXXFLAGS += -O0 -MF src/self_test_$*.d
bin/self_test_%: src/%.cpp
	$(LINK.C) $< $(LDLIBS) -o $@

check: bin/self_test_html_utils
	@x=0; for test in $^; do ./$$test || x=$$? ; done; exit $$x
.PHONY: check

$(PGMS):
	$(LINK.C) $^ $(LDLIBS) -o $@

clean:
	rm -f $(PGMS) src/*.o src/*.d bin/self_test_*


.PHONY: all pgms clean
