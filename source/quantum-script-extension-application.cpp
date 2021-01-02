//
// Quantum Script Extension Application
//
// Copyright (c) 2020-2021 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quantum-script-extension-application-license.hpp"
#include "quantum-script-extension-application.hpp"
#ifndef QUANTUM_SCRIPT_EXTENSION_APPLICATION_NO_VERSION
#include "quantum-script-extension-application-version.hpp"
#endif

#include "quantum-script-variablenumber.hpp"
#include "quantum-script-variablestring.hpp"

#include "quantum-script-extension-application.src"

//#define QUANTUM_SCRIPT_VM_DEBUG_RUNTIME

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace Application {

				using namespace XYO;
				using namespace Quantum::Script;

				static TPointer<Variable> getCmdN(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
					printf("- application-get-cmd-n\n");
#endif

					return VariableNumber::newVariable(
							((Executive *)(function->valueSuper))->mainCmdN
						);
				};

				static TPointer<Variable> getCmdS(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
					printf("- application-get-cmd-s\n");
#endif

					return VariableString::newVariable(
							(((Executive *)(function->valueSuper))->mainCmdS[(arguments->index(0))->toIndex()])
						);
				};

				static TPointer<Variable> getExecutable(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
					printf("- application-get-executable\n");
#endif

					String executable = ((Executive *)(function->valueSuper))->pathExecutable;
					executable << Shell::pathSeparator;
					executable << Shell::getFileName(((Executive *)(function->valueSuper))->mainCmdS[0]);

					return VariableString::newVariable(
							executable
						);
				};

				static TPointer<Variable> getPathExecutable(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
					printf("- application-get-path-executable\n");
#endif

					return VariableString::newVariable(
							((Executive *)(function->valueSuper))->pathExecutable
						);
				};

				void registerInternalExtension(Executive *executive) {
					executive->registerInternalExtension("Application", initExecutive);
				};

				void initExecutive(Executive *executive, void *extensionId) {

					String info = "Application\r\n";
					info << License::shortContent();

					executive->setExtensionName(extensionId, "Application");
					executive->setExtensionInfo(extensionId, info);
#ifndef QUANTUM_SCRIPT_EXTENSION_APPLICATION_NO_VERSION
					executive->setExtensionVersion(extensionId, Extension::Application::Version::versionWithBuild());
#endif
					executive->setExtensionPublic(extensionId, true);

					executive->compileStringX(
						"var Application={};"
					);

					executive->setFunction4("Application.getCmdN", getCmdN, executive);
					executive->setFunction4("Application.getCmdS(x)", getCmdS, executive);
					executive->setFunction4("Application.getExecutable", getExecutable, executive);
					executive->setFunction4("Application.getPathExecutable", getPathExecutable, executive);

					executive->compileStringX(extensionApplicationSource);
				};


			};
		};
	};
};

#ifdef XYO_COMPILE_DYNAMIC_LIBRARY
extern "C" QUANTUM_SCRIPT_EXTENSION_APPLICATION_EXPORT void quantumScriptExtension(Quantum::Script::Executive *executive, void *extensionId) {
	Quantum::Script::Extension::Application::initExecutive(executive, extensionId);
};
#endif

