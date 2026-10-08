// Created by Grigore Stefan <g_stefan@yahoo.com>
// Public domain (Unlicense) <http://unlicense.org>
// SPDX-FileCopyrightText: 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: Unlicense

Script.requireExtension("Console");
Script.requireExtension("Application");

Console.writeLn(Application.getExecutable());
Console.writeLn(Application.getPathExecutable());
for(var k=0;k<Application.getCmdN();++k){
	Console.writeLn(k+":"+Application.getCmdS(k));
};

if(!Script.isUndefined(Application.getCmdS(Application.getCmdN()))){
	throw "test fail: getCmdS(getCmdN()) must be undefined";
};
if(!Script.isUndefined(Application.getCmdS(Application.getCmdN()+50))){
	throw "test fail: getCmdS(getCmdN()+50) must be undefined";
};
Application.arguments.push("--define=A=B");
if(Application.getFlagValue("define")!="A=B"){
	throw "test fail: getFlagValue must return everything after the first =";
};
