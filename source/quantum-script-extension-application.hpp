//
// Quantum Script Extension Application
//
// Copyright (c) 2020-2021 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#ifndef QUANTUM_SCRIPT_EXTENSION_APPLICATION_HPP
#define QUANTUM_SCRIPT_EXTENSION_APPLICATION_HPP

#ifndef QUANTUM_SCRIPT_HPP
#include "quantum-script.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_APPLICATION__EXPORT_HPP
#include "quantum-script-extension-application--export.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_APPLICATION_COPYRIGHT_HPP
#include "quantum-script-extension-application-copyright.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_APPLICATION_LICENSE_HPP
#include "quantum-script-extension-application-license.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_APPLICATION_VERSION_HPP
#include "quantum-script-extension-application-version.hpp"
#endif

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace Application {

				using namespace Quantum::Script;

				QUANTUM_SCRIPT_EXTENSION_APPLICATION_EXPORT void initExecutive(Executive *executive, void *extensionId);
				QUANTUM_SCRIPT_EXTENSION_APPLICATION_EXPORT void registerInternalExtension(Executive *executive);

			};
		};
	};
};

#endif
