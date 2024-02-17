#pragma once
#include "AttachmentInfo.h"
#include "Buffer.h"
#include "Vk.h"
#include "VkTexture.h"

#include <memory>
#include <unordered_map>
#include <variant>
#include <vector>

// Base Usage Class

namespace Anni
{
	// Base Class
	class RsrcUsageInfoInPass
	{
	public:
		enum class RsrcOrigin
		{
			Unknown,
			FromOutSide,
			FromSourcePass,
			FromPreviousFrame,
			EstablishedInSitu
		};

	public:
		RsrcUsageInfoInPass(Vk::RsrcType rsrc_t_, Vk::RsrcAccessTypeRG access_t_);
		RsrcUsageInfoInPass(Vk::RsrcType rsrc_t_,
			Vk::RsrcAccessTypeRG access_t_,
			RsrcOrigin origin_);
		[[nodiscard]] Vk::RsrcType GetRsrcType() const;
		[[nodiscard]] Vk::RsrcAccessTypeRG GetRsrcAccessType() const;
		[[nodiscard]] RsrcOrigin GetRsrcOrigin() const;
		void SetRsrcOrigin(RsrcUsageInfoInPass::RsrcOrigin origin);

	public:
		RsrcUsageInfoInPass() = delete;

		RsrcUsageInfoInPass(const RsrcUsageInfoInPass&) = delete;
		RsrcUsageInfoInPass& operator=(const RsrcUsageInfoInPass&) = delete;

		RsrcUsageInfoInPass(RsrcUsageInfoInPass&&) = delete;
		RsrcUsageInfoInPass& operator=(RsrcUsageInfoInPass&&) = delete;

		virtual ~RsrcUsageInfoInPass() = 0;

	protected:
		Vk::RsrcType rsrc_type{};
		Vk::RsrcAccessTypeRG access_type{};
		RsrcOrigin origin{};
	};

	class VkTexUsageInfo
	{
	public:
		VkDescriptorType desc_type{};
		VkShaderStageFlags shader_stages{};
		Vk::DescSetInfo set_info{};

		std::shared_ptr<VkTexture> tex{};
	};

	class VkUniversalTexUsageInfoRG final : public RsrcUsageInfoInPass
	{
	public:
		// use as texture
		VkUniversalTexUsageInfoRG(
			VkFormat format_,
			std::variant<Vk::AttachmentIndix, Vk::DescSetInfo> binding_info_,

			VkAccessFlags access_mask_,
			VkPipelineStageFlags pip_stage_mask_,
			VkImageLayout layout_inpass_,

			VkDescriptorType type_,
			VkShaderStageFlags shader_stages_,
			Vk::RsrcAccessTypeRG access_t,
			std::optional<VkSamplerCreateInfo> sampler_CI_ = std::nullopt,
			std::optional<VkImageViewCreateInfo> img_view_CI_ = std::nullopt);

		// used for model texture importing
		VkUniversalTexUsageInfoRG(

			VkAccessFlags access_mask_,
			VkPipelineStageFlags pip_stage_mask_,
			VkImageLayout layout_inpass_,

			VkDescriptorType type_,
			VkShaderStageFlags shader_stages_);

		// use as attachment
		VkUniversalTexUsageInfoRG(
			VkFormat format_,
			std::variant<Vk::AttachmentIndix, Vk::DescSetInfo> binding_info_,

			VkAccessFlags access_mask_,
			VkPipelineStageFlags pip_stage_mask_,
			VkImageLayout layout_inpass_,

			AttachmentType attach_type_,
			VkClearValue clear_value_,
			Vk::RsrcAccessTypeRG access_t_,

			VkResolveModeFlagBits resolve_mode_ = VK_RESOLVE_MODE_NONE,
			VkImageLayout resolve_image_layout_ = VK_IMAGE_LAYOUT_UNDEFINED,
			// VkImageView              resolveImageView{};
			VkAttachmentLoadOp load_op_ = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			VkAttachmentStoreOp store_op_ = VK_ATTACHMENT_STORE_OP_DONT_CARE,

			std::optional<VkSamplerCreateInfo> sampler_CI_ = std::nullopt,
			std::optional<VkImageViewCreateInfo> img_view_CI_ = std::nullopt);

