# -*- mode: makefile;  buffer-file-coding-system: utf-8-unix -*-
############################################################
#
# LOG_XXX(target, format, << vars << ...)を
# LOG_XXX(target, format, , vars, ...)に変換するコンバータ
# のコンパイル
#
# Copyright (C) Asura 2005, All right reserved.
#
############################################################

all: logconv2

logconv2 : LogConverter.o
	gcc -o $@ $< -ll

LogConverter.o : LogConverter.c
	gcc -c -o $@ $< -Wall

LogConverter.c : LogConverter.l
	lex -o $@ $<
