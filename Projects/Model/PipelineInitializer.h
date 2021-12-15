#pragma once

#include "PipelineStateObject.h"

namespace PipelineInitializer
{
	void Initialize(std::wstring_view vsName, std::wstring_view psName);
	const PipelineStateObject& GetPipeline(std::wstring_view pipelineName);
};

