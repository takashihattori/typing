#/bin/sh
cd ja_JP.eucJP
for f in *.txt
do
    nkf --ic=EUC-JP --oc=UTF-8 $f >../ja_JP.UTF-8/$f
done
