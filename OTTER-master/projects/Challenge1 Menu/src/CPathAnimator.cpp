#include "CPathAnimator.h"

namespace nou
{
	CPathAnimator::CPathAnimator(Entity& owner)
	{
		m_owner = &owner;

		m_segmentIndex = 0;
		m_segmentTimer = 0.0f;

		m_segmentTravelTime = 1.0f;
		m_mode = PathSampler::PathMode::LERP;
	}

	void CPathAnimator::SetMode(PathSampler::PathMode mode)
	{
		m_mode = mode;
		m_segmentIndex = 0;
	}

	void CPathAnimator::Update(const PathSampler::KeypointSet& keypoints, float deltaTime)
	{
		// TODO: Implement this function
		// Steps to follow:

		if (keypoints.size() <= m_segmentIndex) {
			m_segmentTimer = 0;
			return;
		}

		m_segmentTimer += 0.001f;

		if (m_segmentTimer >= 1)
		{
			m_segmentTimer = 0;
			m_segmentIndex += 1;

		}

		if (m_segmentIndex + 1 >= keypoints.size())
		{
			m_segmentIndex = 0;
		}

		m_segmentTimer *= m_segmentTravelTime;

		if (keypoints.size() < 2) {
			m_segmentTimer = 0;

			return;
		}



		m_owner->transform.m_pos = glm::vec3(PathSampler::LERP(keypoints[m_segmentIndex]->transform.m_pos, keypoints[m_segmentIndex + 1]->transform.m_pos, m_segmentTimer ));
	}
}