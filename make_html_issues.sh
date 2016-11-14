#! /bin/sh

cd LWG
git pull
cd ..

cp -r LWG tmp
mkdir -p tmp/mailing
bin/lists tmp/
rm gh-pages/*.html
mv tmp/mailing/* gh-pages/
rm -r tmp
cd gh-pages
cp lwg-toc.html index.html
git add -A
git commit -m 'Update'
git push
cd ..