		// copy constructor
		VkUniversalTexUsageInfoRG(const VkUniversalTexUsageInfoRG& other) = delete;

		VkUniversalTexUsageInfoRG& operator=(const VkUniversalTexUsageInfoRG& other) = delete;
		VkUniversalTexUsageInfoRG(VkUniversalTexUsageInfoRG&& other) = delete;
		VkUniversalTexUsageInfoRG& operator=(VkUniversalTexUsageInfoRG&& other) = delete;
		~VkUniversalTexUsageInfoRG() override = default;

	public:
		// common ones
		VkFormat format{};
		std::optional<VkSamplerCreateInfo> sampler_CI{};
		std::optional<VkImageViewCreateInfo> img_view_CI{};
		std::variant<Vk::AttachmentIndix, Vk::DescSetInfo> binding_info;
		// for sync
		VkAccessFlags access_mask{}; // for sync
		VkPipelineStageFlags pip_stage_mask{}; // for sync
		VkImageLayout layout_inpass{}; // for sync
		// TODO: sub resource
		std::optional<VkImageSubresourceRange> subrange;

		// only for Tex usage
		VkDescriptorType tex_type{};
		VkShaderStageFlags shader_stages{};

		// only for Attach usage
		AttachmentType attach_type{ AttachmentType::Unknown };
		VkClearValue clear_value{};

		VkResolveModeFlagBits resolve_mode{};
		VkImageLayout resolve_image_layout{};
		// VkImageView         resolveImageView{};

		VkAttachmentLoadOp load_op;
		VkAttachmentStoreOp store_op;

		// some common functions
		std::optional<VkSamplerCreateInfo> GetSamplerCI() const;
		std::optional<VkImageViewCreateInfo> GetImgViewCI() const;
		[[nodiscard]] Vk::TexSyncInfo GetTexSynInfo() const;
	};

	class VkBufUsageInfoRG final : public RsrcUsageInfoInPass
	{
	public:
		[[nodiscard]] Vk::BufSyncInfo GetBufSynInfo() const;

	public:
		VkBufUsageInfoRG(Vk::DescSetInfo slot_info_,
			VkDescriptorType buf_type_,

			VkAccessFlags access_mask_,
			VkShaderStageFlags shader_staQueueges_,
			Vk::RsrcAccessTypeRG access_t_,
			std::optional<BufferSubRange> subrange_);

		// model usage
		VkBufUsageInfoRG(Vk::RsrcAccessTypeRG access_t_,
			std::optional<BufferSubRange> subrange_)
			:

			RsrcUsageInfoInPass(Vk::RsrcType::Buffer, access_t_)
			, subrange(subrange_)
		{
		}

		VkBufUsageInfoRG(const VkBufUsageInfoRG& other) = delete;

		VkBufUsageInfoRG& operator=(const VkBufUsageInfoRG& other) = delete;
		VkBufUsageInfoRG(VkBufUsageInfoRG&& other) = delete;
		VkBufUsageInfoRG& operator=(VkBufUsageInfoRG&& other) = delete;

		~VkBufUsageInfoRG() override = default;

		Vk::DescSetInfo set_info{};
		VkDescriptorType buf_type{};


		VkAccessFlags access_mask{};
		VkShaderStageFlags shader_stages{};

		std::optional<BufferSubRange> subrange;
	};

























	// class VkAttachmentInfo
	//{
	// public:

	//	class WithinPassRG : public RsrcUsageInfoInPass
	//	{
	//	public:
	//		WithinPassRG(const WithinPassRG& other) :
	//			RsrcUsageInfoInPass(other.rsrc_type, other.access_type,
	// other.origin), 			format(other.format), 			slot_info(other.slot_info),
	//			access_mask(other.access_mask),
	//			stage_mask(other.stage_mask),
	//			layout_inpass(other.layout_inpass),
	//			type(other.type),
	//			clear_value(other.clear_value),
	//			resolve_mode(other.resolve_mode),
	//			resolve_image_layout(other.resolve_image_layout),
	//			load_op(other.load_op),
	//			store_op(other.store_op),
	//			sampler_CI(other.sampler_CI),
	//			img_view_CI(other.img_view_CI)
	//		{
	//		}

