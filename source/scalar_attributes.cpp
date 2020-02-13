#include "scalar_attributes.h"
#include <algorithm>

float ScalarAttributes::getMin()
{
	float minimum = m_values[0];
	for(int i = 1; i < m_values.size(); ++i)
    {
	   minimum = std::min(minimum, m_values[i]);
    }

	return minimum;
}

float ScalarAttributes::getMax()
{
	float maximum = m_values[0];
	for(int i = 1; i < m_values.size(); ++i)
    {
	   maximum = std::max(maximum, m_values[i]);
    }

	return maximum;
}