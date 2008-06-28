#
# Copyright 2002 Sony Corporation 
#
# Permission to use, copy, modify, and redistribute this software for
# non-commercial use is hereby granted.
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#

ROBOTS=
PLACE=

# ê›íËÉtÉ@ÉCÉã
GENCONFJAVA_CONF_DIR=../configurations/conf/$(PLACE)
GENCONFJAVA_CONF_FILE=genconf.cnf

COMPONENTS=RelayMessage 
INSTALLDIR=$(shell pwd)/MS
TARGETS=all install

.PHONY: $(TARGETS) checkrobots genconf

$(TARGETS):
	for dir in $(COMPONENTS); do \
		(cd $$dir && $(MAKE) INSTALLDIR=$(INSTALLDIR) $@) \
	done

clean:
	for dir in $(COMPONENTS); do \
		(cd $$dir && $(MAKE) $@) \
	done

check-place:
	@../scripts/check-variable.sh PLACE=$(PLACE) %any%

check-robots:
	@if [ "$(ROBOTS)" = "" ];then \
		echo "=== please specify ROBOTS"; \
		echo "ROBOTS: Striker, Defender, Libero, Goalie";\
		echo "";\
		echo "sample: make ROBOTS=\"Libero Goalie\"";\
		exit 1;\
	fi
	@for robo in $(ROBOTS); do\
		if [ \( "$$robo" != "Libero" \) -a \( "$$robo" != "Striker" \) -a \( "$$robo" != "Defender" \) -a \( "$$robo" != "Goalie" \) ]; then \
			echo "role is invalid (" $$robo ")";\
	 		echo "ROBOTS: Striker, Defender, Libero, Goalie";\
		fi;\
	done

JAVA=$(shell ../scripts/javapath.rb --bin)/java
GENCONFJAVA_JAR=../genconfig/genconfig.jar
CFGDIR=MS/OPEN-R/MW/CONF

genconf : check-robots check-place
	$(JAVA) -jar $(GENCONFJAVA_JAR) -o $(CFGDIR)/CONNECT.CFG $(GENCONFJAVA_CONF_DIR)/$(GENCONFJAVA_CONF_FILE) HUB_CONNECT $(ROBOTS)
	$(JAVA) -jar $(GENCONFJAVA_JAR) -o $(CFGDIR)/HOSTGW.CFG $(GENCONFJAVA_CONF_DIR)/$(GENCONFJAVA_CONF_FILE) HUB_HOSTGW $(ROBOTS)