	//		WithinPassRG& operator=(const WithinPassRG& other) = delete;
	//		WithinPassRG(WithinPassRG&& other) = delete;
	//		WithinPassRG& operator=(WithinPassRG&& other) = delete;

	//		WithinPassRG(
	//			Vk::RsrcType info_y, 			VkFormat format_,
	//			std::variant<Vk::AttachmentIndix, Vk::DescSetInfo>
	// slot_info_,

	//			VkAccessFlags                   access_mask_,
	//			VkPipelineStageFlags            stage_mask_,
	//			VkImageLayout                   layout_inpass_,
	//			AttachmentType type_,
	//			VkClearValue                    clear_value_,
	//			Vk::RsrcAccessTypeRG                  access_t,
	//			RsrcUsageInfoInPass::RsrcOrigin rsrc_origin_,
	//			VkResolveModeFlagBits           resolve_mode_ =
	// VK_RESOLVE_MODE_NONE, 			VkImageLayout                   resolve_image_layout_ =
	// VK_IMAGE_LAYOUT_UNDEFINED,
	//			//VkImageView              resolveImageView{};
	//			VkAttachmentLoadOp  load_op_ =
	// VK_ATTACHMENT_LOAD_OP_DONT_CARE, 			VkAttachmentStoreOp store_op_ =
	// VK_ATTACHMENT_STORE_OP_DONT_CARE) :

	//			RsrcUsageInfoInPass(info_y, access_t, rsrc_origin_),
	//			format(format_),
	//			slot_info(slot_info_),
	//			access_mask(access_mask_),
	//			stage_mask(stage_mask_),
	//			layout_inpass(layout_inpass_),
	//			type(type_),
	//			clear_value(clear_value_),
	//			resolve_mode(resolve_mode_),
	//			resolve_image_layout(resolve_image_layout_),
	//			load_op(load_op_),
	//			store_op(store_op_)

	//		{
	//		}

	//		~WithinPassRG() override = default;

	//		VkFormat format{};
	//		//uint32_t attachment_index{};

	//		std::variant<Vk::AttachmentIndix, Vk::DescSetInfo> slot_info;
	//		//VkAttachmentLoadOp  loadOp{};
	//		//VkAttachmentStoreOp storeOp{};

	//		VkAccessFlags        access_mask{};
	//		VkPipelineStageFlags stage_mask{};
	//		VkImageLayout        layout_inpass{};
	//		AttachmentType       type{ AttachmentType::Unknown };
	//		VkClearValue         clear_value{};

	//		VkResolveModeFlagBits resolve_mode{};
	//		VkImageLayout         resolve_image_layout{};
	//		//VkImageView              resolveImageView{};

	//		VkAttachmentLoadOp  load_op;
	//		VkAttachmentStoreOp store_op;

	//		std::optional<VkSamplerCreateInfo>   sampler_CI{};
	//		std::optional<VkImageViewCreateInfo> img_view_CI{};

	//		std::optional<VkSamplerCreateInfo> GetSamplerCI() const
	//		{
	//			return sampler_CI;
	//		}

	//		std::optional<VkImageViewCreateInfo> GetImgViewCI() const
	//		{
	//			return img_view_CI;
	//		}

	//		[[nodiscard]] Vk::TexSyncInfo GetTexSynInfo() const
	//		{
	//			return {
	//				.access_mask = access_mask,
	//				.stage_mask = stage_mask,
	//				.layout_inpass = layout_inpass };
	//		}
	//	};

	//	struct WithinPass
	//	{
	//		WithinPass(
	//			VkFormat               format_,
	//			uint32_t               attachment_index_,
	//			VkAttachmentLoadOp     loadOp_,
	//			VkAttachmentStoreOp    storeOp_,
	//			VkImageLayout          layout_prepass_,
	//			VkImageLayout          layout_inpass_,
	//			VkImageLayout          layout_afterpass_,
	//			AttachmentType type_,
	//			VkClearValue           clear_value_,

