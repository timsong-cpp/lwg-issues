#! /bin/sh

git submodule foreach git pull origin

git pull

# Restore timestamp from the last commit that touched the file.
# Otherwise the "last modified" time displayed is worthless.
cd LWG
../git-tools/git-restore-mtime --force
cd ..

cp -pr LWG tmp

if [ -f section.data ]
then
   cp section.data tmp/meta-data/
fi

rm -rf tmp/mailing
mkdir -p tmp/mailing
bin/lists tmp/
rm gh-pages/*.html
mv tmp/mailing/* gh-pages/
rm -r tmp
cd gh-pages
cp ../index.html ./
git add -A
git commit -m 'Update'
git push
cd ..

git commit -am 'Update'
git push

