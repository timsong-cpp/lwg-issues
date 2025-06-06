#! /bin/sh -e

git submodule foreach git pull origin

git pull

make -j10

cd LWG
make new-papers dates
cd ..

cp -pr LWG tmp

if [ -f section.data ]
then
   cp section.data tmp/meta-data/
fi

if [ -f lwg-toc.old.html ]
then
   cp lwg-toc.old.html tmp/meta-data/
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