	//			VkResolveModeFlagBits resolveMode_ =
	// VK_RESOLVE_MODE_NONE,
	//			//VkImageView              resolveImageView{};
	//			VkImageLayout resolveImageLayout_ =
	// VK_IMAGE_LAYOUT_UNDEFINED) :

	//			format(format_),
	//			attachment_index(attachment_index_),
	//			loadOp(loadOp_),
	//			storeOp(storeOp_),
	//			layout_prepass(layout_prepass_),
	//			layout_inpass(layout_inpass_),
	//			layout_afterpass(layout_afterpass_),
	//			type(type_),
	//			clear_value(clear_value_),
	//			resolveMode(resolveMode_),
	//			//VkImageView              resolveImageView{};
	//			resolveImageLayout(resolveImageLayout_)

	//		{
	//		}

	//		WithinPass(const WithinPassRG& rhs) :
	//			format(rhs.format),
	//			attachment_index(std::get<Vk::AttachmentIndix>(rhs.slot_info)),
	//			loadOp(rhs.load_op),
	//			storeOp(rhs.store_op),
	//			layout_prepass(rhs.layout_inpass),
	//			layout_inpass(rhs.layout_inpass),
	//			layout_afterpass(rhs.layout_inpass),
	//			type(rhs.type),
	//			clear_value(rhs.clear_value),
	//			resolveMode(rhs.resolve_mode),
	//			//VkImageView              resolveImageView{};
	//			resolveImageLayout(rhs.resolve_image_layout)
	//		{
	//		}

	//		WithinPass(const VkUniversalTexUsageInfoRG& rhs) :
	//			format(rhs.format),
	//			attachment_index(std::get<Vk::AttachmentIndix>(rhs.binding_info)),
	//			loadOp(rhs.load_op),
	//			storeOp(rhs.store_op),
	//			layout_prepass(rhs.layout_inpass),
	//			layout_inpass(rhs.layout_inpass),
	//			layout_afterpass(rhs.layout_inpass),
	//			//type(rhs.attach_type),
	//			clear_value(rhs.clear_value),
	//			resolveMode(rhs.resolve_mode),
	//			//VkImageView              resolveImageView{};
	//			resolveImageLayout(rhs.resolve_image_layout)
	//		{
	//			switch (rhs.attach_type)
	//			{
	//			case Vk::AttachmentType::Unknown:
	//				type = AttachmentType::Unknown;
	//				break;

	//			case Vk::AttachmentType::ColorAttachment:
	//				type = AttachmentType::ColorAttachment;
	//				break;

	//			case Vk::AttachmentType::DepthAttachment:
	//				type = AttachmentType::DepthAttachment;
	//				break;

	//			case Vk::AttachmentType::DepthStencilAttachment:
	//				type = AttachmentType::DepthStencilAttachment;
	//				break;

	//			case Vk::AttachmentType::ResolveOpTargetAttachment:
	//				type =
	// AttachmentType::ResolveOpTargetAttachment; 				break;

	//			case Vk::AttachmentType::NotUsedWithinPass:
	//				type = AttachmentType::NotUsedWithinPass;
	//				break;

	//			default:
	//				break;
	//			}
	//		}

	//		VkFormat               format{};
	//		uint32_t               attachment_index{};
	//		VkAttachmentLoadOp     loadOp{};
	//		VkAttachmentStoreOp    storeOp{};
	//		VkImageLayout          layout_prepass{};
	//		VkImageLayout          layout_inpass{};
	//		VkImageLayout          layout_afterpass{};
	//		AttachmentType type{ AttachmentType::Unknown };
	//		VkClearValue           clear_value{};

	//		VkResolveModeFlagBits resolveMode{ VK_RESOLVE_MODE_NONE };
	//		//VkImageView              resolveImageView{};
	//		VkImageLayout resolveImageLayout{};
	//	};

	//	void SetResolveTargetTexImgView(const VkAttachmentInfo& resolve_target)
	//	{
	//		resolve_tex = resolve_target.tex;
	//		if (!info_in_pass.has_value())
	//		{
	//			throw std::runtime_error("no valid usage info!");
	//		}
	//		else
	//		{
	//			info_in_pass.value().resolveImageLayout =
	// resolve_target.GetInfo().layout_inpass;
	//		}
	//	}

