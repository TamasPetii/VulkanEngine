#include "ShaderCompiler.h"
#include <fstream>
#include <iostream>
#include <filesystem>

std::vector<uint32_t> Vk::ShaderCompiler::Compile(const std::string& shaderFilePath, shaderc_shader_kind kind)
{
	std::string glslSource = LoadGlslShader(shaderFilePath);
	std::string fileName = std::filesystem::path(shaderFilePath).filename().string();
	return CompileGlslToSpirv(glslSource, fileName, kind);
}

std::string Vk::ShaderCompiler::LoadGlslShader(const std::string& shaderFilePath)
{
	std::ifstream file(shaderFilePath);

	if (!file.is_open()) {
		throw std::runtime_error("Failed to open shader file = " + shaderFilePath);
	}

	std::string line, source;
	while (std::getline(file, line))
	{
		source += line + "\n";
	}

	return source;
}

std::vector<uint32_t> Vk::ShaderCompiler::CompileGlslToSpirv(const std::string& glslSource, const std::string& shaderFileName, shaderc_shader_kind kind)
{
	shaderc::Compiler compiler;
	shaderc::CompileOptions options;

	options.SetOptimizationLevel(shaderc_optimization_level_performance);
	shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(glslSource, kind, shaderFileName.c_str(), options);

	if (result.GetCompilationStatus() != shaderc_compilation_status_success)
	{
		std::cerr << "Shader compilation failed: " << result.GetErrorMessage() << "\n";
		throw std::runtime_error("Failed to compile shader file = " + shaderFileName);
	}

	return { result.begin(), result.end() };
}