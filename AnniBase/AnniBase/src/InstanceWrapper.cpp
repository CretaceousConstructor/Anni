#include "InstanceWrapper.h"

namespace Anni
{
	const std::string InstanceWrapper::app_name{ "DivineRapier" };
	const std::string InstanceWrapper::engine_name{ "Annihilation" };

	InstanceWrapper::InstanceWrapper(vk::raii::Context& context_) :
		context(context_)
	{
		InitWindowBackendSystem();
		CreateVulkanInstance();
		ValidationUtility::SetupDebugMessenger(*instance_refac,debug_messenger);
	}

	InstanceWrapper::~InstanceWrapper()
	{
		ShutDownWindowBackendSystem();
	}

	vk::Instance InstanceWrapper::GetInstanceReFac()
	{
		return *instance_refac;
	}

	void InstanceWrapper::InitWindowBackendSystem()
	{
		glfwSetErrorCallback(GlfwErrorCallback);//����glfw����ص�������
		glfwInit();        //glfw��ʼ����
	}

	void InstanceWrapper::ShutDownWindowBackendSystem()
	{
		glfwTerminate();
	}


	void InstanceWrapper::CreateVulkanInstance()
	{
		//��� ʵ��(instance) ���õ��� ȫ�ֲ�instance layer(�����Ѿ�������ȫ�ֲ���豸���ˣ�ȫ����ȫ�ֲ�)
		VULKAN_HPP_ASSERT(ValidationUtility::CheckIfRequiredInstanceLayersSupported(), "some instance layers required, but not available!");

		// initialize the vk::ApplicationInfo structure
		vk::ApplicationInfo applicationInfo(app_name.c_str(), 2, engine_name.c_str(), 1, ANNI_VK_API_VERSION);

		// initialize the vk::InstanceCreateInfo��дʵ��������Ϣ
		vk::InstanceCreateInfo instance_create_info({}, &applicationInfo);

		//����vulkanʵ��
			//1.����������Լ�����validation�����������Ϣ������Ҫ��һЩextensions
			//2.���glfw���õ���ʵ����չ ���Ҵ򿪣�
			//֮���vkcreateinstance���������ֵ

		//��� ʵ��instance ���õ��� ȫ����չ(instance extensions������û�м���豸��չ(device extension)��֮��Ż���)
		const auto extensions = ExtensionUtility::GetNeededGlobalInstanceExtensions(ValidationUtility::VALIDATION_LAYERS_ENABLED);

		instance_create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		instance_create_info.ppEnabledExtensionNames = extensions.data();
		//*********************************************************************


		vk::ValidationFeaturesEXT validation_feature_ext;

		validation_feature_ext.enabledValidationFeatureCount = static_cast<uint32_t>(ValidationUtility::enabled_validation_features.size());
		validation_feature_ext.pEnabledValidationFeatures = ValidationUtility::enabled_validation_features.data();

		validation_feature_ext.disabledValidationFeatureCount = static_cast<uint32_t>(ValidationUtility::disabled_validation_features.size());
		validation_feature_ext.pDisabledValidationFeatures = ValidationUtility::disabled_validation_features.data();



		vk::DebugUtilsMessengerCreateInfoEXT debug_create_info{};

		std::vector<const char*> RequiredInstanceLayer;
		//��֤����Ҫ��instance layer
		if (ValidationUtility::VALIDATION_LAYERS_ENABLED)
		{
			//��� ʵ��validation ���õ��� ��(layer)�����Ҵ�
			RequiredInstanceLayer.insert(RequiredInstanceLayer.end(), ValidationUtility::required_validation_instance_layers.begin(), ValidationUtility::required_validation_instance_layers.end());

			//����Ĵ�����Ϊ�˲���CreateDebugUtilsMessengerEXT��DestroyDebugUtilsMessengerEXT�����validation��Ϣ
			ValidationUtility::PopulateDebugMessengerCreateInfo(debug_create_info);
			validation_feature_ext.pNext = &debug_create_info;                      //������ֵpNext�Ϳ������CreateDebugUtilsMessengerEXT��DestroyDebugUtilsMessengerEXT�����validation��Ϣ
			instance_create_info.pNext = &validation_feature_ext;
		}


		instance_create_info.enabledLayerCount = static_cast<uint32_t>(RequiredInstanceLayer.size());
		instance_create_info.ppEnabledLayerNames = RequiredInstanceLayer.data();

		instance_refac = vk::createInstanceUnique(instance_create_info);
	}

	void InstanceWrapper::GlfwErrorCallback(int error, const char* description)
	{
		fprintf(stderr, "GLFW ERROR %d: %s\n", error, description);
	}

}