	//	class Bundle
	//	{
	//	public:
	//		Bundle() = default;
	//		Bundle(std::vector<VkAttachmentInfo> info_array_,
	// DynamicRenderingAttachmentFormatInfo attachment_format_);
	//		[[nodiscard]] DynamicRenderingAttachmentFormatInfo
	// GetAttachmentFormatAndType() const; 		VkAttachmentInfo& operator[](size_t
	// index);

	//	private:
	//		std::vector<VkAttachmentInfo> info_array;
	//		DynamicRenderingAttachmentFormatInfo    attachment_format{};
	//	};

	// public:
	//	VkAttachmentInfo(const WithinPassRG& info, std::shared_ptr<VkTexture>
	// tex_); 	VkAttachmentInfo(const WithinPassRG& info, std::shared_ptr<VkTexture>
	// tex_, std::shared_ptr<VkTexture> resolve_tex_);

	//	VkAttachmentInfo(WithinPass meme_, std::shared_ptr<VkTexture> tex_);
	//	VkAttachmentInfo(WithinPass meme_, std::shared_ptr<VkTexture> tex_,
	// std::shared_ptr<VkTexture> resolve_tex_);

	//	VkAttachmentInfo() = default;

	//	[[nodiscard]] const WithinPass& GetInfo() const;

	//	[[nodiscard]] DynamicRenderingAttachmentFormatInfo
	// GetAttachmentFormatAndType() const;
	//	[[nodiscard]] VkRenderingAttachmentInfo  GetRenderingAttachmentInfo();
	//	void                                     SetMemento(WithinPass meme_);
	//	void SetTexture(std::shared_ptr<VkTexture> tex);
	//	[[nodiscard]] const VkTexture& GetTex() const;
	//	[[nodiscard]] VkTexture& GetTex();

	// public:
	//	//static Bundle GetAttachmentInfos(const Memento &meme_);
	//	static Bundle GetAttachmentInfos(const WithinPass& meme_, const
	// VkTexture::TexturePtrBundle& textures_); 	static Bundle
	// GetAttachmentInfos(const WithinPass& meme_, const VkTexture::TexturePtrBundle&
	// textures_, const VkTexture::TexturePtrBundle& resolve_textures_);

	// private:
	//	VkRenderingAttachmentInfo  rendering_attachment_info{}; //provided by
	// vulkan header, will be pass to VkBeginRendering funciton
	//	std::optional<WithinPass>  info_in_pass;
	//	std::shared_ptr<VkTexture> tex;
	//	std::shared_ptr<VkTexture> resolve_tex;
	// };

	// namespace DynamicRenderingAttachInfoGeneration
	//{
	//	template <typename T>
	//	inline void Process(
	//		std::vector<VkRenderingAttachmentInfo>& color_attachment_infos,
	//		VkRenderingAttachmentInfo& depth_attachment_info,
	//		VkRenderingAttachmentInfo& stensil_attachment_info,
	//		std::optional<size_t>                   img_index,
	//		T&);

	//	template <>
	//	inline void Process(
	//		std::vector<VkRenderingAttachmentInfo>& color_attachment_infos,
	//		VkRenderingAttachmentInfo& depth_attachment_info,
	//		VkRenderingAttachmentInfo& stensil_attachment_info,
	//		std::optional<size_t>                   img_index,
	//		VkAttachmentInfo& attach)
	//	{
	//		const auto attach_format_and_type =
	// attach.GetAttachmentFormatAndType(); 		if (AttachmentType::ColorAttachment ==
	// attach_format_and_type.attach_type)
	//		{
	//			color_attachment_infos.push_back(attach.GetRenderingAttachmentInfo());
	//		}
	//		if (AttachmentType::DepthAttachment ==
	// attach_format_and_type.attach_type || AttachmentType::DepthStencilAttachment
	//== attach_format_and_type.attach_type)
	//		{
	//			depth_attachment_info =
	// attach.GetRenderingAttachmentInfo();
	//		}
	//	}

