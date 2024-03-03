#include "ShaderFactory.h"

using namespace Microsoft::WRL;

namespace Anni
{
ShaderFactory::ShaderFactory(GraphicsComponent &gfx_) :
    gfx(gfx_),
    device_manager(gfx.DeviceMan())
{
	HRESULT hres;

	// Initialize DXC utils
	hres = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(dxc_utils.ReleaseAndGetAddressOf()));
	if (FAILED(hres))
	{
		throw std::runtime_error("Could not init DXC Utility");
	}

	// Initialize the DXC compiler
	hres = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(dxc_compiler.ReleaseAndGetAddressOf()));
	if (FAILED(hres))
	{
		throw std::runtime_error("Could not init DXC Compiler");
	}
}

std::shared_ptr<ShaderWrapper> ShaderFactory::GetShader(const std::string &path_, VkShaderStageFlagBits stage) const
{
	std::vector<char> hlsl_str = Util::ReadFile(path_);

	const DxcBuffer src_buffer{
	    .Ptr      = hlsl_str.data(),
	    .Size     = static_cast<uint32_t>(hlsl_str.size()),
	    .Encoding = 0,
	};

	//GET FILE NAME
	//const size_t start_pos = path_.find_last_of('/');
	//const auto   file_name = path_.substr(start_pos + 1, path_.size());
	//path必须要整全路径，否则renderdoc就傻眼了
	const auto file_name = path_;

	//CONVERT TO WSTRING
	const int            wchars_num = MultiByteToWideChar(CP_UTF8, 0, file_name.c_str(), -1, nullptr, 0);
	std::vector<wchar_t> wstr(wchars_num);
	MultiByteToWideChar(CP_UTF8, 0, file_name.c_str(), -1, wstr.data(), wchars_num);
	std::vector<LPCWSTR> arguments;

	//COMPILATION ARGUMENTS
	arguments.push_back(L"-spirv");
	arguments.push_back(L"-fspv-target-env=vulkan1.3");
	arguments.push_back(L"-fspv-debug=vulkan-with-source");

	arguments.push_back(L"-fspv-extension=SPV_KHR_multiview");
	arguments.push_back(L"-fspv-extension=SPV_KHR_shader_draw_parameters");
	arguments.push_back(L"-fspv-extension=SPV_EXT_descriptor_indexing");

	arguments.push_back(L"-T");        //target stage
	switch (stage)
	{
		case VK_SHADER_STAGE_VERTEX_BIT:
			arguments.push_back(L"vs_6_0");
			break;
		case VK_SHADER_STAGE_FRAGMENT_BIT:
			arguments.push_back(L"ps_6_0");
			break;
		default:
			throw std::runtime_error("Unknown shader stage!");
	}

	arguments.push_back(L"-E");        //entry point
	arguments.push_back(L"main");

	arguments.push_back(L"-HV");        //hlsl version
	arguments.push_back(L"2021");

	//arguments.push_back(DXC_ARG_DEBUG);        //-Zi选项会被-fspv-debug覆盖。
	//arguments.push_back(L"-Qembed_debug");
	//arguments.push_back(DXC_ARG_DEBUG_NAME_FOR_SOURCE);

	arguments.push_back(DXC_ARG_SKIP_OPTIMIZATIONS);              //为了更好的调试体验
	arguments.push_back(DXC_ARG_WARNINGS_ARE_ERRORS);             //-WX
	arguments.push_back(DXC_ARG_PACK_MATRIX_COLUMN_MAJOR);        //-Zpc

	//put your shader name here
	arguments.push_back(wstr.data());

	ComPtr<IDxcResult> compile_result;

	const HRESULT hres = dxc_compiler->Compile(&src_buffer, arguments.data(), (uint32_t) arguments.size(), nullptr, IID_PPV_ARGS(&compile_result));



	if (FAILED(hres))
	{
		throw std::runtime_error("failed to compile shader!");
	}

	ComPtr<IDxcBlob> shader_obj;
	compile_result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shader_obj), nullptr);

	//Error Handling
	ComPtr<IDxcBlobUtf8> error_messages;
	compile_result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(error_messages.GetAddressOf()), nullptr);
	if (error_messages && error_messages->GetStringLength() > 0)
	{
		auto str = std::string();
		str.resize(error_messages->GetBufferSize());
		for (size_t i = 0; i < str.size(); ++i)
		{
			str[i] = static_cast<char const *>(error_messages->GetBufferPointer())[i];
		}
		MessageBoxA(nullptr, str.c_str(), "DXC Compiler Error", MB_OK);

		throw std::runtime_error("compile error!");
	}

	VkShaderModuleCreateInfo shader_module_CI{};
	shader_module_CI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

	//static std::vector<std::uint32_t> spriv_buffer;
	//spriv_buffer.resize(shader_obj->GetBufferSize() / sizeof(std::uint32_t));

	//for (size_t i = 0; i < spriv_buffer.size(); ++i)
	//{
	//	const std::uint32_t spv_uint = static_cast<std::uint32_t *>(shader_obj->GetBufferPointer())[i];        //
	//	spriv_buffer[i]              = spv_uint;
	//}
	//shader_module_CI.codeSize = shader_obj->GetBufferSize();
	//shader_module_CI.pCode    = spriv_buffer.data();

	//Create a Vulkan shader module from the compilation result
	shader_module_CI.codeSize = shader_obj->GetBufferSize();
	shader_module_CI.pCode    = static_cast<uint32_t *>(shader_obj->GetBufferPointer());

	VkShaderModule shader_module;
	VK_CHECK_RESULT(vkCreateShaderModule(device_manager.GetLogicalDevice(), &shader_module_CI, nullptr, &shader_module));

	return std::make_shared<ShaderWrapper>(stage, path_, shader_module, gfx);
}

