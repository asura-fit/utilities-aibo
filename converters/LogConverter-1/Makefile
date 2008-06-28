# -*- mode: makefile;  buffer-file-coding-system: utf-8-unix -*-
############################################################
#
# Log::xxx(target, format, vars)を
# LOG_XXX(target, format, << vars)に変換するコンバータのコンパイル
#
# Copyright (C) Asura 2005, All right reserved.
#
############################################################

all: logconv1

logconv1 : LogConverter.o
	gcc -o $@ $< -ll

LogConverter.o : LogConverter.c
	gcc -c -o $@ $< -Wall

LogConverter.c : LogConverter.l
	lex -o $@ $<
