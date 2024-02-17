#include "DeviceManager.h"
namespace Anni
{
	DeviceManager::DeviceManager(InstanceWrapper& instance_wrapper_, WindowsSys& window_) :
		instance_wrapper(instance_wrapper_),
		window(window_)
	{

		PickOnePhysicalDevice();
		CreateLogicalDevice();
	}

	//DeviceManager::~DeviceManager()
	//{
	//	for (const auto& cp : command_pools)
	//	{
	//		vkDestroyCommandPool(device, cp, nullptr);
	//	}
	//	vkDestroyDevice(device, nullptr);
	//}

	void DeviceManager::PickOnePhysicalDevice()
	{
		vk::Instance instance = instance_wrapper.GetInstanceReFac();
		std::vector<vk::PhysicalDevice> devices = instance.enumeratePhysicalDevices();

		for (const auto& d : devices)
		{
			if (DeviceManager::IsDeviceSuitable(d, window.GetSurface()))
			{
				physical_device = d;
				break;
			}
		}

		VULKAN_HPP_ASSERT(physical_device, "failed to find a suitable GPU!");

		// Use an ordered map to automatically sort candidates by increasing score
		// Ŀǰ��ʱûʹ������ϵͳ��
		/*
			std::multimap<int, VkPhysicalDevice> candidates;
			for (const auto &device : devices)
			{
				int score = RateDeviceSuitability(device);
				candidates.insert(std::make_pair(score, device));
			}
		 */
	}


	int DeviceManager::RateDeviceSuitability(const VkPhysicalDevice& device_)
	{
		VkPhysicalDeviceProperties deviceProperties;        //������������������֧�ֵ�vulkan�汾
		vkGetPhysicalDeviceProperties(device_, &deviceProperties);

		VkPhysicalDeviceFeatures deviceFeatures;        //����ѹ����64λfloat�����ӿ�
		vkGetPhysicalDeviceFeatures(device_, &deviceFeatures);

		int score = 0;

		// Discrete GPUs have a significant performance advantage
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			score += 1000;
		}

		// Maximum possible size of textures affects graphics quality
		score += deviceProperties.limits.maxImageDimension2D;

		// Application can't function without geometry shaders
		if (!deviceFeatures.geometryShader)
		{
			return 0;
		}

