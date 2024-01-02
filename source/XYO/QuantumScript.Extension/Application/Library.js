// Quantum Script Extension Application
// Copyright (c) 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

(function() {
var x;
var n;
var k;
n = Application.getCmdN();
Application.arguments = [];
for (x = 1, k = 0; x < n; ++x, ++k) {
	Application.arguments[k] = Application.getCmdS(x);
};
})();

Application.getFlagValue = function(argument, defaultValue) {
	var scan;
	for (scan of Application.arguments) {
		if (scan.indexOf("--") == 0) {
			if (scan.indexOf("=") >= 0) {
				var info = scan.split("=");
				if (info.length > 1) {
					if (info[0].substring(2) == argument) {
						return info[1];
					};
				};
			};
		};
	};
	return defaultValue;
};

Application.hasFlag = function(argument) {
	var scan;
	for (scan of Application.arguments) {
		if (scan == "--" + argument) {
			return true;
		};
		if (scan.indexOf("--" + argument + "=") >= 0) {
			return true;
		};
	};
	return false;
};

Application.getArgument = function(position, defaultValue) {
	var scan;
	var index = 0;
	for (scan of Application.arguments) {
		if (scan.indexOf("--") == 0) {
			continue;
		};
		if (position == index) {
			return scan;
		};
		++index;
	};
	return defaultValue;
};
