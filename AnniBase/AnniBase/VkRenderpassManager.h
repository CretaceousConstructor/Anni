#pragma once
//#include "DeviceManager.h"
//#include "BufferFactory.h"
//#include "TextureFactory.h"
//#include "VkFactoryBundle.h"
//#include "VkManagerBundle.h"
//#include "BufViewFactory.h"
//#include "VkSynObjectFactory.h"
//#include <unordered_map>
//
//namespace Anni
//{
//class RsrcManager
//{
//  public:
//	RsrcManager(GraphicsComponent &_gfx);
//	~RsrcManager() = default;
//
//	RsrcManager()                    = delete;
//	RsrcManager(const RsrcManager &) = delete;
//	RsrcManager &operator=(const RsrcManager &) = delete;
//	RsrcManager(RsrcManager &&)                 = delete;
//	RsrcManager &operator=(RsrcManager &&) = delete;
//
//	/**
//	 * \brief 返回没有状态记忆的工厂类对象
//	 * \return stateless factory
//	 */
//
//	const BufferFactory & GetBufferFactory();
//	const VkTextureFactory &GetTextureFactory();
//
//	const ImageFactory & GetImageFactory();
//	const VkShaderFactory &GetShaderFactory();
//
//	VkSynObjectFactory &GetSynOjectFactory();
//
//	const VkModelFactory &GetModelFactory();
//
//	const VkDescriptorSetFactory &GetDescriptorSetFactory();
//
//	VkTimelineSemPoolUnsafe &GetTimelineSemPool();
//
//  private:
//	GraphicsComponent &gfx;
//
//  private:
//	BufferFactory buffer_factory;
//	ImageFactory  image_factory;
//
//	ImgViewFactory img_view_factory;
//	BufViewFactoryUnsafe buf_view_factory;
//
//	VkTimelineSemPoolUnsafe tml_sem_pool;
//
//
//};
//
//class VkRenderpassManager
//{
//  public:
//	VkRenderpassManager(GraphicsComponent &_gfx);
//	~VkRenderpassManager() = default;
//
//	VkRenderpassManager()                            = delete;
//	VkRenderpassManager(const VkRenderpassManager &) = delete;
//	VkRenderpassManager &operator=(const VkRenderpassManager &) = delete;
//	VkRenderpassManager(VkRenderpassManager &&)                 = delete;
//	VkRenderpassManager &operator=(VkRenderpassManager &&) = delete;
//
//  public:
//	template <typename First, typename... Rest>
//	static std::shared_ptr<First> ProduceRenderpass(Rest &&...rest);
//
//  public:
//	void AddRenderPass(
//	    const std::string &name, uint32_t slot, const std::vector<VkAttachmentInfo> &attachments,
//	    const std::vector<VkSubpassDependency> &dependencies, const std::vector<std::shared_ptr<VkSubpassWrapper>> &subpasses);
//
//	void AddPipeline(const GFXPipelineCI &para_pack, const PipelineMetaInfo &meta_info);
//
//	void AddPipelineLayout(const PipelineLayoutMetaInfo &pipe_layout_info);
//
//  public:
//	[[nodiscard]] VkPipeline         GetPipeline(const PipelineMetaInfo &meta_info) const;
//	[[nodiscard]] VkPipelineLayout   GetPipelineLayout(const PipelineLayoutMetaInfo &meta_info) const;
//	[[nodiscard]] VkPipelineBuilder &GetPipelineBuilder();
//
//	[[nodiscard]] const std::vector<VkDescriptorSet> &GetDescriptorSetBundle(DescriptorSetMetaInfo meta_info) const;
//
//  public:
//  public:
//	VkDescriptorManager &  GetDescriptorManager();
//	VkDescriptorManagerV0 &GetDescriptorManagerV0();
//	VkPipelineManager &    GetPipelineManager();
//
//	/**
//	 * \brief 返回没有状态记忆的工厂类对象
//	 * \return stateless factory
//	 */
//	//const VkSubPassFacotry &       GetSubPassFactory();
//
//	const BufferFactory &GetBufferFactory();
//
//	const VkTextureFactory &GetTextureFactory();
//
//	VkSynObjectFactory &GetSynOjectFactory();
//
//	const ImageFactory &GetImageFactory();
//	const VkModelFactory &GetModelFactory();
//
//	const VkDescriptorSetFactory &GetDescriptorSetFactory();
//	const VkShaderFactory &       GetShaderFactory();
//
//  public:
//	VkManagerBundle &GetManagerBundle();
//	VkFactoryBundle &GetFactoryBundle();
//
//  private:
//	GraphicsComponent &gfx;
//
//  private:
//	VkManagerBundle managers;
//	VkFactoryBundle factories;
//
//	//VkObjectPoolBundle object_pools;
//
//  private:
//	//VkRenderpassWrapper &                             GetRenderpass(uint32_t pass);
//	//std::unordered_map<uint32_t, VkRenderpassWrapper> render_passes;
//	//std::unordered_map<uint32_t, std::string>         render_passes_names;
//};
//
//template <typename First, typename... Rest>
//std::shared_ptr<First> VkRenderpassManager::ProduceRenderpass(Rest &&...rest)
//{
//	std::shared_ptr<First> result = std::make_shared<First>(std::forward<Rest...>(rest...));
//	result->Init();
//	return result;
//}
//}        // namespace Anni
