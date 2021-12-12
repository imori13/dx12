#pragma once

#include "PipelineStateObject.h"

namespace PipelineInitializer
{
	void Initialize();
	const PipelineStateObject& GetPipeline(std::wstring_view pipelineName);
};

