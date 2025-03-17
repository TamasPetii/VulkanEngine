#pragma once
#include "../EngineApi.h"
#include <shaderc/shaderc.hpp>
#include <string>
#include <vector>

namespace Vk
{
	class ENGINE_API ShaderCompiler
	{
	public:
		static std::vector<uint32_t> Compile(const std::string& shaderFilePath, shaderc_shader_kind kind);
	private:
		static std::string LoadGlslShader(const std::string& shaderFilePath);
		static std::vector<uint32_t> CompileGlslToSpirv(const std::string& shaderFilePath, const std::string& glslSource, shaderc_shader_kind kind);
	};
}
