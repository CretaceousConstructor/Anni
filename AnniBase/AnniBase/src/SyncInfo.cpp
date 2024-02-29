#include "SyncInfo.h"

Anni::SemInsertInfoSafe::SemInsertInfoSafe(WaitValue wait_val_, vk::PipelineStageFlags2 wait_stages_):
	wait_val(wait_val_), wait_stages(wait_stages_)
{
}

Anni::SemInsertInfoSafe::SemInsertInfoSafe(SignalValue signal_val_, vk::PipelineStageFlags2 signal_stages_):
	signal_val(signal_val_), signal_stages(signal_stages_)
{
}

Anni::SemInsertInfoSafe::SemInsertInfoSafe(SignalValue signal_val_, vk::PipelineStageFlags2 signal_stages_,
	WaitValue wait_val_, vk::PipelineStageFlags2 wait_stages_):
	wait_val(wait_val_), wait_stages(wait_stages_),
	signal_val(signal_val_), signal_stages(signal_stages_)
{
}