	//	template <>
	//	inline void Process(
	//		std::vector<VkRenderingAttachmentInfo>& color_attachment_infos,
	//		VkRenderingAttachmentInfo& depth_attachment_info,
	//		VkRenderingAttachmentInfo& stensil_attachment_info,
	//		std::optional<size_t>                   img_index,
	//		VkAttachmentInfo::Bundle& attachment_bundle)
	//	{
	//		const auto attach_format_and_type =
	// attachment_bundle.GetAttachmentFormatAndType();

	//		if (AttachmentType::ColorAttachment ==
	// attach_format_and_type.attach_type)
	//		{
	//			color_attachment_infos.push_back(attachment_bundle[img_index.value()].GetRenderingAttachmentInfo());
	//		}

	//		if (AttachmentType::DepthAttachment ==
	// attach_format_and_type.attach_type || AttachmentType::DepthStencilAttachment
	//== attach_format_and_type.attach_type)
	//		{
	//			depth_attachment_info =
	// attachment_bundle[img_index.value()].GetRenderingAttachmentInfo();
	//		}
	//	}

	//	//recursion base
	//	template <typename T>
	//	void GenerateRenderingAttachInfo(
	//		std::vector<VkRenderingAttachmentInfo>& color_attachment_infos,
	//		VkRenderingAttachmentInfo& depth_attachment_info,
	//		VkRenderingAttachmentInfo& stensil_attachment_info,
	//		size_t                                  img_index,
	//		T&& first)
	//	{
	//		Process(color_attachment_infos, depth_attachment_info,
	// stensil_attachment_info, img_index, std::forward<T>(first)); 		return;
	//	}

	//	//function call
	//	template <typename First, typename... Rest>
	//	void GenerateRenderingAttachInfo(
	//		std::vector<VkRenderingAttachmentInfo>& color_attachment_infos,
	//		VkRenderingAttachmentInfo& depth_attachment_info,
	//		VkRenderingAttachmentInfo& stensil_attachment_info,
	//		size_t                                  img_index,
	//		First&& first,
	//		Rest &&...rest)
	//	{
	//		Process(color_attachment_infos, depth_attachment_info,
	// stensil_attachment_info, img_index, std::forward<First>(first)); 		return
	// GenerateRenderingAttachInfo(color_attachment_infos, depth_attachment_info,
	// stensil_attachment_info, img_index, std::forward<Rest>(rest)...);
	//	}

	//	//recursion base
	//	template <typename T>
	//	void GenerateRenderingAttachInfo(
	//		std::vector<VkRenderingAttachmentInfo>& color_attachment_infos,
	//		VkRenderingAttachmentInfo& depth_attachment_info,
	//		VkRenderingAttachmentInfo& stensil_attachment_info,
	//		T&& first)
	//	{
	//		Process(color_attachment_infos, depth_attachment_info,
	// stensil_attachment_info, std::nullopt, std::forward<T>(first)); 		return;
	//	}
	//	//function call
	//	template <typename First, typename... Rest>
	//	void GenerateRenderingAttachInfo(
	//		std::vector<VkRenderingAttachmentInfo>& color_attachment_infos,
	//		VkRenderingAttachmentInfo& depth_attachment_info,
	//		VkRenderingAttachmentInfo& stensil_attachment_info,
	//		First&& first,
	//		Rest &&...rest)
	//	{
	//		Process(color_attachment_infos, depth_attachment_info,
	// stensil_attachment_info, std::nullopt, std::forward<First>(first)); 		return
	// GenerateRenderingAttachInfo(color_attachment_infos, depth_attachment_info,
	// stensil_attachment_info, std::forward<Rest>(rest)...);
	//	}

	//	inline void GenerateRenderingAttachInfo(
	//		std::vector<VkRenderingAttachmentInfo>& color_attachment_infos,
	//		VkRenderingAttachmentInfo& depth_attachment_info,
	//		VkRenderingAttachmentInfo& stensil_attachment_info,
	//		std::vector<VkAttachmentInfo>& attachment_infos)
	//	{
	//		for (auto& attach_info : attachment_infos)
	//		{
	//			Process(color_attachment_infos, depth_attachment_info,
	// stensil_attachment_info, std::nullopt, attach_info);
	//		}
	//	}

	//}        // namespace DynamicRenderingAttachInfoGeneration

} // namespace Anni
