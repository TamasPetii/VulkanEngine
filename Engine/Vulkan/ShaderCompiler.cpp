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
	options.SetIncluder(std::make_unique<ShaderIncluder>());

	options.SetOptimizationLevel(shaderc_optimization_level_performance);
	shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(glslSource, kind, shaderFileName.c_str(), options);

	if (result.GetCompilationStatus() != shaderc_compilation_status_success)
	{
		std::cerr << "Shader compilation failed: " << result.GetErrorMessage() << "\n";
		throw std::runtime_error("Failed to compile shader file = " + shaderFileName);
	}

	return { result.begin(), result.end() };
}

shaderc_include_result* Vk::ShaderIncluder::GetInclude(const char* requested_source, shaderc_include_type type, const char* requesting_source, size_t include_depth)
{
	std::string filepath = std::string("../Engine/Shaders/Common/") + requested_source;

	std::ifstream file(filepath);
	if (!file.is_open()) {
		std::cerr << "Failed to open include file: " << filepath << std::endl;
		return new shaderc_include_result{ "", 0, "File not found", 13, nullptr };
	}

	std::string content((std::istreambuf_iterator<char>(file)),	std::istreambuf_iterator<char>());
	file.close();

	auto* container = new std::pair<std::string, std::string>{requested_source, content };
	auto* result = new shaderc_include_result{
		container->first.data(), container->first.size(),
		container->second.data(), container->second.size(),
		container };

	return result;
}

void Vk::ShaderIncluder::ReleaseInclude(shaderc_include_result* data)
{
	delete static_cast<std::pair<std::string, std::string>*>(data->user_data);
	delete data;
}
