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

	class ENGINE_API ShaderIncluder : public shaderc::CompileOptions::IncluderInterface
	{
	public:
		shaderc_include_result* GetInclude(const char* requested_source, shaderc_include_type type, const char* requesting_source, size_t include_depth) override;
		void ReleaseInclude(shaderc_include_result* data) override;
	};
}