//std::shared_ptr<VkShaderExtWrapper> ShaderFactory::GetShaderExt(const std::string &path, VkShaderCreateInfoEXT shader_CI) const
//{
//	std::vector<char> hlsl_str = Util::ReadFile(path);
//
//	HRESULT         hres;
//	const DxcBuffer src_buffer{
//	    .Ptr      = hlsl_str.data(),
//	    .Size     = static_cast<uint32_t>(hlsl_str.size()),
//	    .Encoding = 0,
//	};
//
//	//GET FILE NAME
//	//const size_t start_pos = path_.find_last_of('/');
//	//const auto   file_name = path_.substr(start_pos + 1, path_.size());
//	//path必须要整全路径，否则renderdoc就傻眼了
//	const auto file_name = path;
//
//	//CONVERT TO WSTRING
//	const int            wchars_num = MultiByteToWideChar(CP_UTF8, 0, file_name.c_str(), -1, nullptr, 0);
//	std::vector<wchar_t> wstr_filename(wchars_num);
//	MultiByteToWideChar(CP_UTF8, 0, file_name.c_str(), -1, wstr_filename.data(), wchars_num);
//	std::vector<LPCWSTR> arguments;
//
//	//COMPILATION ARGUMENTS
//	arguments.push_back(L"-spirv");
//	arguments.push_back(L"-fspv-target-env=vulkan1.3");
//	arguments.push_back(L"-fspv-debug=vulkan-with-source");
//
//	arguments.push_back(L"-fspv-extension=SPV_KHR_multiview");
//	arguments.push_back(L"-fspv-extension=SPV_KHR_shader_draw_parameters");
//	arguments.push_back(L"-fspv-extension=SPV_EXT_descriptor_indexing");
//
//	arguments.push_back(L"-T");        //target stage
//	switch (shader_CI.stage)
//	{
//		case VK_SHADER_STAGE_VERTEX_BIT:
//			arguments.push_back(L"vs_6_0");
//			break;
//		case VK_SHADER_STAGE_FRAGMENT_BIT:
//			arguments.push_back(L"ps_6_0");
//			break;
//		default:
//			throw std::runtime_error("Unknown shader stage!");
//	}
//
//	std::wstring_convert<std::codecvt_utf8<wchar_t>> wconverter;        //to convert string to wstring
//	std::wstring                                     wideString_entry_point = wconverter.from_bytes(shader_CI.pName);
//
//	arguments.push_back(L"-E");        //entry point
//	arguments.push_back(wideString_entry_point.c_str());
//
//	arguments.push_back(L"-HV");        //hlsl version
//	arguments.push_back(L"2021");
//
//	//arguments.push_back(DXC_ARG_DEBUG);        //-Zi选项会被-fspv-debug覆盖。
//	//arguments.push_back(L"-Qembed_debug");
//	//arguments.push_back(DXC_ARG_DEBUG_NAME_FOR_SOURCE);
//
//	arguments.push_back(DXC_ARG_SKIP_OPTIMIZATIONS);              //为了更好的调试体验
//	arguments.push_back(DXC_ARG_WARNINGS_ARE_ERRORS);             //-WX
//	arguments.push_back(DXC_ARG_PACK_MATRIX_COLUMN_MAJOR);        //-Zpc
//
//	//put your shader name here
//	arguments.push_back(wstr_filename.data());
//
//	ComPtr<IDxcResult> compile_result;
//
//	hres = dxc_compiler->Compile(&src_buffer, arguments.data(), (uint32_t) arguments.size(), nullptr, IID_PPV_ARGS(&compile_result));
//
//	if (FAILED(hres))
//	{
//		throw std::runtime_error("failed to compile shader!");
//	}
//
//	ComPtr<IDxcBlob> shader_obj;
//	compile_result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shader_obj), nullptr);
//
//	//Error Handling
//	ComPtr<IDxcBlobUtf8> error_messages;
//	compile_result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(error_messages.GetAddressOf()), nullptr);
//	if (error_messages && error_messages->GetStringLength() > 0)
//	{
//		auto str = std::string();
//		str.resize(error_messages->GetBufferSize());
//		for (size_t i = 0; i < str.size(); ++i)
//		{
//			str[i] = static_cast<char const *>(error_messages->GetBufferPointer())[i];
//		}
//		MessageBoxA(nullptr, str.c_str(), "DXC Compiler Error", MB_OK);
//
//		throw std::runtime_error("compile error!");
//	}
//
//	//typedef struct VkShaderCreateInfoEXT {
//	//    VkStructureType                 sType;
//	//    const void*                     pNext;
//	//    VkShaderCreateFlagsEXT          flags;
//	//    VkShaderStageFlagBits           stage;
//	//    VkShaderStageFlags              nextStage;
//	//    VkShaderCodeTypeEXT             codeType;
//	//    size_t                          codeSize;
//	//    const void*                     pCode;
//	//    const char*                     pName;
//	//    uint32_t                        setLayoutCount;
//	//    const VkDescriptorSetLayout*    pSetLayouts;
//	//    uint32_t                        pushConstantRangeCount;
//	//    const VkPushConstantRange*      pPushConstantRanges;
//	//    const VkSpecializationInfo*     pSpecializationInfo;
//	//} VkShaderCreateInfoEXT;
//	//
//
//	//Create a Vulkan shader module from the compilation result
//	shader_CI.pNext    = VK_NULL_HANDLE;
//	shader_CI.codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT;
//	shader_CI.codeSize = shader_obj->GetBufferSize();
//	shader_CI.pCode    = static_cast<const void *>(shader_obj->GetBufferPointer());
//
//	VkShaderEXT shader;
//
//	VK_CHECK_RESULT(vkCreateShadersEXT(device_manager.GetLogicalDevice(), 1u, &shader_CI, VK_NULL_HANDLE, &shader));
//	return std::make_shared<VkShaderExtWrapper>(shader, shader_CI, path);
//}
//
}        // namespace Anni