		return score;
	}

	bool DeviceManager::QueueFamilyIndices::IsComplete() const
	{
		return graphics_family.has_value() && present_family.has_value() && transfer_family.has_value();
	}

	DeviceManager::QueueFamilyIndices DeviceManager::FindQueueFamilies(vk::PhysicalDevice device, const vk::SurfaceKHR surface)
	{
		QueueFamilyIndices indices;
		std::vector<vk::QueueFamilyProperties> queue_family_properties = device.getQueueFamilyProperties();

		//typedef struct VkQueueFamilyProperties {
		//	VkQueueFlags    queueFlags;
		//	uint32_t        queueCount;
		//	uint32_t        timestampValidBits;
		//	VkExtent3D      minImageTransferGranularity;
		//} VkQueueFamilyProperties;

		/*for (auto& shit : queueFamilies) {

			std::cout << "queueFlags"						  << " "        << shit.queueFlags << std::endl;
			std::cout << "queueCount"						  << " "        << shit.queueCount << std::endl;
			std::cout << "timestampValidBits"				  << " "		<< shit.timestampValidBits << std::endl;
			std::cout << "minImageTransferGranularity.depth " << " "		<< shit.minImageTransferGranularity.depth << std::endl;
			std::cout << "minImageTransferGranularity.height" << " "		<< shit.minImageTransferGranularity.height << std::endl;
			std::cout << "minImageTransferGranularity.width " << " "		<< shit.minImageTransferGranularity.width << std::endl;

			std::cout << std::endl;
		}*/


		int i = 0;
		for (const auto& qf : queue_family_properties)
		{
			const VkBool32 present_support = device.getSurfaceSupportKHR(i, surface);

			if (qf.queueFlags & vk::QueueFlagBits::eGraphics)
			{        //������м�������Ҫ֧��ͼ�β���
				if (!indices.graphics_family.has_value())
				{
					indices.graphics_family = i;
				}
			}
			if (present_support)
			{
				if (!indices.present_family.has_value())
				{
					indices.present_family = i;
				}
			}

			//�ҵ�����ר����transfer��û����graphics��family index
			if ((qf.queueFlags & vk::QueueFlagBits::eTransfer) && !(qf.queueFlags & vk::QueueFlagBits::eGraphics))
			{
				if (!indices.transfer_family.has_value())
				{
					indices.transfer_family = i;
				}
			}

			if (indices.IsComplete())
			{
				//3�ֶ����Ժ�Ϳ����� break��
				break;
			}
			i++;
		}
		return indices;
	}


	bool DeviceManager::IsDeviceSuitable(const vk::PhysicalDevice phy_device, const vk::SurfaceKHR surface)
	{

		QueueFamilyIndices indices = FindQueueFamilies(phy_device, surface);        //

		//��������豸���豸��չ֧�����
		bool extensions_supported = CheckIfDeviceExtensionSupported(phy_device);        //��Ҫ�����ܲ�����swapchain

		//TODO:��������豸��features��֧�����
		bool                     swap_chain_adequate = false;
		vk::PhysicalDeviceFeatures supported_features = phy_device.getFeatures();

		VULKAN_HPP_ASSERT(extensions_supported);
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(phy_device, surface);        //ѯ�ʵ�������device
		swap_chain_adequate = !swapChainSupport.formats.empty() && !swapChainSupport.present_modes.empty();

		const bool enough_depth_formats_to_use = CheckIfRequiredDepthFomatAndFeaturesSupported(phy_device);

		/*typedef struct VkSurfaceCapabilitiesKHR {
			uint32_t                         minImageCount; 2
			����֧�ֵ�swapchain��ͼƬ(buffer)����
			uint32_t                         maxImageCount; 8
			���֧�ֵ�swapchain��ͼƬ(buffer)����
			VkExtent2D                       currentExtent; 320 258  the current width and height of the surface,
			VkExtent2D                       minImageExtent;320 258  the smallest valid swapchain extent for the surface on the specified device.
			VkExtent2D                       maxImageExtent;320 258  the largest  valid swapchain extent for the surface on the specified device
			uint32_t                         maxImageArrayLayers;1   һ���������Ļչʾ�� presentable images ����
			VkSurfaceTransformFlagsKHR       supportedTransforms;1   ����Ļչʾ��ʱ����Խ��еĲ��������緭ת,����˵��ʲô����Ҳ������
			VkSurfaceTransformFlagBitsKHR    currentTransform; VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
			VkCompositeAlphaFlagsKHR         supportedCompositeAlpha; 1 ֱ�Ӻ���alpha channel
			VkImageUsageFlags                supportedUsageFlags; 9f

				VK_IMAGE_USAGE_TRANSFER_SRC_BIT = 0x00000001,  //������Ϊת��ָ���Դͷ
				VK_IMAGE_USAGE_TRANSFER_DST_BIT = 0x00000002,  //������Ϊת��ָ���Ŀ��
				VK_IMAGE_USAGE_SAMPLED_BIT = 0x00000004,       //������������VkImageView Ȼ��shader����
				VK_IMAGE_USAGE_STORAGE_BIT = 0x00000008,       //������������VkImageView Ȼ����ΪĿ�꣬���洢
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT = 0x00000010,  //������������VkImageView��Ȼ����Ϊ��ɫ�����԰󶨵�frame buffer��
				VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT = 0x00000080,  //������������VkImageView��Ȼ����Ա�shader��ȡ��������Ϊframbuffer�ĸ���
		} VkSurfaceCapabilitiesKHR;*/
		//�ǵø������Բ����Ĺ���

		return indices.IsComplete() && extensions_supported && swap_chain_adequate && supported_features.samplerAnisotropy && enough_depth_formats_to_use;
	}

	bool DeviceManager::CheckIfDeviceExtensionSupported(vk::PhysicalDevice device)
	{
		//���� �豸��չ����
		std::vector<vk::ExtensionProperties> available_extensions = device.enumerateDeviceExtensionProperties();
		const auto& device_required_extensions = ExtensionUtility::GetRequiredExtensionsForAGoodDevice();

		std::set<std::string> required_extensions(device_required_extensions.begin(), device_required_extensions.end());
		for (const auto& extension : available_extensions)
		{
			required_extensions.erase(extension.extensionName);
		}

		return required_extensions.empty();        //����ǿյģ��������Ҫ���豸��չ���ܶ���
	}

	bool DeviceManager::CheckIfRequiredDepthFomatAndFeaturesSupported(vk::PhysicalDevice phy_device)
	{
		//// Provided by VK_VERSION_1_0
		//typedef struct VkFormatProperties {
		//	VkFormatFeatureFlags    linearTilingFeatures;
		//	VkFormatFeatureFlags    optimalTilingFeatures;
		//	VkFormatFeatureFlags    bufferFeatures;
		//} VkFormatProperties;

		constexpr static vk::Format required_depth_formats[]{
			vk::Format::eD32Sfloat,
			vk::Format::eD32SfloatS8Uint,
			vk::Format::eD24UnormS8Uint
		};

		constexpr vk::FormatFeatureFlags required_depth_format_feature{ vk::FormatFeatureFlagBits::eDepthStencilAttachment };

		bool result = true;
		for (const vk::Format format : required_depth_formats)
		{
			vk::FormatProperties2 props = phy_device.getFormatProperties2(format);

			//if ((props.linearTilingFeatures & Vk::required_depth_format_feature) == Vk::required_depth_format_feature &&
			//    ((props.optimalTilingFeatures & Vk::required_depth_format_feature) == Vk::required_depth_format_feature))
			if ((props.formatProperties.optimalTilingFeatures & required_depth_format_feature) == required_depth_format_feature)
			{
			}
			else
			{
				result = false;
			}
		}

		return result;
	}

	//���������swapchainmanagerҲ��Ҫ������Ϊ�˷�ֹcircular depen��ֻ���ظ�һ��
	DeviceManager::SwapChainSupportDetails DeviceManager::QuerySwapChainSupport(vk::PhysicalDevice phy_device, vk::SurfaceKHR surface)
	{
		SwapChainSupportDetails details;
		details.capabilities = phy_device.getSurfaceCapabilities2KHR(surface);
		//֧�ֵ�backbuffer��ʽ
		//colorSpace
		//VK_COLOR_SPACE_SRGB_NONLINEAR_KHR = 0,

		//format;
		//VK_FORMAT_B8G8R8A8_UNORM = 44,
		//VK_FORMAT_B8G8R8A8_SRGB = 50,
		//VK_FORMAT_A2B10G10R10_UNORM_PACK32 = 64
		details.formats = phy_device.getSurfaceFormats2KHR(surface);
		//���ܵ�չʾ��ģʽ
		//VK_PRESENT_MODE_FIFO_KHR = 2,
		//VK_PRESENT_MODE_FIFO_RELAXED_KHR = 3,
		//VK_PRESENT_MODE_MAILBOX_KHR = 1,
		//VK_PRESENT_MODE_IMMEDIATE_KHR = 0,
		details.present_modes = phy_device.getSurfacePresentModes2EXT(surface);

		return details;
	}

	uint32_t DeviceManager::FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties, const vk::PhysicalDevice para_physical_device)
	{
		vk::PhysicalDeviceMemoryProperties2 memProperties2 = para_physical_device.getMemoryProperties2();
		const auto& memProperties = memProperties2.memoryProperties;

		for (uint32_t mem_type_index = 0; mem_type_index < memProperties.memoryTypeCount; mem_type_index++)
		{
			if ((typeFilter & (1 << mem_type_index)) && ((memProperties.memoryTypes[mem_type_index].propertyFlags & properties) == properties))
			{
				return mem_type_index;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}



	vk::Bool32 DeviceManager::FormatIsFilterable(vk::Format format, vk::ImageTiling tiling) const
	{
		vk::FormatProperties2 formatProps = physical_device.getFormatProperties2(format);

		if (tiling == vk::ImageTiling::eOptimal)
			return static_cast<vk::Bool32>(formatProps.formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear);

		if (tiling == vk::ImageTiling::eLinear)
			return static_cast<vk::Bool32>(formatProps.formatProperties.linearTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear);

		return false;
	}























	//VkCommandPool DeviceManager::CreateCommandPool(CommandPoolType type)
	//{
	//	const QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(physical_device, window.GetSurface());
	//
	//	switch (type)
	//	{
	//		case CommandPoolType::graphics_command_pool: {
	//			VkCommandPool           graphicsCommandPool;
	//			VkCommandPoolCreateInfo graphics_command_pool_CI{};
	//			graphics_command_pool_CI.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	//			graphics_command_pool_CI.queueFamilyIndex = queueFamilyIndices.graphics_family.value();
	//			graphics_command_pool_CI.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;        // Optional
	//
	//			VK_CHECK_RESULT(vkCreateCommandPool(device, &graphics_command_pool_CI, nullptr, &graphicsCommandPool))
	//			command_pools.push_back(graphicsCommandPool);
	//			return command_pools.back();
	//		}
	//		case CommandPoolType::transfor_command_pool: {
	//			VkCommandPool           transforCommandPool;
	//			VkCommandPoolCreateInfo transfer_command_pool_CI{};
	//			transfer_command_pool_CI.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	//			transfer_command_pool_CI.queueFamilyIndex = queueFamilyIndices.transfer_family.value();
	//			//transforPoolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	//			transfer_command_pool_CI.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;        // �������ڶ��ݵ�ʹ�� ���ҿ��Ը���
	//			VK_CHECK_RESULT(vkCreateCommandPool(device, &transfer_command_pool_CI, nullptr, &transforCommandPool))
	//			command_pools.push_back(transforCommandPool);
	//			return command_pools.back();
	//		}
	//
	//		default: /* ��ѡ�� */
	//			throw std::runtime_error("failed to create command pool!");
	//	}
	//}

	//DeviceManager::VkExecutionQueue &DeviceManager::GetSuitableQueue(QueueCapability required_queue_cap) const
	//{
	//	std::vector<std::vector<VkExecutionQueue>::iterator> suitable_queue;
	//	for (auto s = queues.begin(); s != queues.end(); ++s)
	//	{
	//		auto cur_queue_cap = s->GetQueueCap();
	//		if (required_queue_cap.graphics && !cur_queue_cap.graphics)
	//		{
	//			continue;
	//		}
	//		if (required_queue_cap.compute && !cur_queue_cap.compute)
	//		{
	//			continue;
	//		}
	//		if (required_queue_cap.transfer && !cur_queue_cap.transfer)
	//		{
	//			continue;
	//		}
	//		if (required_queue_cap.present && !cur_queue_cap.present)
	//		{
	//			continue;
	//		}
	//		suitable_queue.push_back(s);
	//	}
	//
	//	std::sort(suitable_queue.begin(), suitable_queue.end(),
	//	          [](const std::vector<DeviceManager::VkExecutionQueue>::iterator &rhs, const std::vector<DeviceManager::VkExecutionQueue>::iterator &lhs) {
	//		          rhs->GetNumDisbatchedPasses() < lhs->GetNumDisbatchedPasses();
	//	          });
	//
	//	std::vector<VkExecutionQueue>::iterator result = *suitable_queue.begin();
	//	return *result;
	//}
	//
	//DeviceManager::VkExecutionQueue &DeviceManager::GetSuitableQueue2(QueueCapability required_queue_cap) const
	//{
	//	std::vector<std::vector<VkExecutionQueue>::iterator> suitable_queue;
	//	for (auto s = queues.begin(); s != queues.end(); ++s)
	//	{
	//		auto cur_queue_cap = s->GetQueueCap();
	//		if (required_queue_cap.graphics && !cur_queue_cap.graphics)
	//		{
	//			continue;
	//		}
	//		if (required_queue_cap.compute && !cur_queue_cap.compute)
	//		{
	//			continue;
	//		}
	//		if (required_queue_cap.transfer && !cur_queue_cap.transfer)
	//		{
	//			continue;
	//		}
	//		if (required_queue_cap.present && !cur_queue_cap.present)
	//		{
	//			continue;
	//		}
	//		suitable_queue.push_back(s);
	//	}
	//
	//	//std::sort(suitable_queue.begin(), suitable_queue.end(),
	//	//	[](const std::vector<DeviceManager::VkExecutionQueue>::iterator& rhs,const std::vector<DeviceManager::VkExecutionQueue>::iterator& lhs)
	//	//	{
	//	//		rhs->GetNumDisbatchedPasses() < lhs->GetNumDisbatchedPasses();
	//	//	}
	//	//);
	//
	//	std::ranges::sort(suitable_queue, [](const auto &rhs, const auto &lhs) {
	//		return rhs->GetNumDisbatchedPasses() < lhs->GetNumDisbatchedPasses();
	//	});
	//
	//	std::vector<VkExecutionQueue>::iterator result = *suitable_queue.begin();
	//	return *result;
	//}
	//

	void DeviceManager::CreateLogicalDevice()
	{
		//���м��崴����Ϣ
		std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;

		//�������ԣ����������ܶ��queue
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		queue_families_props = physical_device.getQueueFamilyProperties2();

		constexpr float queue_priority = 1.0f;
		for (const auto& [index, queue_families_prop] : std::ranges::views::enumerate(queue_families_props))
		{
			//ÿ�ֶ��м��崴�������ܶ��queue��֮���Ӹ�Queue Managerȥ����
			vk::DeviceQueueCreateInfo queue_create_info(
				vk::DeviceQueueCreateFlags(VK_ZERO_FLAG),
				index,
				queue_families_prop.queueFamilyProperties.queueCount,
				&queue_priority
			);
			queue_create_infos.push_back(queue_create_info);
		}
		//*********************************************************************
		vk::StructureChain<vk::PhysicalDeviceVulkan11Features, vk::PhysicalDeviceVulkan12Features, vk::PhysicalDeviceVulkan13Features> enabled_gpu_features_chain;
		vk::PhysicalDeviceFeatures          enabled_gpu_features = {};        // vulkan 1.0
		vk::PhysicalDeviceVulkan11Features& enabled11_gpu_features = enabled_gpu_features_chain.get<vk::PhysicalDeviceVulkan11Features>();        // vulkan 1.1
		vk::PhysicalDeviceVulkan12Features& enabled12_gpu_features = enabled_gpu_features_chain.get<vk::PhysicalDeviceVulkan12Features>();        // vulkan 1.2
		vk::PhysicalDeviceVulkan13Features& enabled13_gpu_features = enabled_gpu_features_chain.get<vk::PhysicalDeviceVulkan13Features>();        // vulkan 1.3

		//����ֱ��Ӳ�����Ƿ���һЩ������Ҳ����ͨ��������ѯ ĳЩfeature�Ƿ�device֧�֡�
		//TODO:��ѯ ĳЩfeature�Ƿ�device֧�֡�
		//If the VkPhysicalDeviceVulkan12Features structure is included in the pNext chain of the VkPhysicalDeviceFeatures2 structure passed to vkGetPhysicalDeviceFeatures2, it is filled in to indicate whether each corresponding feature is supported. VkPhysicalDeviceVulkan12Features can also be used in the pNext chain of VkDeviceCreateInfo to selectively enable these features.


		// Enable gpu features 1.0 here.
		enabled_gpu_features.samplerAnisotropy = VK_TRUE;
		enabled_gpu_features.depthClamp = VK_TRUE;
		enabled_gpu_features.shaderSampledImageArrayDynamicIndexing = VK_TRUE;
		enabled_gpu_features.multiDrawIndirect = VK_TRUE;
		enabled_gpu_features.drawIndirectFirstInstance = VK_TRUE;
		enabled_gpu_features.independentBlend = VK_TRUE;
		enabled_gpu_features.multiViewport = VK_TRUE;
		enabled_gpu_features.fragmentStoresAndAtomics = VK_TRUE;
		enabled_gpu_features.geometryShader = VK_TRUE;
		enabled_gpu_features.sampleRateShading = VK_TRUE;

		// Enable gpu features 1.1 here.   ����chain���ӵĿ�ͷ
		enabled11_gpu_features.pNext = &enabled12_gpu_features;
		enabled11_gpu_features.shaderDrawParameters = VK_TRUE;
		enabled11_gpu_features.multiview = VK_TRUE;

		// Enable gpu features 1.2 here.
		enabled12_gpu_features.drawIndirectCount = VK_TRUE;
		enabled12_gpu_features.imagelessFramebuffer = VK_TRUE;
		enabled12_gpu_features.separateDepthStencilLayouts = VK_TRUE;
		enabled12_gpu_features.descriptorIndexing = VK_TRUE;
		enabled12_gpu_features.runtimeDescriptorArray = VK_TRUE;
		enabled12_gpu_features.descriptorBindingPartiallyBound = VK_TRUE;
		enabled12_gpu_features.descriptorBindingVariableDescriptorCount = VK_TRUE;
		enabled12_gpu_features.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
		enabled12_gpu_features.descriptorBindingUpdateUnusedWhilePending = VK_TRUE;
		enabled12_gpu_features.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
		enabled12_gpu_features.timelineSemaphore = VK_TRUE;

		enabled12_gpu_features.shaderOutputViewportIndex = VK_TRUE;
		enabled12_gpu_features.shaderOutputLayer = VK_TRUE;

		enabled12_gpu_features.pNext = &enabled13_gpu_features;

		// Enable gpu features 1.3 here.
		enabled13_gpu_features.pNext = nullptr;        // todo: vulkan 1.4
		enabled13_gpu_features.dynamicRendering = VK_TRUE;
		enabled13_gpu_features.shaderDemoteToHelperInvocation = VK_TRUE;

		//*********************************************************************
		//*********************************************************************
		vk::PhysicalDeviceFeatures2 physicalDeviceFeatures2(enabled_gpu_features, &enabled_gpu_features_chain);
		vk::DeviceCreateInfo device_create_info;

		device_create_info.setQueueCreateInfos(queue_create_infos);
		device_create_info.setPEnabledFeatures(VK_NULL_HANDLE);
		device_create_info.setPNext(&physicalDeviceFeatures2);
		device_create_info.setPEnabledExtensionNames(ExtensionUtility::GetRequiredExtensionsForAGoodDevice());

		//VkDeviceCreateInfo.enabledLayerCount is deprecated and ignored.
		//VkDeviceCreateInfo.ppEnabledLayerNames is deprecatedand ignored.

		device = physical_device.createDeviceUnique(device_create_info);

	}

	vk::Device DeviceManager::GetLogicalDevice()
	{
		return *device;
	}

	vk::PhysicalDevice DeviceManager::GetPhysicalDevice()
	{
		return physical_device;
	}


	const std::vector<vk::QueueFamilyProperties2>& DeviceManager::GetQueueFamilyProps() const
	{
		return queue_families_props;
	}

	vk::Format DeviceManager::FindSupportedFormat(const std::vector<vk::Format> candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) const
	{
		//// Provided by VK_VERSION_1_0
		//typedef struct VkFormatProperties {
		//	VkFormatFeatureFlags    linearTilingFeatures;
		//	VkFormatFeatureFlags    optimalTilingFeatures;
		//	VkFormatFeatureFlags    bufferFeatures;
		//} VkFormatProperties;

		for (const vk::Format& format : candidates)
		{
			vk::FormatProperties2 props = physical_device.getFormatProperties2(format);

			if (tiling == vk::ImageTiling::eLinear && (props.formatProperties.linearTilingFeatures & features) == features)
			{
				return format;
			}
			else if (tiling == vk::ImageTiling::eOptimal && (props.formatProperties.optimalTilingFeatures & features) == features)
			{
				return format;
			}
		}
		
		VULKAN_HPP_ASSERT(false, "failed to find supported format!");
	}

	//void DeviceManager::CreateBufferAndBindToMemo(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory, VkSharingMode sharingmode, const VkSurfaceKHR &surface) const
	//{
	//	/*typedef struct VkBufferCreateInfo {
	//		VkStructureType        sType;
	//		const void* pNext;
	//		VkBufferCreateFlags    flags;
	//		VkDeviceSize           size;
	//		VkBufferUsageFlags     usage;
	//		VkSharingMode          sharingMode;
	//		uint32_t               queueFamilyIndexCount;
	//		const uint32_t* pQueueFamilyIndices;
	//	} VkBufferCreateInfo;*/
	//
	//	VkBufferCreateInfo bufferInfo{};
	//	bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	//	bufferInfo.size        = size;
	//	bufferInfo.usage       = usage;
	//	bufferInfo.sharingMode = sharingmode;
	//
	//	const QueueFamilyIndices indices            = FindQueueFamilies(physical_device, surface);
	//	const std::set<uint32_t> queueFamilyIndices = {indices.graphics_family.value(), indices.present_family.value(), indices.transfer_family.value()};
	//
	//	std::vector<uint32_t> uniqueQueueFamilyIndices;
	//	for (auto index : queueFamilyIndices)
	//	{
	//		uniqueQueueFamilyIndices.push_back(index);
	//	}
	//
	//	if (sharingmode == VK_SHARING_MODE_CONCURRENT)
	//	{
	//		bufferInfo.pQueueFamilyIndices   = uniqueQueueFamilyIndices.data();
	//		bufferInfo.queueFamilyIndexCount = static_cast<uint32_t>(uniqueQueueFamilyIndices.size());
	//	}
	//
	//	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
	//	{
	//		throw std::runtime_error("failed to create buffer!");
	//	}
	//
	//	VkMemoryRequirements memRequirements;
	//	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);
	//
	//	VkMemoryAllocateInfo allocInfo{};
	//	allocInfo.sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	//	allocInfo.allocationSize = memRequirements.size;
	//
	//	//if (size != memRequirements.size)
	//	//{
	//	//	throw std::runtime_error("memRequirements size differs from size parameter!");
	//	//}
	//
	//	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties, physical_device);
	//
	//	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
	//	{
	//		throw std::runtime_error("failed to allocate buffer memory!");
	//	}
	//
	//	vkBindBufferMemory(device, buffer, bufferMemory, 0);
	//}

	//void DeviceManager::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, const VkCommandBuffer &transfer_command_buffer) const
	//{
	//	VkCommandBufferBeginInfo beginInfo{};
	//	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	//	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	//
	//	vkBeginCommandBuffer(transfer_command_buffer, &beginInfo);
	//
	//	VkBufferCopy copyRegion{};
	//	copyRegion.srcOffset = 0;        // Optional
	//	copyRegion.dstOffset = 0;        // Optional
	//	copyRegion.size      = size;
	//	vkCmdCopyBuffer(transfer_command_buffer, srcBuffer, dstBuffer, 1, &copyRegion);
	//
	//	vkEndCommandBuffer(transfer_command_buffer);
	//
	//	VkSubmitInfo submitInfo{};
	//	submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	//	submitInfo.commandBufferCount = 1;
	//	submitInfo.pCommandBuffers    = &transfer_command_buffer;
	//
	//	vkQueueSubmit(tranfer_queue, 1, &submitInfo, VK_NULL_HANDLE);
	//	vkQueueWaitIdle(tranfer_queue);
	//}
	//
	//



}        // namespace Anni
