#/bin/sh
cd data
for f in sfc-e*.txt
do
    ruby ../q-generate.rb ../template/ja.txt $f 150 >../ja_JP.UTF-8/$f
    ruby ../q-generate.rb ../template/en.txt $f 150 >../en/$f
done
for f in sfc-c*.txt
do
    ruby ../q-generate.rb ../template/ja.txt $f 100 >../ja_JP.UTF-8/$f
    ruby ../q-generate.rb ../template/en.txt $f 100 >../en/$f
done
cd ../ja_JP.UTF-8
for f in *.txt
do
    nkf --ic=UTF-8 --oc=EUC-JP $f | LANG=ja_JP.eucJP sed -e 's/ja_JP.UTF-8/ja_JP.eucJP/' >../ja_JP.eucJP/$f
done
