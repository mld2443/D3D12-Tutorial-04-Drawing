////////////////////////////////////////////////////////////////////////////////
// Filename: particlecomputeclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include "computepipelineinterface.h"


/////////////
// SHADERS //
/////////////
#include "particle.cs.h"


////////////////////////////////////////////////////////////////////////////////
// Interface name: ParticleComputeClass
////////////////////////////////////////////////////////////////////////////////
class ParticleComputeClass : public ComputePipelineInterface
{
public:
	ParticleComputeClass() = delete;
	ParticleComputeClass(const ParticleComputeClass&) = delete;
	ParticleComputeClass& operator=(const ParticleComputeClass&) = delete;

	ParticleComputeClass(ID3D12Device*);
	~ParticleComputeClass() = default;

	void SetPipelineParameters(UINT, float);

protected:
	void InitializeRootSignature(ID3D12Device*) override;

	void SetShaderBytecode() override;

	void NameD3D12Resources() override;

private:
};
