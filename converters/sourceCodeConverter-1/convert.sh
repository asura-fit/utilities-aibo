#!/bin/sh
#
#

date

for TARGET in `find components robot -name \*.cc -o -name \*.h` `find robot-make -name Makefile* -o -name mksrclist.rb`
  do

  echo -ne "Converting $TARGET ... \x1b[s"

  echo -ne '\x1b[u\x1b[KChecking "Local Variables:" in the file.'
  grep -i -l -e "Local Variables:" $TARGET >> localvariable.txt

  # no need, because of subversion
  #echo -ne '\x1b[u\x1b[KBackuping original file'
  #cp $TARGET $TARGET.org

  echo -ne '\x1b[u\x1b[KConverting to UTF-8'
  lv -Ou8 $TARGET > $TARGET.tmp.1 || exit -1
  cp $TARGET.tmp.1 $TARGET.tmp.2

  echo -ne '\x1b[u\x1b[KIndenting'
  emacs -batch -eval "(setq my-file-name \"$TARGET.tmp.2\")" -load `pwd`/basic.el -load `pwd`/convert.el > /dev/null 2>&1 || exit -1

  echo -ne '\x1b[u\x1b[KAppending the emacs variables to head of the file.'
  cat header.txt $TARGET.tmp.2 footer.txt > $TARGET.tmp.3 || exit -1

  echo -ne '\x1b[u\x1b[KOverwrite Original File.'
  mv -f $TARGET.tmp.3 $TARGET

  echo -ne "\x1b[u\x1b[Kdone.\n"

done

date
