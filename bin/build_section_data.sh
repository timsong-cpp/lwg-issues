#!/bin/sh
grep -v "^[A-Z]" <bin/annex-f | grep -v "^[0-9]" | grep -v "ISO/IEC" | grep -v '^$' >bin/index
bin/section_data <bin/index >bin/section.data

if [ -e bin/networking-annex-f ]; then
    grep -v "^[A-Z]" <bin/networking-annex-f | grep -v "^[0-9]" | grep -v "ISO/IEC" | grep -v '^$' >bin/networking-index
    bin/section_data networking.ts <bin/networking-index >bin/networking-section.data
    cat bin/networking-section.data >> bin/section.data
    cp networking-section.data meta-data/networking-section.data
fi

cat meta-data/networking-section.data >> bin/section.data
cat meta-data/tr1_section.data >>bin/section.data
ls -l bin/section.data
