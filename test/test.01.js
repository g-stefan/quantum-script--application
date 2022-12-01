// Created by Grigore Stefan <g_stefan@yahoo.com>
// Public domain (Unlicense) <http://unlicense.org>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: Unlicense

Script.requireExtension("Console");
Script.requireExtension("Application");

Console.writeLn(Application.getExecutable());
Console.writeLn(Application.getPathExecutable());
for(var k=0;k<Application.getCmdN();++k){
	Console.writeLn(k+":"+Application.getCmdS(k));
};
