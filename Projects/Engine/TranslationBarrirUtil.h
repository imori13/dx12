#pragma once

constexpr D3D12_RESOURCE_BARRIER GetTranslationBarrier(gsl::not_null<ID3D12Resource*> resource, D3D12_RESOURCE_STATES oldState, D3D12_RESOURCE_STATES newState) noexcept
{
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = resource;
	barrier.Transition.StateBefore = oldState;
	barrier.Transition.StateAfter = newState;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	return barrier;
}