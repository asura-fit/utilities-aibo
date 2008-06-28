#!/bin/sh

# pwd の結果から、ASURA_HOMEの位置を推定する
# ただし、make-asura.sh を実行できるディレクトリは
#   Asura-core
# だけとする
BASE_DIR=`pwd | sed -e 's/\(.*\)\/.*$/\1/'`
CUR_DIR=`pwd | sed -e 's/.*\/\(.*\)$/\1/'`
#echo $BASE_DIR
#echo $CUR_DIR
case "${CUR_DIR}" in
robot-make)
    ASURA_HOME="${BASE_DIR}"
    TARGET="robot-make"
    ;;
*)
    cat <<EOF
This script cannot be invoked at any places except robot-make. 
So please invoke this script at the Asura-core directory.
Eg)
  % cd trunk/robot-make
  % ../utilities/script/make-asura.sh mkmemstick
EOF
    ;;
esac
#echo ${ASURA_HOME}
#exit 1;

cache_file=".make-asura"
if [ -e $cache_file -a -f $cache_file ]; then
	. ${cache_file}
fi

function quitHandler () {
    exit 0
}

trap quitHandler INT 

# dialog コマンドを使ってメニューを表示する。
# 結果は resultDisplayMenu 変数に格納される。
# 引数は
# height width menu_height title \
#   item_val1 item_label1 item_val2 item_label2 ....
function displayMenu () {
    local height=$1;      shift;
    local width=$1;       shift;
    local menu_height=$1; shift;

    local title=$1;       shift;
    echo "$title"

    local result=`tempfile`

    dialog --backtitle "$title" --menu "$title" $height $width $menu_height "$@" 2> $result

    resultDisplayMenu=`cat $result`
    rm $result
}

function readTeamColor() {
    displayMenu 16 60 9 "Please select Team color" \
	"Red" "Red" \
	"Blue" "Blue"
}

    
function readRoleParam() {
    displayMenu 16 60 9 "Please select ROLE parameter"\
	"Striker"  "Striker"\
	"Libero"   "Libero"\
	"Defender" "Defender"\
	"Goalie"   "Goalie"
}

function readPlaceParam() {
    local PLACES CONF_DIR
    PLACES=""
    CONF_DIR="${ASURA_HOME}/configurations/conf"
    
    for file in ${CONF_DIR}/*
    do
	file=`echo $file | sed -e 's/.*\/\(.*\)$/\1/'`
	if [ "${file}" != "CVS" -a "${file}" != "${CONF_DIR}/*" ]
	then
	    PLACES="${PLACES} ${file} ${file}"
	fi
    done
    displayMenu 16 60 9 "Please select PLACE parameter" $PLACES
}

function readNumberCheckMode() {
    displayMenu 16 60 9 "Please select NUMBER_CHECK mode"\
		"ENABLE" "Enable" \
		"DISABLE" "Disable"
}

function readInstallDir() {
	local defval=$1
	local result=`tempfile`

    dialog --backtitle "$title" \
	       --inputbox "Plese Input INSTALLDIR" 0 0 $defval 2> $result
    resultDisplayMenu=`cat $result`
	rm $result
}

function makeAsuraCore () {
	flag="false";
	while [ "$flag" = "false" ]; do
		readTeamColor
		TEAM=$resultDisplayMenu
		readRoleParam
		ROLE=$resultDisplayMenu
		readPlaceParam
		PLACE=$resultDisplayMenu
		readNumberCheckMode
		NUMCHK=$resultDisplayMenu
		readInstallDir $INSTALLDIR
		INSTALLDIR=$resultDisplayMenu

		echo 
		dialog --yesno \
"Team Color : $TEAM
Role : $ROLE
Place : $PLACE
NumberCheck : $NUMCHK
INSTALLDIR  : $INSTALLDIR" 0 0

		if [ "$?" = "0" ]; then
			flag="true";
		fi
	done

	echo "TEAM=\"$TESM\"" > $cache_file
	echo "ROLE=\"$ROLE\"" >> $cache_file
	echo "PLACE=\"$PLACE\"" >> $cache_file
	echo "NUMCHK=\"$NUMCHK\"" >> $cache_file
	echo "INSTALLDIR=\"$INSTALLDIR\"" >> $cache_file

   	make PLACE="$PLACE" ROLE="$ROLE" TEAM="$TEAM"  \
	     NUMBER_CHECK_MODE="$NUMCHK" INSTALLDIR="$INSTALLDIR" $@
}

case "${TARGET}" in
robot-make )
    makeAsuraCore $@
    ;;
*)
    ;;
esac
