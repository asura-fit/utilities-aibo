#!/bin/sh

PLACE=${PLACE:-"aiboRoom"}
INVOKE_DIR=${INVOKE_DIR:-"."}
AIMON_PORT="8000"
LOGGING=""
WITH_EMACS="true"

OPENR_BASE="/usr/local/OPEN_R_SDK"
RP_OPENR_BASE="${OPENR_BASE}/RP_OPEN_R"
RP_OPENR_START="${RP_OPENR_BASE}/bin/start-rp-openr"

# parse command-line options
set -- `getopt "p:P:d:l:ehr" $*`
if [ $? != 0 ]; then usage; fi
while [ "$1" != -- ]
do
	case $1 in
	-p)
		AIMON_PORT=$2
		shift
		;;
	-P)
		PLACE=$2
		shift
		;;
	-d)
		INVOKE_DIR=$2
		shift
		;;
	-l)
		LOGGING=$2
		shift
		;;
	-e)
		if [ "$WITH_EMACS" = "true" ]; then
			WITH_EMACS="false"
		else
			WITH_EMACS="true"
		fi
		;;
	-r)
		${RP_OPENR_BASE}/bin/rp-openr-ipcrm
		;;
	-h)
		usage
		;;
	*)
		usage
		exit
		;;
	esac
	shift
done
shift


# モジュール src が checkout されたディレクトリを取得する
# たとえば、~/Work で cvs checkout src が実行されていて、
# Asura-core, Util, AiboTool などのディレクトリ内で、
# checkAsuraBase を実行すると, ~/Work/src が出力される。
# 実際には ~/Work の部分はフルパスに展開される。
function checkAsuraBase () {
    # assume in the aiboTools
    echo "../";
    #local current=`pwd`
    # echo $current | sed -e 's/\(.*\/src\)\/.*$/\1/'
}

ASURA_BASE=`checkAsuraBase`

GENCONF_BASE="${ASURA_BASE}/genconfig"
GENCONF_TEMPLATE="${GENCONF_BASE}/template"
GENCONF_JAR="${GENCONF_BASE}/genconfig.jar"
GENCONF_CONF="${ASURA_BASE}/configurations/conf/${PLACE}/genconf.cnf"

HOSTGW_TEMPLATE="${GENCONF_TEMPLATE}/AIBOTOOLS.HOSTGW.CFG.templ"
CFG_PATH="${INVOKE_DIR}/MS/OPEN-R/MW/CONF"
HOSTGW="${CFG_PATH}/HOSTGW.CFG"
PIPENAME="${INVOKE_DIR}/stdIn.pipe"

JAVA=`${ASURA_BASE}/scripts/javapath.rb --bin`/java

function genconfig () {
    ${JAVA} -jar ${GENCONF_JAR} $@
}

function genHOSTGW () {
	local target=$1
	local output=${2:-$HOSTGW}

	genconfig -t ${HOSTGW_TEMPLATE} -o ${output} \
	             ${GENCONF_CONF} AIMON ${target}
}

function genHOSTGW2() {
	local target=$1
	local output=${2:-$HOSTGW}

	cat ${HOSTGW_TEMPLATE} | sed -e $"s/\\\${IP}/$target/" > $output
}

function createCFG_DIR () {
	if [ -e $CFG_DIR ]; then
		if [ -d ${CFG_DIR} ]; then
			return;
		else
			echo "cannot create directory : ${CFG_DIR}"
			echo "a same name file already exists!"
		fi
	else
		mkdir -p ${CFG_DIR}
	fi
}

function startRP_base () {
	echo "Enter invoke dir [${INVOKE_DIR}/]"
	pushd ${INVOKE_DIR} > /dev/null

	export AIMON_PORT
	
	${RP_OPENR_START}

	popd > /dev/null
	echo "Leave from invoke dir"
}

function startRP () {
	if [ "$LOGGING" = "" ]; then 
		startRP_base
	else
		startRP_base | tee $LOGGING
	fi
}

function usage () {
    cat <<EOF
Usage: runtools.sh [options...] target
target: Striker, Libero, Defender or Goalie
options:
	-d	INVOKE_DIR
	-p	specify port number used as listen port connected from Aimon
	-P	specify PLACE parameter to select configuration file
	-l	specify log file
	-r	clean up IPC resources
	-h	display this message
EOF
}

if [ "$1" = "" ]; then
	usage
	exit
fi

# create stdIn.pipe
if [ -e $PIPENAME ]; then
	if [ ! -p $PIPENAME ]; then
		echo "cannot create stdIn.pipe"
		echo "same name file is already exists"
	fi
else
	mknod $PIPENAME p
fi

function isIPAddress() {
	for r in Striker Libero Defender Goalie
	do
		r=`echo $r | tr [:upper:] [:lower:]`
		r2=`echo $1 | tr [:upper:] [:lower:]`
		if [ "$r" = "$r2" ]; then
			echo -n false
			return
		fi
	done
	echo -n true
}

IS_IPADDR=`isIPAddress $1`

# start emacs 
if [ "$WITH_EMACS" = "true" ]; then
	emacs -f asura-scm-aibotool &
fi

# start RP-OPEN-R
createCFG_DIR
if [ "$IS_IPADDR" = "true" ]; then
	genHOSTGW2 $1
else
	genHOSTGW $1
fi
startRP
