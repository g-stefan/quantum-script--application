//
// Quantum Script Extension Application
//
// Copyright (c) 2020-2022 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

(function() {
	var x;
	var n;
	var k;
	n = Application.getCmdN();
	Application.arguments = [];
	for(x = 1, k = 0; x < n; ++x, ++k) {
		Application.arguments[k] = Application.getCmdS(x);
	};
})();
