FILES = typingadmin2-euc typingadmin2-utf8 typingadmin2-en

all: typingadmin2-en typingadmin2-utf8

typingadmin2-en: typingadmin2-euc translate.sed
	LANG=ja_JP.eucJP sed -f translate.sed typingadmin2-euc >typingadmin2-en

typingadmin2-utf8: typingadmin2-euc
	nkf --ic=EUC-JP --oc=UTF-8 typingadmin2-euc >typingadmin2-utf8

install:
	install -c -m 750 -g ipl $(FILES) $(DEST_DIR)
