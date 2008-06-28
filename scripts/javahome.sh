#!/bin/sh

if [ "${JAVA_HOME}" != "" ]; then
	echo ${JAVA_HOME}
	exit 0;
fi

# インストールされている JDK を探す
function searchJDKHome() {

    JDK_PREFIX="/usr/local"
    if [ "${JDK_PREFIX}" = "" ]; then
		JDK_PREFIX="./"
    fi

    ALL_VERSIONS="";
    pushd ${JDK_PREFIX} > /dev/null
    for dir in j2sdk*
    do
	if [ "$dir" = "j2sdk*" ]; then
	    echo "No JDK Found."
	    exit -1;
	fi
	ALL_VERSIONS="${ALL_VERSIONS}\n${dir##j2sdk}";
    done
    popd > /dev/null
    VERSION=`echo -e "${ALL_VERSIONS}" | sort -nr | head -1`

    echo ${JDK_PREFIX}/j2sdk${VERSION}
}

searchJDKHome
