#!/bin/sh
# check-variable.sh 
# usage: ./check-variable.sh VARIABLE_NAME=VARIABLE_CONTENT CHOICES_OF_VARIABLE_CONTENT"
# if CHOICES_OF_VARIABLE_CONTENT is "%any%", this script check only whether VARIABLE_CONTENT is empty.
# example: ./check-variable.sh ROLE=${ROLE} Red Blue 
# ${ROLE} is variable and Red, Blue are CHOICES_OF_VARIABLE.

SED=`which sed`
TARGET_VAR=`echo $1|$SED -e 's/=.*//'`
TARGET_CONTENT=`echo $1|$SED -e 's/.*=//'`

shift 1

if [ $# -lt 1 ];then
    echo "===== need more than 2 arguments"
    echo "variable name: $TARGET_VAR"
    echo "variable content: $TARGET_CONTENT"
    echo "choices of variable content: $*"
    exit 1
fi

if [ x$TARGET_CONTENT = x ];then
    echo "===== $TARGET_VAR is empty"
    echo "===== please comfirm $TARGET_VAR"
    exit 1
fi

#check only whether TARGET_CONTENT is empty
if [ x$1 = x%any% ];then
    echo "===== variable $TARGET_VAR = $TARGET_CONTENT is ok"
    exit 0
fi

for i in $*
  do
  if [ $i = $TARGET_CONTENT ];then
      echo "===== variable $TARGET_VAR = $TARGET_CONTENT is ok"
      exit 0
  fi
done
echo "===== $TARGET_CONTENT is invalid in $TARGET_VAR"
echo "===== please confirm $TARGET_VAR"
echo "choices of $TARGET_VAR: $*"
exit 1
