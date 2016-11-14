#! /bin/sh

cp -r LWG tmp
bin/lists tmp/
cp tmp/mailing/* gh-pages/
rm -r tmp

