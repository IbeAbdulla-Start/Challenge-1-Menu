#pragma once


#include "NOU/Entity.h"
#include "Tools/PathUtility.h"

namespace nou
{
	class CPathAnimator
	{
	public:

		//How long it should take us to travel along each segment.
		float m_segmentTravelTime;

		CPathAnimator(Entity& owner);
		~CPathAnimator() = default;

		void SetMode(PathSampler::PathMode mode);
		void Update(const PathSampler::KeypointSet& keypoints, float deltaTime);

	private:

		Entity* m_owner;
		float m_segmentTimer;
		size_t m_segmentIndex;
		PathSampler::PathMode m_mode;
	};
}