#include "forwardPass.h"

namespace Cjing3D {

	ForwardPass::ForwardPass(SystemContext& context) :
		mContext(context)
	{
	}

	void ForwardPass::Render(World & world, XMMATRIX transform)
	{
		SetupFixedState();
	}

	void ForwardPass::SetupFixedState()
	{
	}

}