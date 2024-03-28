// Quantum Script Extension Application
// Copyright (c) 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/QuantumScript.Extension/Application/Library.hpp>
#include <XYO/QuantumScript.Extension/Application/Copyright.hpp>
#include <XYO/QuantumScript.Extension/Application/License.hpp>
#include <XYO/QuantumScript.Extension/Application/Version.hpp>
#include <XYO/QuantumScript.Extension/Application/Library.Source.cpp>

namespace XYO::QuantumScript::Extension::Application {

	static TPointer<Variable> getCmdN(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef XYO_QUANTUMSCRIPT_DEBUG_RUNTIME
		printf("- application-get-cmd-n\n");
#endif

		return VariableNumber::newVariable(
		    ((Executive *)(function->valueSuper))->mainCmdN);
	};

	static TPointer<Variable> getCmdS(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef XYO_QUANTUMSCRIPT_DEBUG_RUNTIME
		printf("- application-get-cmd-s\n");
#endif

		return VariableString::newVariable(
		    (((Executive *)(function->valueSuper))->mainCmdS[(arguments->index(0))->toIndex()]));
	};

	static TPointer<Variable> getExecutable(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef XYO_QUANTUMSCRIPT_DEBUG_RUNTIME
		printf("- application-get-executable\n");
#endif

		String executable = ((Executive *)(function->valueSuper))->pathExecutable;
		executable << Shell::pathSeparator;
		executable << Shell::getFileName(((Executive *)(function->valueSuper))->mainCmdS[0]);

		return VariableString::newVariable(
		    executable);
	};

	static TPointer<Variable> getPathExecutable(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef XYO_QUANTUMSCRIPT_DEBUG_RUNTIME
		printf("- application-get-path-executable\n");
#endif

		return VariableString::newVariable(
		    ((Executive *)(function->valueSuper))->pathExecutable);
	};

	void registerInternalExtension(Executive *executive) {
		executive->registerInternalExtension("Application", initExecutive);
	};

	void initExecutive(Executive *executive, void *extensionId) {

		String info = "Application\r\n";
		info << License::shortLicense().c_str();

		executive->setExtensionName(extensionId, "Application");
		executive->setExtensionInfo(extensionId, info);
		executive->setExtensionVersion(extensionId, Extension::Application::Version::versionWithBuild());
		executive->setExtensionPublic(extensionId, true);

		executive->compileStringX(
		    "var Application={};");

		executive->setFunction4("Application.getCmdN", getCmdN, executive);
		executive->setFunction4("Application.getCmdS(x)", getCmdS, executive);
		executive->setFunction4("Application.getExecutable", getExecutable, executive);
		executive->setFunction4("Application.getPathExecutable", getPathExecutable, executive);

		executive->compileStringX(librarySource);
	};

};

#ifdef XYO_PLATFORM_COMPILE_DYNAMIC_LIBRARY
extern "C" XYO_QUANTUMSCRIPT_EXTENSION_APPLICATION_EXPORT void quantumScriptExtension(XYO::QuantumScript::Executive *executive, void *extensionId) {
	XYO::QuantumScript::Extension::Application::initExecutive(executive, extensionId);
};
#endif
